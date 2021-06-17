#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cinttypes>
#include <fstream>
#include <sstream>
#include "gameStructs.h"
#include "ResourceLoader.h"
#include "Renderer.h"
#include "Particles.h"
#include "GL.h"
#include <Windows.h>

namespace BreakOut
{
	enum class GameState
	{
		RESETING,
		MAIN_MENU,
		GAME_RUNNING,
		GAME_BALL_STUCK,
		GAME_PAUSED,
		GAMEOVER,
		WIN
	} state, previous_sate;

	Level currentLevel;
	MovingObjects  ball, paddle;
	Sprite arrow;
	Screen screen;
	GLFWwindow* window;
	constexpr float PADDLE_HORIZONTAL_SPEED = 1000.f;
	const float PI = 4 * atan(1);
	const float BALL_INITIAL_SPEED = 400.f;
	double frameStartTime, dtime, shakeTime, resetingTime = 0.1;
	int32_t shake;
	uint32_t lives;
	float arrowTheta = 0;
	float arrowBallDistance = 100;
	float arrowSpeed = 2.f;
	namespace ParticlesGenerator
	{
		Particle particles[totalParticles];
		uint32_t aliveParticles = 0;
	}
	bool forceStop = false;
	int32_t current_level_count = 0;
	constexpr int32_t MAX_LEVELS_COUNT = 4;
	constexpr double frameCap = 1.0 / 60.0;

