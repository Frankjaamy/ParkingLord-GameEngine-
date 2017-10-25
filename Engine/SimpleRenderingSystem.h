#pragma once
#include <stdlib.h>
#include <vector>

#include "GLib\GLib.h"

#include "SmartPointer.h"
#include "JobSystem.h"
#include "IJob.h"
class BasicActor;
struct RenderInfo
{
	WeakPtr<BasicActor> m_weakPtr_actor;
	GLib::Sprites::Sprite * m_pSprite;
	GLib::Texture * m_pTexture;
	RenderInfo(const StrongPtr<BasicActor> & actor, GLib::Sprites::Sprite * sp, GLib::Texture * spTexture):m_weakPtr_actor(actor),m_pSprite(sp),m_pTexture(spTexture) {

	}

	~RenderInfo() {
		//GLib::Release(m_pTexture);
		GLib::Sprites::Release(m_pSprite);
	}

};

class SimpleRenderingSystem {
	friend class RenderObjectsJob;
public:
	~SimpleRenderingSystem();
	SimpleRenderingSystem();

	StrongPtr<RenderInfo> registerActor(const StrongPtr<BasicActor> & actor, GLib::Sprites::Sprite * sp, GLib::Texture * spTexture);

	void draw(float dt);
	void clean();
	
	class RenderObjectsJob : public Engine::JobSystem::IJob
	{
	public:
		RenderObjectsJob(SimpleRenderingSystem * renderer,size_t start_index, size_t how_many_to_process):IJob("DrawObjects"), m_pRenderSystem_(renderer),m_render_start_index_(start_index), m_render_number_(how_many_to_process) {

		}
		void Run();
	private:
		size_t m_render_start_index_;
		size_t m_render_number_;

		SimpleRenderingSystem * m_pRenderSystem_;
	};
public:


private:
	std::vector<StrongPtr<RenderInfo>> m_strP_renderInfos;

	std::mutex m_render_mutex_;
};