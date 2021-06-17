#ifndef RESOURCE_LOADER_H
#define RESOURCE_LOADER_H

#include "gameStructs.h"

namespace BreakOut
{
	namespace ResourceLoader
	{
		Level LoadLevel(int levelId);

		uint32_t LoadTexture(const std::string& name);

		uint32_t LoadShader(const std::string& vertex,const std::string& geometry, const std::string& fragment);
	
		std::vector<Character> LoadCharacters(std::string fonts, uint32_t fontSize);

	}
	extern Screen screen;
	extern uint32_t lives;
}
#endif //RESOURCE_LOADER_H
