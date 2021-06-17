#ifndef GAME_STRUCT_H
#define GAME_STRUCT_H

#include <glad/glad.h>
#include <cinttypes>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

namespace BreakOut
{
	struct Sprite
	{
		glm::vec2 position;
		glm::vec2 size;
		glm::vec4 color;
	};

	struct Level
	{
		std::vector<Sprite> solidBlocks;
		std::vector<Sprite> destructableBlocks;
		size_t remainingBlocks;
	};

	struct MovingObjects : Sprite
	{
		glm::vec2 velocity;
	};

	struct Screen
	{
		int32_t height, width;
	};

	struct Character
	{
		uint32_t texture;
		glm::ivec2 size;
		glm::ivec2 bearing;
		uint32_t advance;
	};

}
#endif //GAME_STRUCT_H
