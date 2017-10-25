#ifndef __JOB_RUNNER_H
#define __JOB_RUNNER_H

#include "SharedJobQueue.h"

namespace Engine
{
namespace JobSystem
{

struct JobRunnerInput
{
	SharedJobQueue *	i_pQueue;
#ifdef _DEBUG
	std::string			m_ThreadName;
#endif
};

DWORD WINAPI JobRunner( void * i_pJobRunnerInput );

} // namespace JobSystem
} // namespace Engine

#endif // __JOB_RUNNER_H