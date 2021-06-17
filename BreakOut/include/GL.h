#ifndef GL_H
#define GL_H

#include <glad/glad.h>
#include <cinttypes>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

namespace BreakOut
{
	namespace GL
	{
		namespace Shader
		{
			void Use(uint32_t shaderID);
			void SetUniform(uint32_t shaderID, const std::string& name, int u);
			void SetUniform(uint32_t shaderID, const std::string& name, float u);
			void SetUniform(uint32_t shaderID, const std::string& name, glm::mat4& u);
			void SetUniform(uint32_t shaderID, const std::string& name, const glm::vec2& u);
			void SetUniform(uint32_t shaderID, const std::string& name, const glm::vec3& u);
			void SetUniform(uint32_t shaderID, const std::string& name, const glm::vec4& u);
		}

		namespace Texture
		{
			void Bind(uint32_t textureID);
			void Unbind();
			uint32_t GenerateTexture(int32_t width, int32_t height, void* data,
									GLenum ImageFormat = GL_RGB, GLenum internalFormat = GL_RGB,
									GLenum type = GL_UNSIGNED_BYTE);
			void SetupTexture(GLenum wrapS = GL_REPEAT, GLenum wrapT = GL_REPEAT,
							GLenum filterMin = GL_LINEAR, GLenum filterMax = GL_LINEAR);
		}

		namespace FrameBuffer
		{
			uint32_t GenerateFrameBuffer();
			void BindFrameBuffer(uint32_t fbo);
			//NOTE(Mouad): Pass type = GL_TEXTURE_2D_MULTISAMPLE for multisample attachment
			void AttachColor(uint32_t texture, GLenum type = GL_TEXTURE_2D, uint32_t attachementNumber = 0);
			uint32_t GenerateRenderBuffer();
			void BindRenderBuffer(uint32_t rbo);
			void AttachRenderbufferMultisample(uint32_t rbo);
		}
	}
}

#endif //GL_H
