#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <Windows.h>

#include "EngineOperation.h"
#include "BasicActor.h"
#include "ControllerFactory.h"
#include "PhysicActor.h"
#include "CollisionSystem.h"
#include "SimplePhysicSystem.h"
#include "SimpleRenderingSystem.h"
#include "AllocatorOperation.h"
#include "Vector.h"

#include "GLib\GLib.h"
#include "Utilities\Timing.h"
#include "Utilities\MyOwnAssert.h"


#define READ_BOUND_BOX(Vertex_Point) \
	float Vertex_Point[2] = { 0.0f };\
	sizeRead = Lua::LoadFloatArray(pLuaState, -1, Vertex_Point, 2);\
	ASSERT(sizeRead == 2, "wrong size read!");\
	lua_pop(pLuaState, 1);\



namespace Engine {
	EngineWorld * g_World = nullptr;
	StrongPtr<BasicActor> CreateActor(char * name, const CYNData::Vector3D & position, AABB boundingBox, PhysicInfo * physic, GLib::Sprites::Sprite * sprite, GLib::Texture * spTexture, const ControllerInfo & controller_info) {
		StrongPtr<BasicActor> strptr_Actor(new BasicActor(name, position.X(), position.Y(),boundingBox));

		Engine::GetEngine()->addActor(strptr_Actor);

		StrongPtr<PhysicActor> physicActor = nullptr;
		physicActor = Engine::GetEngine()->getPhysics()->registerActor(strptr_Actor, physic);
		if (physicActor != nullptr) {
			Engine::GetEngine()->getCollider()->registerCollision(strptr_Actor, physicActor);
		}
		Engine::GetEngine()->getRenderer()->registerActor(strptr_Actor, sprite,spTexture);
		
		ActorController * controller_created = nullptr;
		controller_created = ControllerFactory::CreateController(controller_info);
		ControllerFactory::RegisterController(controller_created, &physicActor);
		return strptr_Actor;
	}
	StrongPtr<BasicActor> CreateActorFromLua(const char * luaFile, const CYNData::Vector3D & position) {
		
		lua_State * pLuaState = luaL_newstate();
		ASSERT(true, "getLuaState Failed!");
		luaL_openlibs(pLuaState);

		size_t sizeFile = 0;
		void * pLuaFile = File::LoadFile(luaFile, sizeFile);
		if (pLuaFile && sizeFile) {
			int result = 0;

			result = luaL_loadbuffer(pLuaState, reinterpret_cast<char *>(pLuaFile), sizeFile, "lua_initial_chunk");
			ASSERT(result == 0, "load chunk error!");

			lua_pcall(pLuaState, 0, result, 0);
			ASSERT(result == 0, "calling LuaFunc error!");

			int type = LUA_TNIL;
			result = lua_getglobal(pLuaState, "Actor");
			ASSERT(result == LUA_TTABLE, "Actor is not a table. Check Lua File.");

			lua_pushstring(pLuaState, "name");
			type = lua_gettable(pLuaState, -2);
			ASSERT(type == LUA_TSTRING, "Wrong data type fetched.");

			const char * actorName = lua_tostring(pLuaState,-1);
			ASSERT(actorName, "No name found!");
			if(actorName)
				actorName = _strdup(actorName);

			lua_pop(pLuaState, 1);

			lua_pushstring(pLuaState, "initial_position");
			type = lua_gettable(pLuaState, -2);
			ASSERT(type == LUA_TTABLE, "Wrong data type fetched.");
			float data_position[2] = { 0.0f };
			size_t sizeRead = Lua::LoadFloatArray(pLuaState, -1, data_position, 2);
			ASSERT(sizeRead == 2, "wrong size read!");
			lua_pop(pLuaState, 1);

			lua_pushstring(pLuaState, "player_controller");
			type = lua_gettable(pLuaState, -2);
			ASSERT(type == LUA_TSTRING, "Wrong data type fetched.");

			ControllerInfo * controller = nullptr;
			const char * controller_name = lua_tostring(pLuaState, -1);
			lua_pop(pLuaState, 1);
			if (HashString(controller_name)!=HashString("")) {
				controller_name = _strdup(controller_name);

				lua_pushstring(pLuaState, "controller_info");
				type = lua_gettable(pLuaState, -2);
				ASSERT(type == LUA_TTABLE, "Wrong data type fetched.");
				float data_controller[2] = { 0.0f };
				size_t sizeRead = Lua::LoadFloatArray(pLuaState, -1, data_controller, 2);
				ASSERT(sizeRead == 2, "wrong size read!");
				lua_pop(pLuaState, 1);
				
				controller = new ControllerInfo(controller_name, data_controller);
			}
			else
				controller_name = nullptr;

			lua_pushstring(pLuaState, "bounding_box");
			type = lua_gettable(pLuaState, -2);
			ASSERT(type == LUA_TTABLE, "Wrong data type fecteched");

			lua_pushstring(pLuaState, "center");
			type = lua_gettable(pLuaState, -2);
			ASSERT(type == LUA_TTABLE, "Wrong data type fecteched");
			READ_BOUND_BOX(center)

			lua_pushstring(pLuaState, "extends");
			type = lua_gettable(pLuaState, -2);
			ASSERT(type == LUA_TTABLE, "Wrong data type fecteched");
			READ_BOUND_BOX(extends)

			lua_pop(pLuaState, 1);

			PhysicInfo * physic_info = nullptr;
			lua_pushstring(pLuaState, "physics_settings");
			type = lua_gettable(pLuaState, -2);
			if (type == LUA_TTABLE) {
				lua_pushstring(pLuaState, "mass");
				type = lua_gettable(pLuaState, -2);
				ASSERT(type == LUA_TNUMBER, "Wrong data type fetched.");
				lua_Number mass = lua_tonumber(pLuaState, -1);
				lua_pop(pLuaState, 1);

				lua_pushstring(pLuaState, "drag");
				type = lua_gettable(pLuaState, -2);
				ASSERT(type == LUA_TNUMBER, "Wrong data type fetched.");
				lua_Number drag = lua_tonumber(pLuaState, -1);
				lua_pop(pLuaState, 1);

				lua_pushstring(pLuaState, "rotation_ratio");
				type = lua_gettable(pLuaState, -2);
				ASSERT(type == LUA_TNUMBER, "Wrong data type fetched.");
				lua_Number rotation_ratio = lua_tonumber(pLuaState, -1);
				lua_pop(pLuaState, 1);

				physic_info = new PhysicInfo((float)mass, (float)drag, (float)rotation_ratio);
			}
			lua_pop(pLuaState, 1);

			lua_pushstring(pLuaState, "render_settings");
			type = lua_gettable(pLuaState, -2);
			ASSERT(type == LUA_TTABLE, "Wrong data type fetched.");

			lua_pushstring(pLuaState, "sprite");
			type = lua_gettable(pLuaState, -2);
			ASSERT(type == LUA_TSTRING, "Wrong data type fetched.");
			const char * spriteName = lua_tostring(pLuaState,-1);
			if (spriteName)
				spriteName = _strdup(spriteName);

			lua_pop(pLuaState, 1);
			lua_pop(pLuaState, 1);
			//pop out the initial 
			lua_pop(pLuaState, 1);
			int luaIndex = lua_gettop(pLuaState);
			if (luaIndex == 0) {
				Log("Finished");
			}
			lua_close(pLuaState);

			GLib::Texture * SpriteTexture = nullptr;
			GLib::Sprites::Sprite * sprite = Engine::File::CreateSprite(spriteName, SpriteTexture);
			CYNData::Vector3D initPos(data_position[0], data_position[1]);

			CYNData::Vector3D vec_center(center[0],center[1]);
			CYNData::Vector3D vec_extends(extends[0],extends[1]);

			AABB bounding_box = AABB(vec_center, vec_extends);
			StrongPtr<BasicActor> actorCreated = CreateActor(const_cast<char *>(actorName), position, bounding_box, physic_info, sprite, SpriteTexture,*controller);
			
			if (controller) {
				delete controller;
			}
			if (physic_info) {
				delete physic_info;
			}
			free(const_cast<char *>(controller_name));
			free(const_cast<char *>(actorName));
			free(const_cast<char *>(spriteName));
			delete[] pLuaFile;
			return actorCreated;
		}
		return nullptr;
	}
	
