#ifndef __SHARED_JOB_QUEUE_H
#define __SHARED_JOB_QUEUE_H

#include <Windows.h>
#include <queue>

#include "IJob.h"
#include "INamed.h"

namespace Engine
{
namespace JobSystem
{
class SharedJobQueue
	: public INamed
{
public:
	SharedJobQueue();

	inline void				Shutdown();
	inline bool				ShutdownRequested() const;

	bool					Add( IJob & i_Job );
	IJob *					GetWhenAvailable();
private:
	std::queue<IJob *>		m_Jobs;

	CONDITION_VARIABLE		m_WakeAndCheck;
	CRITICAL_SECTION		m_QueueAccess;

	bool					m_ShutdownRequested;
};

} // namespace JobSystem
} // namespace Engine

#include "SharedJobQueue-inl.h"
#endif // __SHARED_JOB_QUEUE_H