#pragma once
#include "SmartPointer.h"
#include "Vector.h"

#include "ControllerFactory.h"
#include "CollisionSystem.h"

#include "GLib\GLib.h"
#include "lua\lua.hpp"

#include "Engine\EngineWorld.h"

struct PhysicInfo;
class BasicActor;

namespace Engine {
	class EngineWorld;
	extern EngineWorld * g_World;

	EngineWorld * GetEngine();

	StrongPtr<BasicActor> CreateActor(char * name, const CYNData::Vector3D & position, AABB boundingBox, PhysicInfo * physic, GLib::Sprites::Sprite * renderer, GLib::Texture * spTexture, const ControllerInfo & controller_info);
	StrongPtr<BasicActor> CreateActorFromLua(const char * luaFile, const CYNData::Vector3D & position);

	bool InitializeWindow(HINSTANCE i_hInstance, int i_CmdShow, const char * i_pWindowName, WORD i_IconID, unsigned int i_WindowWidth, unsigned int i_WindowHeight);
	void ClearData();	
	void ShutDown();
	
	void UpdateControllers(float dt);
	void UpdatePhysicStatus(float dt);
	void UpdateCollisionInfo(float dt);

	void CollisionCheck(float dt);
	void Draw(float dt);

	void SetCollisionHandler(CollisionSystem::CollisionHandler_t handler);

	namespace Lua {
		size_t LoadFloatArray(lua_State * i_pLuaState, int i_indexOnStack, float * o_pFloatArray, size_t i_numbersWanted);
		size_t LoadIntArray(lua_State * i_pLuaState, int i_indexOnStack, int * o_pFloatArray, size_t i_numbersWanted);
	}
	namespace File {
		void * LoadFile(const char * i_pFilename, size_t & o_sizeFile);
		GLib::Sprites::Sprite * CreateSprite(const char * i_pFilename, GLib::Texture * & o_pTextureUsed);
	}
}