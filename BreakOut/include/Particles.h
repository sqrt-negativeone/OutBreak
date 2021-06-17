#ifndef PARTICLES_H
#define PARTICLES_H
#include "gameStructs.h"
namespace BreakOut
{
	extern double dtime;
	extern MovingObjects ball;
	extern Screen screen;
	namespace ParticlesGenerator
	{
		struct Particle
		{
			glm::vec2 position;
			glm::vec2 velocity;
			glm::vec4 color;
			float scale = 10;
			float life = 0.f;
		};
		constexpr uint32_t totalParticles = 10000;
		constexpr uint32_t respawnRate = 2;
		extern Particle particles[totalParticles];
		extern uint32_t aliveParticles;
		void Update();
		void ClearAll();
		void killParticle(uint32_t particleIndex);
		void RespawnParticles();
	};
}
#endif //PARTICLES_H
