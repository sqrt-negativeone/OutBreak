#include "GL.h"

using namespace BreakOut::GL;


void Shader::Use(uint32_t shaderID)
{
	glUseProgram(shaderID);
}
void Shader::SetUniform(uint32_t shaderID, const std::string& name, int u)
{
	glUniform1i(glGetUniformLocation(shaderID, name.c_str()), u);
}

void Shader::SetUniform(uint32_t shaderID, const std::string& name, float u)
{
	glUniform1f(glGetUniformLocation(shaderID, name.c_str()), u);
}

void Shader::SetUniform(uint32_t shaderID,const std::string& name, glm::mat4& u)
{
	glUniformMatrix4fv(glGetUniformLocation(shaderID, name.c_str()),1, GL_FALSE, glm::value_ptr(u));
}

void Shader::SetUniform(uint32_t shaderID,const std::string& name, const glm::vec2& u)
{
	glUniform2f(glGetUniformLocation(shaderID, name.c_str()), u.x, u.y);
}

void Shader::SetUniform(uint32_t shaderID,const std::string& name, const glm::vec3& u)
{
	glUniform3f(glGetUniformLocation(shaderID, name.c_str()), u.x, u.y, u.z);
}

void Shader::SetUniform(uint32_t shaderID,const std::string& name, const glm::vec4& u)
{
	glUniform4f(glGetUniformLocation(shaderID, name.c_str()), u.x, u.y, u.z, u.w);
}

void Texture::Bind(uint32_t textureID)
{
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

uint32_t Texture::GenerateTexture(int32_t width, int32_t height, void* data, GLenum ImageFormat, GLenum internalFormat, GLenum type)
{
	uint32_t textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, ImageFormat, type, data);
	return textureID;
}

void Texture::SetupTexture(GLenum wrapS , GLenum wrapT , GLenum filterMin, GLenum filterMax)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMax);
}

uint32_t FrameBuffer::GenerateFrameBuffer()
{
	uint32_t fbo;
	glGenFramebuffers(1, &fbo);
	return fbo;
}

void FrameBuffer::BindFrameBuffer(uint32_t fbo)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void FrameBuffer::AttachColor(uint32_t texture, GLenum type, uint32_t attachementNumber)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachementNumber,type, texture, 0);
}

uint32_t FrameBuffer::GenerateRenderBuffer()
{
	uint32_t fbo;
	glGenRenderbuffers(1, &fbo);
	return fbo;
}

void FrameBuffer::BindRenderBuffer(uint32_t rbo)
{
	glBindRenderbuffer(GL_FRAMEBUFFER, rbo);
}

void FrameBuffer::AttachRenderbufferMultisample(uint32_t rbo)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);
}

