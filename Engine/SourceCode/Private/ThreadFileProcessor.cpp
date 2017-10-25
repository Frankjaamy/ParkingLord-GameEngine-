#include "AllocatorOperation.h"
#include "ThreadFileProcessor.h"
#include <cstring>
#include <stdlib.h>

namespace Engine {
	namespace Threads {
		ThreadedFileProcessor::LoadTask::LoadTask(const char * i_pFileName)
		{
			ASSERTNL(i_pFileName);
			m_FileName = _strdup(i_pFileName);
		}

		ThreadedFileProcessor::LoadTask::~LoadTask()
		{
			free(const_cast<char *>(m_FileName));
		}
		const char * ThreadedFileProcessor::LoadTask::GetFileName(void) const
		{
			return m_FileName;
		}

		static ThreadedFileProcessor * m_pInstance = nullptr;
		ThreadedFileProcessor::ThreadedFileProcessor(HANDLE i_LoadThreadHandle, HANDLE i_ProcessThreadHandle) :
			m_ShutdownRequestEvent(L"ShutdownRequestEvent", false),
			m_LoadThreadHandle(i_LoadThreadHandle),
			m_ProcessThreadHandle(i_ProcessThreadHandle),
			m_LoadQueueSemaphore(0,100,L"LoadQueueSemaphore"),
			m_LoadQueueMutex(L"LoadQueueMutex"),
			m_ProcessQueueSemaphore(0, 100,L"ProcessQueueSemaphore"),
			m_ProcessQueueMutex(L"ProcessQueueMutex")
		{
			ResumeThread(m_LoadThreadHandle);
			ResumeThread(m_ProcessThreadHandle);
		}
		ThreadedFileProcessor & ThreadedFileProcessor::getInstance() {
			if (!m_pInstance) {
				DWORD ThreadID = 0;
				HANDLE LoadThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadedFileProcessor::LoadThreadFunction, NULL, CREATE_SUSPENDED, &ThreadID);
				ASSERTNL(LoadThreadHandle != nullptr);

				HANDLE ProcessThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadedFileProcessor::ProcessThreadFunction, NULL, CREATE_SUSPENDED, &ThreadID);
				ASSERTNL(LoadThreadHandle != nullptr);

