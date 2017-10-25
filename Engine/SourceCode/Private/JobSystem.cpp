#include "JobSystem.h"

#include <Windows.h>
#include <thread>
#include <map>

#include "Utilities\MyOwnAssert.h"
#include "HashString.h"

#include "JobRunner.h"
#include "SharedJobQueue.h"

namespace Engine
{
namespace JobSystem
{

struct JobRunnerData
{
	HANDLE				m_ThreadHandle;
	DWORD				m_ThreadID;
	JobRunnerInput		m_Input;
};

struct JobQueueData
{
	SharedJobQueue		m_SharedQueue;
	std::vector<JobRunnerData *>	m_Runners;
};

std::map<hashed_string_value, JobQueueData *>	Queues;

void Shutdown()
{
	LogOutPutStandard("Job System: Shutting down.\n" );

	IJob::Shutdown();

	std::vector<HANDLE>	AllThreads;

	std::map<hashed_string_value, JobQueueData *>::iterator iter = Queues.begin();
	while( iter != Queues.end() )
	{

		if( iter->second )
		{
			const size_t count = iter->second->m_Runners.size();
			for( size_t i = 0; i < count; i++ )
			{
				JobRunnerData * pRunner = iter->second->m_Runners[i];
				if( pRunner  && pRunner->m_ThreadHandle != NULL )
						AllThreads.push_back( pRunner->m_ThreadHandle );
			}

			iter->second->m_SharedQueue.Shutdown();
		}
		++iter;
	}

	LogOutPutStandard("Job System: Waiting for Queue runner threads to shut down.\n" );

	DWORD result = WaitForMultipleObjects( static_cast<DWORD>( AllThreads.size() ), &AllThreads[0], TRUE, INFINITE );
	ASSERTNL( result == WAIT_OBJECT_0 );

	iter = Queues.begin();
	while( iter != Queues.end() )
	{
		if( iter->second )
		{
			const size_t count = iter->second->m_Runners.size();
			for( size_t i = 0; i < count; i++ )
			{
				JobRunnerData * pRunner = iter->second->m_Runners[i];
				if( pRunner )
					delete pRunner;
			}

			delete iter->second;
		}

		++iter;
	}
	
	Queues.clear();

}

void AddRunner( JobQueueData & i_QueueData )
{
	size_t runners = i_QueueData.m_Runners.size();

	JobRunnerData * pNewRunner = new JobRunnerData;
	
	pNewRunner->m_Input.i_pQueue = &i_QueueData.m_SharedQueue;
#ifdef _DEBUG
	const size_t	sizeThreadName = 32;
	char			ThreadName[sizeThreadName];
	sprintf_s( ThreadName, sizeThreadName, "%s %d", i_QueueData.m_SharedQueue.GetName(), int( runners + 1 ) );
	pNewRunner->m_Input.m_ThreadName = ThreadName;
#endif

	pNewRunner->m_ThreadHandle = CreateThread( NULL, 0, JobRunner, &pNewRunner->m_Input, CREATE_SUSPENDED, &pNewRunner->m_ThreadID );
	ASSERTNL( pNewRunner->m_ThreadHandle != NULL );

	i_QueueData.m_Runners.push_back( pNewRunner );
	ResumeThread( pNewRunner->m_ThreadHandle );
}

void AddRunner( const HashString & i_QueueName )
{
	std::map<hashed_string_value, JobQueueData *>::iterator existing = Queues.find( i_QueueName.GetHashedValue());
	ASSERTNL( existing != Queues.end() );
	ASSERTNL( existing->second );

	AddRunner( *existing->second );
}

void CreateQueue( const char * i_pName, unsigned int i_numRunners )
{
	ASSERTNL( i_pName );
	
	HashString	HashedName( i_pName );

	ASSERTNL( Queues.find( HashedName.GetHashedValue() ) == Queues.end() );

	JobQueueData * pNewJobQueueData = new JobQueueData;
	pNewJobQueueData->m_SharedQueue.SetName( i_pName );

	Queues[HashedName.GetHashedValue()] = pNewJobQueueData;

	for( unsigned int i = 0; i < i_numRunners; i++ )
		AddRunner( *pNewJobQueueData );
}

void RunJob( IJob & i_Job, const HashString & i_QueueName )
{
	std::map<hashed_string_value, JobQueueData *>::iterator existing = Queues.find( i_QueueName.GetHashedValue());
	ASSERTNL( existing != Queues.end() );
	ASSERTNL( existing->second );

	LogOutPutStandard( "Job System: Adding Job to Queue \"%s\".\n", existing->second->m_SharedQueue.GetName() );
	existing->second->m_SharedQueue.Add( i_Job );
}

DWORD WairForAllJobsDone(const HashString & i_QueueName) {
	std::map<hashed_string_value, JobQueueData *>::iterator existing = Queues.find(i_QueueName.GetHashedValue());
	ASSERTNL(existing != Queues.end());
	ASSERTNL(existing->second);
	size_t handle_size = existing->second->m_Runners.size();
	HANDLE * handles = new HANDLE[handle_size];
	for (int i = 0; i < handle_size; ++i) {
		handles[i] = existing->second->m_Runners[i]->m_ThreadHandle;
	}
	DWORD result = WaitForMultipleObjects(handle_size, handles, true, 3000);
	return result;
}
} // namespace JobSystem
} // namespace Engine