	bool InitializeWindow(HINSTANCE i_hInstance, int i_CmdShow, const char * i_pWindowName, WORD i_IconID, unsigned int i_WindowWidth, unsigned int i_WindowHeight) {
		bool bSuccess = GLib::Initialize(i_hInstance, i_CmdShow, i_pWindowName, i_IconID, i_WindowWidth, i_WindowHeight);
		return bSuccess;
	}
	void ClearData() {
		Engine::GetEngine()->cleanSelf();
	}
	void ShutDown() {
		Engine::GetEngine()->cleanSelf();
		delete g_World;

		GLib::Shutdown();
		Engine::Memory::MemoryClear();
	}
	void UpdateControllers(float dt) {
		StrongPtr<PlayerController> controller = Engine::GetEngine()->GetActorController();
		if (controller != nullptr) {
			controller->UpdateActor(dt);
		}
	}
	void UpdatePhysicStatus(float dt) {
		GetEngine()->getPhysics()->run(dt);
	}
	void Draw(float dt) {
		GetEngine()->getRenderer()->draw(dt);
	}
	void UpdateCollisionInfo(float dt) {
		GetEngine()->getCollider()->updateCollideInfo(dt);
	}
	void CollisionCheck(float dt) {
		GetEngine()->getCollider()->CheckCollide(dt);
	}
	void SetCollisionHandler(CollisionSystem::CollisionHandler_t handler) {
		Engine::GetEngine()->getCollider()->SetCollisionHandler(handler);
	}
	
	EngineWorld * GetEngine() {
		if (!g_World) {
			g_World = new EngineWorld();
		}
		return g_World;
	}

