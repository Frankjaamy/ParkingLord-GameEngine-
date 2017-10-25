#pragma once
#include <mutex>
#include "GLib\GLib.h"
#include "CollideHandler.h"

namespace Game {
	extern std::mutex g_game_mutex;
	
	void ReadLevelFromLua(const char * luaFile);
	bool Startup();
	void MainLoop();
	void ShutDown();

	void TestKeyCallback(unsigned int i_VKeyID, bool bWentDown);
}

namespace File {
	void * LoadFile(const char * i_pFilename, size_t & o_sizeFile);
}
