#ifndef __JOB_SYSTEM_H
#define __JOB_SYSTEM_H

#include <windows.h>
#include <mutex>
#include "HashString.h"

namespace Engine
{
namespace JobSystem
{
class IJob;

void CreateQueue( const char * i_pName, unsigned int i_numRunners );
void AddRunner( const HashString & i_QueueName );

void RunJob( IJob & i_Job, const HashString & i_QueueName );
void Shutdown();

DWORD WairForAllJobsDone(const HashString & i_QueueName);
} // namespace JobSystem
} // namespace Engine

#endif // __JOB_SYSTEM_H