	namespace File {
			void * LoadFile(const char * i_pFilename, size_t & o_sizeFile) {
				assert(i_pFilename != NULL);

				FILE * pFile = NULL;

				errno_t fopenError = fopen_s(&pFile, i_pFilename, "rb");
				if (fopenError != 0)
					return NULL;

				assert(pFile != NULL);

				int FileIOError = fseek(pFile, 0, SEEK_END);
				assert(FileIOError == 0);

				long FileSize = ftell(pFile);
				assert(FileSize >= 0);

				FileIOError = fseek(pFile, 0, SEEK_SET);
				assert(FileIOError == 0);

				uint8_t * pBuffer = new uint8_t[FileSize];
				assert(pBuffer);

				size_t FileRead = fread(pBuffer, 1, FileSize, pFile);
				assert(FileRead == FileSize);

				fclose(pFile);

				o_sizeFile = FileSize;

				return pBuffer;
			}
			
			GLib::Sprites::Sprite * CreateSprite(const char * i_pFilename, GLib::Texture * & o_pTextureUsed)
			{
				assert(i_pFilename);

				size_t sizeTextureFile = 0;

				// Load the source file (texture data)
				void * pTextureFile = File::LoadFile(i_pFilename, sizeTextureFile);

				// Ask GLib to create a texture out of the data (assuming it was loaded successfully)
				GLib::Texture * pTexture = pTextureFile ? GLib::CreateTexture(pTextureFile, sizeTextureFile) : nullptr;

				// exit if something didn't work
				// probably need some debug logging in here!!!!
				if (pTextureFile)
					delete[] pTextureFile;

				if (pTexture == nullptr)
					return NULL;

				unsigned int width = 0;
				unsigned int height = 0;
				unsigned int depth = 0;

				// Get the dimensions of the texture. We'll use this to determine how big it is on screen
				bool result = GLib::GetDimensions(pTexture, width, height, depth);
				assert(result == true);
				assert((width > 0) && (height > 0));

				// Define the sprite edges
				GLib::Sprites::SpriteEdges	Edges = { -float(width / 2.0f), float(height), float(width / 2.0f), 0.0f };
				GLib::Sprites::SpriteUVs	UVs = { { 0.0f, 0.0f },{ 1.0f, 0.0f },{ 0.0f, 1.0f },{ 1.0f, 1.0f } };
				GLib::RGBA							Color = { 255, 255, 255, 255 };

				// Create the sprite
				GLib::Sprites::Sprite * pSprite = GLib::Sprites::CreateSprite(Edges, 0.1f, Color, UVs);
				if (pSprite == nullptr)
				{
					GLib::Release(pTexture);
					return nullptr;
				}

				// Bind the texture to sprite
				GLib::Sprites::SetTexture(*pSprite, *pTexture);
				o_pTextureUsed = pTexture;
				return pSprite;
			}

		}
	namespace Input {

	}
	namespace Lua {
		size_t LoadFloatArray(lua_State * i_pLuaState, int i_indexOnStack, float * o_pFloatArray, size_t i_numbersWanted) {
			ASSERT(i_pLuaState, "Invalid i_pLuaState!");
			ASSERT(lua_gettop(i_pLuaState) >= -i_indexOnStack, "Invalid position.");
			ASSERT(lua_type(i_pLuaState,i_indexOnStack) == LUA_TTABLE, "Wrong data type fetched.");

			size_t index = 0;
			lua_pushnil(i_pLuaState);

			while (lua_next(i_pLuaState, i_indexOnStack - 1)) {
				if (lua_type(i_pLuaState, -1) == LUA_TNUMBER) {
					lua_Number number = lua_tonumber(i_pLuaState, -1);
					o_pFloatArray[index] = (float)number;
				}
				else {
					ASSERT(false, "Expect a proper number here.");
				}

				lua_pop(i_pLuaState, 1);
				if (++index == i_numbersWanted) {
					lua_pop(i_pLuaState, 1);
					break;
				}
			}
			return index;
		}
		size_t LoadIntArray(lua_State * i_pLuaState, int i_indexOnStack, int * o_pFloatArray, size_t i_numbersWanted) {
			ASSERT(i_pLuaState, "Invalid i_pLuaState!");
			ASSERT(lua_gettop(i_pLuaState) >= -i_indexOnStack, "Invalid position.");
			ASSERT(lua_type(i_pLuaState, i_indexOnStack) == LUA_TTABLE, "Wrong data type fetched.");

			size_t index = 0;
			lua_pushnil(i_pLuaState);

			while (lua_next(i_pLuaState, i_indexOnStack - 1)) {
				if (lua_type(i_pLuaState, -1) == LUA_TNUMBER) {
					lua_Number number = lua_tonumber(i_pLuaState, -1);
					o_pFloatArray[index] = (int)number;
				}
				else {
					ASSERT(false,"Expect proper number here.")
				}

				lua_pop(i_pLuaState, 1);
				if (++index == i_numbersWanted) {
					lua_pop(i_pLuaState, 1);
					break;
				}
			}
			return index;
		}
	}
}