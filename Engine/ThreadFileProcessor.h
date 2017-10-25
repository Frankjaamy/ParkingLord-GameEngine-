#pragma once
#include <stdint.h>
#include <queue>
#include <Windows.h>

#include "Utilities\MyOwnAssert.h"

#include "Events.h"
#include "Mutex.h"
#include "Semaphore.h"

namespace Engine
{
	namespace Threads {
		class ThreadedFileProcessor {
		public:
			static ThreadedFileProcessor & getInstance();
			void ShutDown();
		public:
			class LoadTask {
			public:
				LoadTask(const char * i_FileName);
				virtual ~LoadTask();

				const char * GetFileName(void) const;
				virtual void ProcessFileContents(uint8_t * i_pBuffer, uint32_t i_BufferBytes) = 0;
			private:
				const char * m_FileName;
			};


			void AddToLoadQueue(LoadTask & i_loadTask);
			LoadTask * getFromLoadQueue();
		private:
			ThreadedFileProcessor(HANDLE i_LoadThreadHandle, HANDLE i_ProcessThreadHandle);
		private:
			struct _LoadedTaskData
			{
				uint8_t * m_pFileContents;
				uint32_t m_FileContentsSize;
				LoadTask * m_pLoadTask;
				_LoadedTaskData(uint8_t * i_pFileContents, uint32_t i_FileContentsSize, LoadTask * i_pLoadTask) :
					m_pFileContents(i_pFileContents),
					m_FileContentsSize(i_FileContentsSize),
					m_pLoadTask(i_pLoadTask)
				{
					ASSERTNL(i_pFileContents != NULL);
					ASSERTNL(i_FileContentsSize > 0);
					ASSERTNL(i_pLoadTask != NULL);
				}

			};
			void AddToProcessQueue(_LoadedTaskData & i_processTask);
			_LoadedTaskData * getFromProcessQueue();
		private:
			static DWORD WINAPI LoadThreadFunction(void * i_pThreadData);
			static DWORD WINAPI ProcessThreadFunction(void * i_pThreadData);

		private:
			std::queue<LoadTask *> m_taskToLoaded;
			std::queue<_LoadedTaskData *> m_tasksToProcess;

			HANDLE					m_LoadThreadHandle;
			HANDLE					m_ProcessThreadHandle;

			ManualResetEvent		m_ShutdownRequestEvent;
			Semaphore				m_LoadQueueSemaphore;
			Mutex					m_LoadQueueMutex;

			Semaphore				m_ProcessQueueSemaphore;
			Mutex					m_ProcessQueueMutex;
		};
	}
}