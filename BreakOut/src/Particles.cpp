#include <Particles.h>

using namespace BreakOut;

void ParticlesGenerator::Update()
{
	//NOTE(Mouad): we loop over all the alive particles and update their life and position and color
	// particles with with negative life are killed
	for (int particleIndex = 0; particleIndex < aliveParticles;)
	{
		Particle &p = particles[particleIndex];
		p.life -= dtime;
		if (p.life > 0)
		{
			p.position += p.velocity * static_cast<float>(dtime);
			p.scale += 10.f * dtime;
			//p.color.a -= dtime * 2.5f;
			++particleIndex;
		}
		else
		{
			killParticle(particleIndex);
		}
	}
}

void ParticlesGenerator::killParticle(uint32_t particleIndex)
{
	--aliveParticles;
	std::swap(particles[particleIndex], particles[aliveParticles]);
}

void ParticlesGenerator::RespawnParticles()
{
	for (int particlesToSpawn = 0; particlesToSpawn < respawnRate; ++particlesToSpawn)
	{
		if (aliveParticles >= totalParticles) return;
		Particle &p = particles[aliveParticles++];
		float rColor = ((rand() % 100) / 100.0f);
		float gColor = ((rand() % 100) / 100.0f);
		float bColor = ((rand() % 100) / 100.0f);
		p.position = glm::vec2(rand() % screen.width, rand() % screen.height);
		glm::vec2 dir = glm::normalize(p.position - ball.position);
		float ballRadius = ball.size.x / 4;
		glm::vec2 ballCenter = ball.position + ball.size / 2.f;
		float dis = glm::length(p.position-ball.position);
		dis -= static_cast<int>(dis / ballRadius) * ballRadius;
		dir *= dis;
		p.position = ballCenter + dir - 5.f;
		p.color = glm::vec4(rColor, gColor, bColor, 1.0f);
		p.velocity = static_cast<float>(rand() % 30) * glm::vec2(0.f, 1.f);
		p.scale = 10.f + (rand() % 10);
		if (glm::dot(ball.velocity, dir) >=0)
		{
			p.life = 0.5f;
		}
		else
		{
			glm::vec2 orth = glm::cross(glm::vec3(ball.velocity, 0.f), glm::cross(glm::vec3(ball.velocity, 0.f), glm::vec3(ball.velocity + glm::vec2(1.0f), 0.f)));
			auto power = abs(glm::dot(orth, dir));
			p.life = ballRadius / (ballRadius - power * power);
			p.life /= 2;
		}
	}
}

void ParticlesGenerator::ClearAll()
{
	aliveParticles = 0;
}
