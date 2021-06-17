#ifndef RENDERER_H
#define RENDERER_H

#include "gameStructs.h"
#include "ResourceLoader.h"
#include "GL.h"

namespace BreakOut
{
	namespace Renderer
	{
		void InitRenderer();
		void RenderSprite(const Sprite& sprite, uint32_t& spriteShader, uint32_t texture);
		void InGameRender();
		void MainMenuRender();
		void RenderText(const std::string& text, const std::vector<Character>& characters, float x, float y, float scale = 1.0f, glm::vec3 color = glm::vec3(1.f));
		void StuckRender();
		void GameOverRender();
		void PauseGameRender();
		void WinRender();
		void RenderArrow(float theta, float distance);
	}

	extern Screen screen;
	extern Sprite arrow;
	extern Level currentLevel;
	extern MovingObjects  ball, paddle;
	extern int32_t shake;
	extern double frameStartTime;
	extern uint32_t lives;
}
#endif //RENDERER_H
