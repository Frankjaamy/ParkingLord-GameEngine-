#include "BasicActor.h"

#include "SimpleRenderingSystem.h"
#include "EngineWorld.h"
#include "AllocatorOperation.h"
#include "Utilities\Timing.h"
#include "Utilities\MyOwnAssert.h"
using namespace Engine;
SimpleRenderingSystem::SimpleRenderingSystem() {
	
}
SimpleRenderingSystem::~SimpleRenderingSystem() {
	m_strP_renderInfos.clear();
	//std::terminate();
}
void SimpleRenderingSystem::draw(float dt) {
	//add_new_render_info_.join();
#if 1
	GLib::BeginRendering();
	GLib::Sprites::BeginRendering();
	for (std::vector<StrongPtr<RenderInfo>>::reverse_iterator iter = m_strP_renderInfos.rbegin();
		iter != m_strP_renderInfos.rend(); ++iter) {
		StrongPtr<RenderInfo> strptr_RenderInfo = *iter;
		if (strptr_RenderInfo != nullptr) {
			StrongPtr<BasicActor> strptrActor = strptr_RenderInfo->m_weakPtr_actor.Acquire();
			if (strptrActor != nullptr && (!strptrActor->IsDisabled())) {
				GLib::Point2D position;
				position.x = strptrActor->getPosX();
				position.y = strptrActor->getPosY();
				Vector3D rotation = strptrActor->getRotation();
				GLib::Sprites::RenderSprite(*(strptr_RenderInfo->m_pSprite),position, rotation.Z());
			}
		}
	}
	GLib::Sprites::EndRendering();
	GLib::EndRendering();
#endif
#if 0
	size_t size = m_strP_renderInfos.size();
	constexpr size_t threads_to_open = 4;
	static bool b = false;
	if (!b) {
		JobSystem::CreateQueue("Render",1);
		b = true;
	}
	int objects_per_thread = 0;
	if (!(size % threads_to_open)) {
		objects_per_thread = size / threads_to_open;
	}
	else {
		objects_per_thread = size / threads_to_open +1;
	}
	
	RenderObjectsJob * jobs[threads_to_open];

	for (size_t i = 0; i < threads_to_open; ++i) {
		size_t start = i * (objects_per_thread);
		if (start >= size) {
			jobs[i] = nullptr;
			break;
		}
		size_t temp = size - start;
		size_t number = temp > objects_per_thread?objects_per_thread:temp;
		jobs[i] = new RenderObjectsJob(this,start, number);
		JobSystem::RunJob(*jobs[i], "Render");
	}	
	//JobSystem::WairForAllJobsDone(HashString("Render"));
#endif
}
StrongPtr<RenderInfo> SimpleRenderingSystem::registerActor(const StrongPtr<BasicActor> & actor, GLib::Sprites::Sprite * sp, GLib::Texture * spTexture) {
	StrongPtr<RenderInfo> strongPtr(new RenderInfo(actor, sp, spTexture));
	m_strP_renderInfos.push_back(strongPtr);
	return strongPtr;
}
void SimpleRenderingSystem::clean() {
	m_strP_renderInfos.clear();
}

void SimpleRenderingSystem::RenderObjectsJob::Run() {
	//std::lock_guard<std::mutex> gd(m_pRenderSystem_->m_render_mutex_);
	if (m_pRenderSystem_->m_render_mutex_.try_lock()) {
		GLib::BeginRendering();
		GLib::Sprites::BeginRendering();
		for (size_t i = 0; i < m_render_number_;i++) {
			StrongPtr<RenderInfo> strptr_RenderInfo = m_pRenderSystem_->m_strP_renderInfos[m_render_start_index_+i];
			if (strptr_RenderInfo != nullptr) {
				StrongPtr<BasicActor> strptrActor = strptr_RenderInfo->m_weakPtr_actor.Acquire();
				if (strptrActor != nullptr && (!strptrActor->IsDisabled())) {
					GLib::Point2D position;
					position.x = strptrActor->getPosX();
					position.y = strptrActor->getPosY();
					Vector3D rotation = strptrActor->getRotation();
					GLib::Sprites::RenderSprite(*(strptr_RenderInfo->m_pSprite), position, rotation.Z());
				}
			}
		}
		GLib::Sprites::EndRendering();
		GLib::EndRendering();

		m_pRenderSystem_->m_render_mutex_.unlock();
	}
}