				m_pInstance = new ThreadedFileProcessor(LoadThreadHandle, ProcessThreadHandle);
			}
			return *m_pInstance;
		}

		DWORD WINAPI ThreadedFileProcessor::LoadThreadFunction(void * i_pThreadData) {
			ThreadedFileProcessor & processor = ThreadedFileProcessor::getInstance();

			OVERLAPPED FileOverlapped;
			FileOverlapped.Offset = FileOverlapped.OffsetHigh = 0;
			FileOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

			HANDLE LoadHandles[] = { processor.m_ShutdownRequestEvent.GetHandle(), processor.m_LoadQueueSemaphore.GetHandle() };
			HANDLE ReadHandles[] = { processor.m_ShutdownRequestEvent.GetHandle(), FileOverlapped.hEvent };

			bool bIsDone = false;
			while (!bIsDone) {
				DWORD LQResult = WaitForMultipleObjects(sizeof(LoadHandles)/sizeof(LoadHandles[0]),LoadHandles,FALSE,INFINITE);
				if (LQResult == WAIT_OBJECT_0) {
					bIsDone = true;
				}
				else if (LQResult == WAIT_OBJECT_0 +1){
					ThreadedFileProcessor::LoadTask * pLoadTask = processor.getFromLoadQueue();
					if (pLoadTask) {
						const char * pFileName = pLoadTask->GetFileName();
						ASSERTNL(pFileName != nullptr);
						Log("Processing %s\n", pFileName);

						const size_t maxChar = 32;
						wchar_t wtext[32];
						mbstowcs_s(const_cast<size_t *>(&maxChar),wtext, pFileName, strlen(pFileName) + 1);//Plus null
						LPWSTR ptr = wtext;

						HANDLE FileHandle = CreateFile(wtext, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
						if (FileHandle == INVALID_HANDLE_VALUE)
						{
							Log("Error Opening File %s : %d", pLoadTask->GetFileName(), GetLastError());
							delete pLoadTask;
						}
						else {
							DWORD FileSize = GetFileSize(FileHandle, NULL);
							ASSERTNL(FileSize != INVALID_FILE_SIZE);

							uint8_t * pFileBuffer = new uint8_t[FileSize];
							ASSERTNL(pFileBuffer != NULL);

							BOOL RFResult = ReadFile(FileHandle, pFileBuffer, FileSize, NULL, &FileOverlapped);
							if ((RFResult != TRUE) && (GetLastError() != ERROR_IO_PENDING))
							{
								ASSERT("Failed reading %s : %d", pFileName, GetLastError());
								delete pLoadTask;
							}
							else
							{
								DWORD RResult = WaitForMultipleObjects(sizeof(ReadHandles) / sizeof(ReadHandles[0]), ReadHandles, FALSE, INFINITE);
								if (RResult == WAIT_OBJECT_0)
								{
									CancelIo(FileHandle);
									delete pLoadTask;
									bIsDone = true;
								}
								else if (RResult == WAIT_OBJECT_0 + 1)
								{
									DWORD BytesRead = 0;
									BOOL GORResult = GetOverlappedResult(FileHandle, &FileOverlapped, &BytesRead, FALSE);
									ASSERTNL(GORResult == TRUE);

									struct _LoadedTaskData * pNewLoadTaskData = new _LoadedTaskData(pFileBuffer, BytesRead, pLoadTask);

									processor.AddToProcessQueue(*pNewLoadTaskData);
								}
								else
								{
									Log("Failed waiting on File I/O %s : %d", pFileName, GetLastError());
									delete pLoadTask;
								}
							}
						}

					}

				}
				else {
					ASSERTNL(false);
				}
			}
			CloseHandle(FileOverlapped.hEvent);
			return 0;
		}
		DWORD WINAPI ThreadedFileProcessor::ProcessThreadFunction(void * i_pThreadData) {
			ThreadedFileProcessor & processor = ThreadedFileProcessor::getInstance();
			HANDLE waitHandles[] = { processor.m_ShutdownRequestEvent.GetHandle(), processor.m_ProcessQueueSemaphore.GetHandle() };
			bool bIsDone = false;
			while (!bIsDone) {
				DWORD result = WaitForMultipleObjects(sizeof(waitHandles) / sizeof(waitHandles[0]), waitHandles, FALSE, INFINITE);
				if (result == WAIT_OBJECT_0) {
					bIsDone = true;
				}
				else if(result == WAIT_OBJECT_0+1){
					ThreadedFileProcessor::_LoadedTaskData * pLoadData = processor.getFromProcessQueue();

					if (pLoadData)
					{
						pLoadData->m_pLoadTask->ProcessFileContents(pLoadData->m_pFileContents, pLoadData->m_FileContentsSize);

						delete [] pLoadData->m_pFileContents;

						delete pLoadData->m_pLoadTask;

						delete pLoadData;
					}
				}
				else {
					ASSERTNL(false);
				}
			}

			return 0;
		}

		void ThreadedFileProcessor::AddToLoadQueue(LoadTask & i_loadTask) {
			m_LoadQueueMutex.Acquire();
			m_taskToLoaded.push(&i_loadTask);
			m_LoadQueueMutex.Release();
			m_LoadQueueSemaphore.Increment();
		}
		void ThreadedFileProcessor::AddToProcessQueue(struct _LoadedTaskData & i_loadedTask) {
			m_ProcessQueueMutex.Acquire();
			m_tasksToProcess.push(&i_loadedTask);
			m_ProcessQueueMutex.Release();
			m_ProcessQueueSemaphore.Increment();
		}

		ThreadedFileProcessor::LoadTask * ThreadedFileProcessor::getFromLoadQueue(void) {
			LoadTask * pLoadTask = nullptr;
			m_LoadQueueMutex.Acquire();
			if (!m_taskToLoaded.empty()) {
				pLoadTask = m_taskToLoaded.front();
				m_taskToLoaded.pop();
			}
			m_LoadQueueMutex.Release();
			return pLoadTask;
		}
		ThreadedFileProcessor::_LoadedTaskData * ThreadedFileProcessor::getFromProcessQueue(void) {
			struct _LoadedTaskData * pLoadTaskData = nullptr;
			m_ProcessQueueMutex.Acquire();
			if (!m_tasksToProcess.empty()) {
				pLoadTaskData = m_tasksToProcess.front();
				m_tasksToProcess.pop();
			}
			m_ProcessQueueMutex.Release();
			return pLoadTaskData;
		}

		void ThreadedFileProcessor::ShutDown(void) {
			if (m_pInstance) {
				ThreadedFileProcessor & self = getInstance();
				self.m_ShutdownRequestEvent.Signal();

				HANDLE ThreadHandles[] = {self.m_LoadThreadHandle, self.m_ProcessThreadHandle};
				DWORD Result = WaitForMultipleObjects(sizeof(ThreadHandles) / sizeof(ThreadHandles[0]), ThreadHandles, TRUE, INFINITE);

				delete m_pInstance;
				m_pInstance = nullptr;
			}
		}


	}
}