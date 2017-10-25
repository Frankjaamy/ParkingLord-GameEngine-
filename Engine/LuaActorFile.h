#pragma once
#include "ThreadFileProcessor.h"
namespace Engine {
	namespace File {
		class LuaActorFile : public Engine::Threads::ThreadedFileProcessor::LoadTask
		{
		public:
			LuaActorFile(const char * i_FileName):Engine::Threads::ThreadedFileProcessor::LoadTask(i_FileName)
			{

			}
			void ProcessFileContents(uint8_t * i_pBuffer, uint32_t i_BufferBytes) override;

		};
	}
}

