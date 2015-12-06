#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <stdint.h>
#include <string>
#include "SDL2/SDL_ttf.h"


#ifdef ANDROID
#include "SDL_opengles2.h"
#else
#include "gl_core_2_1.h"
#endif


typedef struct
{
    uint8_t imageTypeCode;
    int16_t imageWidth;
    int16_t imageHeight;
    uint8_t bitCount;
    uint8_t *imageData;

} tga_file;


class Texture {

public:
	bool loaded;
	bool isParticleTex;

	std::string file;

	int size_x, size_y;

	GLuint mTextureID;

	Texture();
	Texture(const char* filename);
	Texture(std::string txt, TTF_Font *fnt, SDL_Color c);
	~Texture();
	
protected:
	tga_file *tgaFile;
	tga_file *readTGA(const char *filename);
	bool uploadTGA(const tga_file *tga);

private:
	
};


#endif
