#include "ResourceLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "GL.h"
#include "ft2build.h"
#include FT_FREETYPE_H

using namespace BreakOut;


Level ResourceLoader::LoadLevel(int levelId)
{
	Level level = {};
	std::string levelPath = "assets/levels/" + std::to_string(levelId) + ".txt";
	std::ifstream file;
	file.open(levelPath.c_str());
	if (!file.bad())
	{
		std::string line;
		uint32_t linesCount = 0;
		uint32_t columnCount = 0;
		while(std::getline(file, line)){
			columnCount = line.length();
			for (int32_t i = 0; i < line.length(); ++i){
				int type = line[i] - '0';
				Sprite sprite = {};
				sprite.position = glm::vec2(i, linesCount);
				sprite.color = glm::vec4(1.0f);
				switch (type)
				{
					case 0 :
						{
							//empty
							continue;
						}
					case 1:
						{
							//solid
							sprite.color = glm::vec4(84.0f, 13.0f, 110.0f, 255.0f) / 255.f;
							level.solidBlocks.push_back(sprite);
						} break;
					case 2:
						{
							sprite.color = glm::vec4(247.0f, 37.0f, 133.0f,255.0f) / 255.f;
							level.destructableBlocks.push_back(sprite);
						} break;
					case 3:
						{
							sprite.color = glm::vec4(114.0f, 9.0f, 183.0f, 255.0f) / 255.f;
							level.destructableBlocks.push_back(sprite);
						} break;
					case 4:
						{
							sprite.color = glm::vec4(58.0f, 12.0f, 163.0f, 255.0f) / 255.f;
							level.destructableBlocks.push_back(sprite);
						} break;
					case 5:
						{
							sprite.color = glm::vec4(67.0f, 97.0f, 238.0f, 255.0f) / 255.f;
							level.destructableBlocks.push_back(sprite);
						} break;
					default: break;
				}
			}
			linesCount++;
		}
		float unitWidth = static_cast<float>(screen.width) / static_cast<float>(columnCount);
		float unitHeight = (static_cast<float>(screen.height) / 2.0f) / static_cast<float>(linesCount);
		for(auto& sprite : level.destructableBlocks)
		{
			sprite.position = glm::vec2(sprite.position.x * unitWidth, sprite.position.y * unitHeight);
			sprite.size = glm::vec2(unitWidth, unitHeight);
		}
		for(auto& sprite : level.solidBlocks)
		{
			sprite.position = glm::vec2(sprite.position.x * unitWidth, sprite.position.y * unitHeight);
			sprite.size = glm::vec2(unitWidth, unitHeight);
		}
		level.remainingBlocks = level.destructableBlocks.size();
	}
	return level;
}

uint32_t ResourceLoader::LoadTexture(const std::string& name)
{
	uint32_t texture = 0;
	std::string filename = "assets/textures/" + name;
	int nbChannels;
	int texWidth, texHeight;
	void* data = stbi_load(filename.c_str(), &texWidth, &texHeight, &nbChannels, 0);
	if (data == nullptr)
	{
		std::cout << "ERROR::TEXTURE::COULD'T OPEN FILE::" << name <<std::endl;
		stbi_image_free(data);
		return texture;
	}
	GLenum format;
	if (nbChannels == 1)
	{
		format = GL_RED;
	}
	else if (nbChannels == 3)
	{
		format = GL_RGB;
	}
	else if (nbChannels == 4)
	{
		format = GL_RGBA;
	}
	texture = GL::Texture::GenerateTexture(texWidth, texHeight,data, format, format);
	GL::Texture::SetupTexture();
	stbi_image_free(data);
	GL::Texture::Unbind();
	return texture;
}

uint32_t ResourceLoader::LoadShader(const std::string& vertex,const std::string& geometry, const std::string& fragment)
{
	uint32_t shader;
	std::string vertexPath = "assets/shaders/vertex/" + vertex;
	std::string fragmentPath = "assets/shaders/fragment/" + fragment;
	std::string geometryPath = "assets/shaders/geometry/" + geometry;
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	// open files
	vShaderFile.open(vertexPath.c_str());
	fShaderFile.open(fragmentPath.c_str());
	if (!vShaderFile.bad() && !fShaderFile.bad())
	{
		std::stringstream vShaderStream, fShaderStream;
		// read file’s buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		if (geometry.length() != 0)
		{
			gShaderFile.open(geometryPath.c_str());
			if (!gShaderFile.bad())
			{
				std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}
			else
			{
				std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
			}
		}
	}
	else
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	// 2. compile shaders
	unsigned int vertexID, fragmentID, geometryID = 0;
	int success;
	char infoLog[512];
	// vertex Shader
	vertexID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexID, 1, &vShaderCode, NULL);
	glCompileShader(vertexID);
	// print compile errors if any
	glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
			infoLog << std::endl;
	};
	// fragment Shader
	fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentID, 1, &fShaderCode, NULL);
	glCompileShader(fragmentID);
	// print compile errors if any
	glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
			infoLog << std::endl;
	};
	if (geometry.length() != 0)
	{
		// geometry fragmenet
		const char* gShaderCode = geometryCode.c_str();
		geometryID = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryID, 1, &gShaderCode, NULL);
		glCompileShader(geometryID);
		// print compile errors if any
		glGetShaderiv(geometryID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(geometryID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
				infoLog << std::endl;
		};
	}
	shader = glCreateProgram();
	glAttachShader(shader, vertexID);
	glAttachShader(shader, fragmentID);
	if (geometryID) glAttachShader(shader, geometryID);
	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
			infoLog << std::endl;
	}
	// delete shaders; they’re linked into our program and no longer necessary
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
	return shader;
}


std::vector<Character> ResourceLoader::LoadCharacters(std::string fonts, uint32_t fontSize)
{
	using namespace GL;
	fonts = "assets/fonts/" + fonts;
	std::vector<Character> characters(128);
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout<<"ERROR::FREETYPE:: Couldn't init the library\n";
	}
	FT_Face face;
	if (FT_New_Face(ft, fonts.c_str(), 0, &face))
	{
		std::cout<<"ERROR::FREETYPE:: Couldn't init face\n";
	}
	FT_Set_Pixel_Sizes(face, 0, fontSize);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (GLubyte c = 0; c < 128; ++c)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout<<"ERROR::FREETYPE:: Couldn't init face\n";
			continue;
		}
		uint32_t texture;
		texture = Texture::GenerateTexture(face->glyph->bitmap.width,
											face->glyph->bitmap.rows,
											face->glyph->bitmap.buffer,
											GL_RED,
											GL_RED);
		Texture::SetupTexture(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		characters[c] = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<uint32_t>(face->glyph->advance.x)
		};
	}
	Texture::Unbind();
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	return characters;
}
