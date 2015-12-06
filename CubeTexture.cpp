#include "CubeTexture.h"
#include <iostream>
#include <string>
#include "SDL2/SDL.h"


CubeTexture::CubeTexture(std::vector<std::string> file) : Texture()
{
	if (file.size() != 6) return;
	files = file;
	tga_file *tga = NULL;

	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID);

	tgaFiles = new tga_file*[6];
	for (int i = 0; i < 6; i++)
	{
		std::cout << "reading " << file[i].c_str() << std::endl;
		tgaFiles[i] = readTGA(file[i].c_str());
		if (tgaFiles[i] == NULL) { std::cout << "loading a tga file for a cube map" << std::endl;  return; }
	
		// now upload texture to OpenGL
		int colorMode = tgaFiles[i]->bitCount / 8;
		if (colorMode == 3) // BGR
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, tgaFiles[i]->imageWidth, tgaFiles[i]->imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, tgaFiles[i]->imageData);
		if (colorMode == 4) // BGRA
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, tgaFiles[i]->imageWidth, tgaFiles[i]->imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, tgaFiles[i]->imageData);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	int err = glGetError();
	if (err != 0) {
		std::cout << "OGL error code " << err << " after creating cube texture" << std::endl;
		return;
	}

	loaded = true;
}


CubeTexture::~CubeTexture()
{
	if (tgaFile)
	{
		delete[] (tgaFile->imageData);
		delete tgaFile;
	}
	glDeleteTextures(1, &mTextureID);
}