	void InitializeWindow(int32_t windowWidth, int32_t windowHeight, std::string name)
	{
		screen.width = windowWidth;
		screen.height = windowHeight;
		if (!glfwInit())
			exit(-1);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		window = glfwCreateWindow(screen.width, screen.height, name.c_str(), NULL, NULL);
		if (window == nullptr)
		{
			std::cout << "Failed to ResourceLoader GLFW window" << std::endl;
			glfwTerminate();
			exit(-1);
		}
		glfwMakeContextCurrent(window);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			exit(-1);
		}
		glViewport(0, 0, screen.width, screen.height);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		srand(glfwGetTime());
	}

	void Reset()
	{
		float paddleWidht = 200.f;
		float paddleHeight = 40.f;
		paddle.size = glm::vec2(paddleWidht, paddleHeight);
		paddle.position = glm::vec2((static_cast<float>(screen.width) - paddleWidht) / 2.0f,
											static_cast<float>(screen.height) - paddleHeight - 10);
		paddle.color = glm::vec4(1.f,1.f,1.f,1.f);
		//set the ball sprite
		float ballRadius = 40.f;
		ball.size = glm::vec2(ballRadius);
		ball.position = glm::vec2((static_cast<float>(screen.width) - ballRadius) / 2.0f,
											static_cast<float>(screen.height) - ballRadius - paddleHeight - 10);
		//ball.color = glm::vec4(1.f,1.f,1.f,1.f);
		ball.color = glm::vec4(255.f, 0.f, 84.f, 255.f) / 255.f;
	}

	void InitializeGame()
	{
		//load level
		lives = 3;
		currentLevel = ResourceLoader::LoadLevel(current_level_count);
		Reset();
		arrow.color = glm::vec4(1.f);
		arrow.size = ball.size;
		//set the state of the game
		state = GameState::MAIN_MENU;
		//initialize renderer
		Renderer::InitRenderer();
	}

	void ShakeEffect()
	{
		shake = 1;
		shakeTime = 0.1;
	}

	void HandleInGameInput()
	{
		bool isLeftPressed = glfwGetKey(window, GLFW_KEY_LEFT);
		bool isRightPressed = glfwGetKey(window, GLFW_KEY_RIGHT);
		if (isLeftPressed || isRightPressed)
		{
			if (isLeftPressed)
			{
				paddle.velocity.x = -PADDLE_HORIZONTAL_SPEED;
			}
			else
			{
				paddle.velocity.x = PADDLE_HORIZONTAL_SPEED;
			}
		}
		else
		{
			paddle.velocity.x = 0;
		}
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			previous_sate = state;
			state = GameState::GAME_PAUSED;
		}
	}

	void UpdatePaddle()
	{
		paddle.position += static_cast<float>(dtime) * paddle.velocity;
		if (paddle.position.x < 0 )
		{
			paddle.position.x = 0;
		}
		if ( paddle.position.x > screen.width - paddle.size.x)
		{
			paddle.position.x = screen.width - paddle.size.x;
		}
	}

	void UpdateStuckBall()
	{
		ball.position.x = paddle.position.x + (paddle.size.x  - ball.size.x ) / 2.f;
	}

	void UpdateBall()
	{
		ball.position += static_cast<float>(dtime) * ball.velocity;
		if (ball.position.x < 0)
		{
			ball.position.x = 0;
			ball.velocity.x *= -1;
		}
		if ( ball.position.x > screen.width - ball.size.x)
		{
			ball.position.x = screen.width - ball.size.x;
			ball.velocity.x *= -1;
		}
		if (ball.position.y < 0)
		{
			ball.position.y = 0;
			ball.velocity.y *= -1;
		}
		if ( ball.position.y > screen.height - ball.size.y)
		{
			// player dies
			state = GameState::RESETING;
			--lives;
			if (lives == 0)
			{
				//GAME OVER
				ShakeEffect();
				state = GameState::GAMEOVER;
			}
		}
	}

	void UpdateShakeEffect()
	{
		if (shake)
		{
			shakeTime -= dtime;
			if (shakeTime <= 0)
			{
				shake = false;
			}
		}
	}

	void CheckCollision()
	{
		// ball solid/solid blocks collision
		glm::vec2 ballCenter = ball.position + ball.size / 2.0f;
		float ballRadius = ball.size.x / 2.0f;
		for (auto& block : currentLevel.solidBlocks)
		{
			glm::vec2 recCenterPos = block.position + block.size / 2.0f;
			glm::vec2 diff = ballCenter - recCenterPos;
			glm::vec2 P = glm::clamp(diff, -block.size/ 2.0f, block.size / 2.0f) + recCenterPos;
			diff = P - ballCenter;
			float distance = glm::length(diff);
			if (distance <= ballRadius)
			{
				//collision
				ball.position = P + ballRadius * glm::normalize(-diff) - glm::vec2(ballRadius);
				if (P.x == block.position.x)
				{
					ball.velocity.x = -abs(ball.velocity.x);
				}
				if (P.x == block.position.x + block.size.x)
				{
					ball.velocity.x = abs(ball.velocity.x);
				}
				if (P.y == block.position.y)
				{
					ball.velocity.y = -abs(ball.velocity.y);
				}
				if (P.y == block.position.y + block.size.y)
				{
					ball.velocity.y = abs(ball.velocity.y);
				}
				ShakeEffect();
			}
		}
		//ball/destructable blocks collision
		for (int blockIndex = 0; blockIndex < currentLevel.remainingBlocks;)
		{
			Sprite& block = currentLevel.destructableBlocks[blockIndex];
			glm::vec2 recCenterPos = block.position + block.size / 2.0f;
			glm::vec2 diff = ballCenter - recCenterPos;
			glm::vec2 P = glm::clamp(diff, -block.size/ 2.0f, block.size / 2.0f) + recCenterPos;
			diff = P - ballCenter;
			float distance = glm::length(diff);
			if (distance <= ballRadius)
			{
				//collision
				ball.position = P + ballRadius * glm::normalize(-diff) - glm::vec2(ballRadius);
				if (P.x == block.position.x)
				{
					ball.velocity.x = -abs(ball.velocity.x);
				}
				else if (P.x == block.position.x + block.size.x)
				{
					ball.velocity.x = abs(ball.velocity.x);
				}
				if (P.y == block.position.y)
				{
					ball.velocity.y = -abs(ball.velocity.y);
				}
				else if (P.y == block.position.y + block.size.y)
				{
					ball.velocity.y = abs(ball.velocity.y);
				}
				--currentLevel.remainingBlocks;
				std::swap(block, currentLevel.destructableBlocks[currentLevel.remainingBlocks]);
				ShakeEffect();
				if (currentLevel.remainingBlocks == 0 )
				{
					//Level cleared
					++current_level_count;
					if (current_level_count < MAX_LEVELS_COUNT)
					{
						//there is still some levels
						state = GameState::RESETING;
						currentLevel = ResourceLoader::LoadLevel(current_level_count);
					}
					else
					{
						//WIN
						state = GameState::WIN;
					}
				}
			}
			else blockIndex++;
		}
		//ball/paddle collision
		glm::vec2 recCenterPos = paddle.position + paddle.size / 2.0f;
		glm::vec2 diff = ballCenter - recCenterPos;
		glm::vec2 P = glm::clamp(diff, -paddle.size/ 2.0f, paddle.size / 2.0f) + recCenterPos;
		diff = P - ballCenter;
		float distance = glm::length(diff);
		if (distance <= ballRadius)
		{
			//collision
			ball.position = P + ballRadius * glm::normalize(-diff) - glm::vec2(ballRadius);
			if (P.x == paddle.position.x || P.x == paddle.position.x + paddle.size.x)
			{
				ball.velocity.x *= -1.0f;
			}
			if (P.y == paddle.position.y)
			{
				ball.velocity.y = -abs(ball.velocity.y);
			}
			float ballSpeed = glm::length(ball.velocity);
			ball.velocity = glm::normalize(0.9f*ball.velocity + 0.1f*paddle.velocity) * ballSpeed;
			ShakeEffect();
		}
	}

	void UpdateArrow()
	{
		arrowTheta += dtime * arrowSpeed;
		if (arrowTheta < PI / 6)
		{
			arrowTheta = PI / 6;
			arrowSpeed *= -1;
		}
		if (arrowTheta >= 5 * PI / 6)
		{
			arrowTheta = 5 * PI / 6;
			arrowSpeed *= -1;
		}
		arrowBallDistance += 50.f * cos(glfwGetTime());
	}

	void Run()
	{
		glfwSwapInterval(0);
		//main application loop
		while (!glfwWindowShouldClose(window) && !forceStop)
		{
			dtime = glfwGetTime() - frameStartTime;
			frameStartTime = glfwGetTime();
			glClear(GL_COLOR_BUFFER_BIT);
			switch (state) {
			case GameState::GAME_RUNNING:
			{
				HandleInGameInput();
				UpdatePaddle();
				UpdateBall();
				UpdateShakeEffect();
				CheckCollision();
				ParticlesGenerator::RespawnParticles();
				ParticlesGenerator::Update();
				Renderer::InGameRender();
			} break;
			case GameState::GAME_BALL_STUCK:
			{
				HandleInGameInput();
				UpdatePaddle();
				UpdateStuckBall();
				UpdateArrow();
				Renderer::InGameRender();
				Renderer::StuckRender();
				Renderer::RenderArrow(-arrowTheta,100.f);
				if (glfwGetKey(window, GLFW_KEY_SPACE))
				{
					state = GameState::GAME_RUNNING;
					float theta = - arrowTheta;
					ball.velocity = BALL_INITIAL_SPEED * glm::vec2(cos(theta), sin(theta));
				}
			} break;
			case GameState::GAME_PAUSED:
			{
				Renderer::InGameRender();
				Renderer::PauseGameRender();
				if (glfwGetKey(window, GLFW_KEY_ENTER))
				{
					state = previous_sate;
				}
				if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
				{
					forceStop = true;
				}
			} break;
			case GameState::MAIN_MENU:
			{
				Renderer::MainMenuRender();
				if (glfwGetKey(window, GLFW_KEY_ENTER))
				{
					state = GameState::GAME_BALL_STUCK;
				}
				if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
				{
					forceStop = true;
				}
			} break;
			case GameState::RESETING:
			{
				glClear(GL_COLOR_BUFFER_BIT);
				resetingTime -= dtime;
				if (resetingTime <= 0.)
				{
					Reset();
					state = GameState::GAME_BALL_STUCK;
					ParticlesGenerator::ClearAll();
				}
			} break;
			case GameState::GAMEOVER:
			{
				Renderer::InGameRender();
				Renderer::GameOverRender();
				if (glfwGetKey(window, GLFW_KEY_ENTER))
				{
					Reset();
					state = GameState::GAME_BALL_STUCK;
					ParticlesGenerator::ClearAll();
					lives = 3;
					currentLevel = ResourceLoader::LoadLevel(current_level_count);
					shake = 0;
				}
				if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
				{
					forceStop = true;
				}
			} break;
			case GameState::WIN:
			{
				Renderer::InGameRender();
				Renderer::WinRender();
				if (glfwGetKey(window, GLFW_KEY_ENTER))
				{
					Reset();
					state = GameState::GAME_BALL_STUCK;
					ParticlesGenerator::ClearAll();
					lives = 3;
					current_level_count = 0;
					currentLevel = ResourceLoader::LoadLevel(current_level_count);
				}
				if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
				{
					forceStop = true;
				}
			} break;
			default: break;
			}
			//Check events and swap buffers
			glfwSwapBuffers(window);
			glfwPollEvents();
			//lock frame rate
			double now = glfwGetTime();
			double frameTime = now - frameStartTime;
			double dur = 1000.0 * ((frameStartTime + frameCap) - glfwGetTime());
			DWORD durDW = (dur > 0.0)? (DWORD)dur:0;
			while (durDW > 5)
			{
				Sleep(durDW - 5);
				dur = 1000.0 * ((frameStartTime + frameCap) - glfwGetTime()) + 0.5;
				durDW = (dur > 0.0) ? (DWORD)dur : 0;
			}
			while (glfwGetTime() < frameStartTime + frameCap);
		}
	}
}


int main()
{
    BreakOut::InitializeWindow(1600, 900, "OutBreak");
	BreakOut::InitializeGame();
    BreakOut::Run();
    return 0;
}
