#include "Renderer.h"
#include "Particles.h"
#include <iostream>

using namespace BreakOut;
static uint32_t msFrameBuffer, intermediateFramebuffer, renderBuffer, sceneTexture;
static uint32_t spriteRenderShader, particleShader, postProcessingShader, textShader, colorShader;
//textures
uint32_t ballTexture, paddleTexture, solidBlockTexture, destructableBlockTexture, background, particleTexture, arrowTexture;

static std::vector<Character> brokenWorldCharacters, SweetChiliCharacters;

constexpr float spriteQuadVertices[] = {
	// Position 	texture
	0.0f, 1.0f, 	0.0f, 1.0f,
	1.0f, 0.0f, 	1.0f, 0.0f,
	0.0f, 0.0f, 	0.0f, 0.0f,

	0.0f, 1.0f, 	0.0f, 1.0f,
	1.0f, 1.0f, 	1.0f, 1.0f,
	1.0f, 0.0f, 	1.0f, 0.0f,
};

constexpr float sceneQuadVertices[] = {
	// Position 	texture
	-1.0f, -1.0f, 	 0.0f, 0.0f,
	 1.0f,  1.0f, 	 1.0f, 1.0f,
	-1.0f,  1.0f, 	 0.0f, 1.0f,

	-1.0f, -1.0f, 	 0.0f, 0.0f,
	 1.0f, -1.0f, 	 1.0f, 0.0f,
	 1.0f,  1.0f, 	 1.0f, 1.0f
};

static uint32_t spriteQuadVAO;
static uint32_t sceneQuadVAO;
static uint32_t textVAO;
static uint32_t textVBO;

void SetupShaders()
{
	using namespace GL;
	using namespace Renderer;
	glm::mat4 projection = glm::ortho(0.0F, static_cast<float>(screen.width), static_cast<float>(screen.height), 0.0F, -1.0F, 1.0F);
	// load shaders
	spriteRenderShader 		= ResourceLoader::LoadShader("sprite.glsl", "", "sprite.glsl");
	particleShader 			= ResourceLoader::LoadShader("particles.glsl", "", "particles.glsl");
	postProcessingShader 	= ResourceLoader::LoadShader("postprocess.glsl", "", "postprocess.glsl");
	textShader 				= ResourceLoader::LoadShader("text.glsl", "", "text.glsl");
	colorShader 			= ResourceLoader::LoadShader("transparency.glsl", "", "transparency.glsl");
	//setup sprite renderer shader
	Shader::Use(spriteRenderShader);
	Shader::SetUniform(spriteRenderShader,"image", 0);
	Shader::SetUniform(spriteRenderShader ,"projection", projection);
	//setup particles shader
	Shader::Use(particleShader);
	glm::mat4 textProjection = glm::ortho(0.f, static_cast<float>(screen.width), static_cast<float>(screen.height), 0.f);
	Shader::SetUniform(particleShader,"projection", textProjection);
	Shader::SetUniform(particleShader,"sprite", 0);
	//setup text shader
	Shader::Use(textShader);
	Shader::SetUniform(textShader, "projection", projection);
	Shader::SetUniform(particleShader,"character", 0);
	//setup the post processing shader
	Shader::Use(postProcessingShader);
	Shader::SetUniform(postProcessingShader,"scene", 0);
    float offset = 1.0f / 300.0f;
    float offsets[9][2] = {
        { -offset,  offset  },
        {  0.0f,    offset  },
        {  offset,  offset  },
        { -offset,  0.0f    },
        {  0.0f,    0.0f    },
        {  offset,  0.0f    },
        { -offset, -offset  },
        {  0.0f,   -offset  },
        {  offset, -offset  } 
    };
    glUniform2fv(glGetUniformLocation(postProcessingShader, "offsets"), 9, (float*)offsets);
    float blur_kernel[9] = {
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
        2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
    };
    glUniform1fv(glGetUniformLocation(postProcessingShader, "blur_kernel"), 9, blur_kernel);
}

