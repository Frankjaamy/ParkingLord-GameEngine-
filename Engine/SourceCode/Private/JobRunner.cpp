#include "JobRunner.h"
#include <assert.h>
#include "Utilities\MyOwnAssert.h"

namespace Engine
{
namespace JobSystem
{

DWORD WINAPI JobRunner( void * i_pThreadInput )
{
	assert( i_pThreadInput );
	assert( ( (JobRunnerInput *) i_pThreadInput )->i_pQueue );

	JobRunnerInput * pInput = reinterpret_cast<JobRunnerInput *>( i_pThreadInput );
 	SharedJobQueue * pQueue = reinterpret_cast<SharedJobQueue *>( pInput->i_pQueue );

	LogOutPutStandard("JobRunner \"%s\": Starting Queue \"%s\".\n", pInput->m_ThreadName.c_str(), pQueue->GetName() ? pQueue->GetName() : "Unknown" );

	bool bDone = false;

	do
	{
		LogOutPutStandard("JobRunner \"%s\": Acquiring Job.\n", pInput->m_ThreadName.c_str() );
		IJob * pJob = pQueue->GetWhenAvailable();
		if( pJob )
		{
			LogOutPutStandard("JobRunner \"%s\": Starting Job \"%s\" on Processor %d.\n", pInput->m_ThreadName.c_str(), pJob->GetName() ? pJob->GetName() : "Unknown", GetCurrentProcessorNumber() );

			pJob->Run();

			LogOutPutStandard("JobRunner \"%s\": Finished Job \"%s\".\n", pInput->m_ThreadName.c_str(), pJob->GetName() ? pJob->GetName() : "Unknown" );
			delete pJob;

			bDone = pQueue->ShutdownRequested();
		}
		else
			bDone = pQueue->ShutdownRequested();

	} while( bDone == false );

#ifdef _DEBUG
	LogOutPutStandard("JobRunner \"%s\": Shutting down.\n", pInput->m_ThreadName.c_str() );
#endif
	return 0;
}

} // namespace JobSystem
} // namespace Engine
