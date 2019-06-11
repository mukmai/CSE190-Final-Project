/**
* Texture.hpp
*/

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "stb_image.h"

#include "GlfwApp.h"
#include <glm/glm.hpp>


#include <string>
#include <iostream>

enum class TextureType : unsigned char
{
	DIFFUSE,
	SPECULAR
};

GLuint LoadTextureFromFile(const char *path, const std::string &directory);

#endif