void SetupVertexArrays()
{
	using namespace GL;
	using namespace Renderer;
	uint32_t quadVBO;
	glGenVertexArrays(1, &spriteQuadVAO);
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBindVertexArray(spriteQuadVAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(spriteQuadVertices), spriteQuadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
	//configure scene quad vertex array buffer
	glGenVertexArrays(1, &sceneQuadVAO);
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBindVertexArray(sceneQuadVAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sceneQuadVertices), sceneQuadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
	//configure text vertex array and buffer
	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 6, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SetupTextCharacters()
{
	brokenWorldCharacters = ResourceLoader::LoadCharacters("Broken World.otf", 100);
	SweetChiliCharacters  = ResourceLoader::LoadCharacters("Market_Deco.ttf", 24);
}

void SetupFrameBuffers()
{
	using namespace GL;
	using namespace Renderer;
	glGenFramebuffers(1, &msFrameBuffer);
	glGenFramebuffers(1, &intermediateFramebuffer);
	glGenRenderbuffers(1, &renderBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, msFrameBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, screen.width, screen.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout<<"ERROR::FRAMEBUFFER::COULDN'T COMPLETE MULTISAMPLE FRAMEBUFFER\n";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, intermediateFramebuffer);
	sceneTexture = Texture::GenerateTexture(screen.width, screen.height, nullptr);
	Texture::SetupTexture();
	Texture::Unbind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout<<"ERROR::FRAMEBUFFER::COULDN'T COMPLETE MULTISAMPLE FRAMEBUFFER\n";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SetupTextures()
{
	ballTexture 				= ResourceLoader::LoadTexture("ball.png");
	paddleTexture 				= ResourceLoader::LoadTexture("paddle.png");
	solidBlockTexture 			= ResourceLoader::LoadTexture("solidBlock.png");
	destructableBlockTexture 	= ResourceLoader::LoadTexture("block.png");
	background 					= ResourceLoader::LoadTexture("grid_bg.png");
	particleTexture 			= ResourceLoader::LoadTexture("particleStar.png");
	arrowTexture 				= ResourceLoader::LoadTexture("arrow.png");
}

void Renderer::InitRenderer()
{
	SetupShaders();
	SetupVertexArrays();
	SetupFrameBuffers();
	SetupTextCharacters();
	SetupTextures();
}

void Renderer::RenderSprite(const Sprite& sprite, uint32_t& spriteShader, uint32_t texture)
{
	using namespace GL;
	//position the sprite
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, glm::vec3(sprite.position, 0.0F));
	model = glm::scale(model, glm::vec3(sprite.size, 1.0F));
	Shader::Use(spriteShader);
	Shader::SetUniform(spriteShader, "model", model);
	Shader::SetUniform(spriteShader, "color", sprite.color);
	//draw the sprite
	glActiveTexture(GL_TEXTURE0);
	Texture::Bind(texture);
	//glBindVertexArray(spriteQuadVAO);
	glBindVertexArray(spriteQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::InGameRender()
{
	using namespace GL;
	// bind to multisample framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, msFrameBuffer);
	glClearColor(0.F, 0.F, 0.F, 1.0F);
	glClear(GL_COLOR_BUFFER_BIT);
	//Draw scene to multisample framebuffers
	glm::mat4 model(1.0F);
	Shader::Use(spriteRenderShader);
	//draw the background
	model = glm::translate(model, glm::vec3(0.0F));
	model = glm::scale(model, glm::vec3(screen.width, screen.height, 1.0F));
	Shader::SetUniform(spriteRenderShader,"model", model);
	Shader::SetUniform(spriteRenderShader,"color", glm::vec3(1.0F));
	glBindVertexArray(spriteQuadVAO);
	glActiveTexture(GL_TEXTURE0);
	Texture::Bind(background);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//draw destructable blocks
	for (uint32_t i = 0; i < currentLevel.remainingBlocks; ++i) {
		RenderSprite(currentLevel.destructableBlocks[i], spriteRenderShader, destructableBlockTexture);
	}
	//draw solid blocks
	for (auto& block : currentLevel.solidBlocks) {
		RenderSprite(block, spriteRenderShader, solidBlockTexture);
	}
	//draw the ball
	RenderSprite(static_cast<Sprite>(ball), spriteRenderShader, ballTexture);
	//draw the paddle
	RenderSprite(static_cast<Sprite>(paddle), spriteRenderShader, paddleTexture);
	//draw particles
	Shader::Use(particleShader);
	using namespace ParticlesGenerator;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	for (int particleIndex = 0; particleIndex < aliveParticles; ++particleIndex)
	{
		Particle &p = particles[particleIndex];
		Shader::SetUniform(particleShader, "offset", p.position);
		Shader::SetUniform(particleShader, "color", p.color);
		Shader::SetUniform(particleShader, "scale", p.scale);
		glActiveTexture(GL_TEXTURE0);
		Texture::Bind(particleTexture);
		glBindVertexArray(spriteQuadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Blit to intermediateFramebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, msFrameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFramebuffer);
	glBlitFramebuffer(0, 0, screen.width, screen.height,
					  0, 0, screen.width, screen.height,
					  GL_COLOR_BUFFER_BIT, GL_NEAREST);
	//use the scene texture to do the post processing magic
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.F, 0.F, 0.F, 1.0F);
	glClear(GL_COLOR_BUFFER_BIT);
	Shader::Use(postProcessingShader);
	Shader::SetUniform(postProcessingShader, "shake", shake);
	Shader::SetUniform(postProcessingShader, "time", static_cast<float>(frameStartTime));
	glActiveTexture(GL_TEXTURE0);
	Texture::Bind(sceneTexture);
	glBindVertexArray(sceneQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	RenderText("lives : " + std::to_string(lives), SweetChiliCharacters, 10, 10);
	//RenderText("frames : " + std::to_string(static_cast<int>(1 / dtime)), SweetChiliCharacters, screen.width - 200, 10);
}

void Renderer::RenderText(const std::string& text,const std::vector<Character>& characters,float x, float y, float scale , glm::vec3 color)
{
	using namespace GL;
	Shader::Use(textShader);
	Shader::SetUniform(textShader, "textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);
	for (auto& c : text)
	{
		Character ch = characters[c];
		float xpos = x + ch.bearing.x * scale;
		float ypos = y + (characters['H'].bearing.y - ch.bearing.y) * scale;
		float w = ch.size.x * scale;
		float h = ch.size.y * scale;
		float vertices[] = {
			xpos, 		ypos + h, 	0.0f, 1.0f,
			xpos + w, 	ypos, 		1.0f, 0.0f,
			xpos, 		ypos, 		0.0f, 0.0f,
			xpos, 		ypos + h, 	0.0, 1.0f,
			xpos + w, 	ypos + h, 	1.0f, 1.0f,
			xpos + w, 	ypos, 		1.0f, 0.0f
		};
		Texture::Bind(ch.texture);
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.advance >> 6) * scale;
	}
}

void Renderer::StuckRender()
{
	RenderText("Press SPACE to lunch the ball in the direction of the arrow", SweetChiliCharacters, 2 * screen.width / 5.f - 400, 2 * screen.height / 3.f, 1.5f );
}

void RenderActionOptions()
{
	using namespace Renderer;
	RenderText("press Enter to start", SweetChiliCharacters, 2 * screen.width / 5.f, 2 * screen.height / 3.f );
	RenderText("Press Q to quit", SweetChiliCharacters, 2 * screen.width / 5.f + 20, 2 * screen.height / 3.f + 50);
}

void Renderer::MainMenuRender()
{
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);
	RenderText("OutBreak", brokenWorldCharacters, screen.width / 2 - 220, screen.height / 3.f );
	RenderActionOptions();
}


void Renderer::GameOverRender()
{
	using namespace GL;
	Shader::Use(colorShader);
	Shader::SetUniform(colorShader,"color", glm::vec4(0.0f, 0.f, 0.f, 0.8f));
	glBindVertexArray(sceneQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	RenderText("GAME OVER", brokenWorldCharacters, screen.width / 2 - 220, screen.height / 3.f );
	RenderActionOptions();
}


void Renderer::PauseGameRender()
{
	using namespace GL;
	Shader::Use(colorShader);
	Shader::SetUniform(colorShader,"color", glm::vec4(0.0f, 0.f, 0.f, 0.8f));
	glBindVertexArray(sceneQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	RenderText("Game Paused", brokenWorldCharacters, screen.width / 2 - 250, screen.height / 3.f );
	RenderActionOptions();
}

void Renderer::WinRender()
{
	using namespace GL;
	Shader::Use(colorShader);
	Shader::SetUniform(colorShader,"color", glm::vec4(0.0f, 0.f, 0.f, 0.8f));
	glBindVertexArray(sceneQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	RenderText("Oh looks like you WIN!", brokenWorldCharacters, screen.width / 2 - 450, screen.height / 3.f );
	RenderActionOptions();
}

void Renderer::RenderArrow(float theta, float distance)
{
	arrow.position = distance * glm::vec2(cos(theta), sin(theta)) + ball.position;
	using namespace GL;
	//position the sprite
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, glm::vec3(arrow.position, 0.0F));
	model = glm::rotate(model, theta, glm::vec3(0.f, 0.f, 1.f));
	model = glm::scale(model, glm::vec3(arrow.size, 1.0F));
	Shader::Use(spriteRenderShader);
	Shader::SetUniform(spriteRenderShader, "model", model);
	Shader::SetUniform(spriteRenderShader, "color", arrow.color);
	//draw the sprite
	glActiveTexture(GL_TEXTURE0);
	Texture::Bind(arrowTexture);
	//glBindVertexArray(spriteQuadVAO);
	glBindVertexArray(spriteQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
