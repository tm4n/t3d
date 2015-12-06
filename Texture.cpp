#include "Texture.h"
#include <iostream>
#include <string>
#include "SDL2/SDL.h"

Texture::Texture()
{
	isParticleTex = false;
	loaded = false;
	tgaFile = NULL;
}

Texture::Texture(const char* filename)
{
	isParticleTex = false;
	loaded = false;
	tga_file *tga = readTGA(filename);
	if (!tga) return;
	
	bool success = uploadTGA(tga);
	if (!success) return;

	loaded = true;
}

Texture::Texture(std::string txt, TTF_Font *fnt, SDL_Color c)
{
	isParticleTex = false;
	loaded = false;
	tgaFile = NULL;
	
	// Fixes empty surface errors
	if (txt.length() == 0) txt.assign(" ");

	file.assign(txt);
	
	// render text
	SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(fnt, txt.c_str(), c, 1024);
	//SDL_Surface *surface = TTF_RenderUTF8_Solid(fnt, txt.c_str(), c);
	
	if (surface == NULL) {std::cout  << "Could not render a TTF text!" << std::endl; exit(-1);}
	
	// Convert to OpenGl texture
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID); 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	int colors = surface->format->BytesPerPixel;
	if (colors == 4) {   // alpha
		if (surface->format->Rmask != 0x000000ff)
		{
			unsigned char *d = (unsigned char*)(surface->pixels);
			for (int imageIdx = 0; imageIdx < surface->w*surface->h*colors; imageIdx += colors)
			{
				unsigned char colorSwap = d[imageIdx];
				d[imageIdx] = d[imageIdx + 2];
				d[imageIdx + 2] = colorSwap;
			}
			
			
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
						GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
						
	}
	if (colors == 3) {           // no alpha
		if (surface->format->Rmask != 0x000000ff)
		{
			unsigned char *d = (unsigned char*)(surface->pixels);
			for (int imageIdx = 0; imageIdx < surface->w*surface->h*colors; imageIdx += colors)
			{
				unsigned char colorSwap = d[imageIdx];
				d[imageIdx] = d[imageIdx + 2];
				d[imageIdx + 2] = colorSwap;
			}
			
			
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0,
						GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
	}
	if (colors == 1)
	{
		// no alpha, palettized
		SDL_Surface *intermediary = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 
			//0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000); original!
			0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000); //little endian

		SDL_BlitSurface(surface, 0, intermediary, 0);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, intermediary->w, intermediary->h, 0,
						GL_RGBA, GL_UNSIGNED_BYTE, intermediary->pixels);
						
		SDL_FreeSurface(intermediary);
	}
						
	size_x = surface->w;
	size_y = surface->h;

	SDL_FreeSurface(surface);
						
	loaded = true;
	
	int err = glGetError();
	if (err != 0) {
		std::cout << "OGL error code " << err << " after creating text texture" << std::endl;
	}
}


tga_file *Texture::readTGA(const char *filename)
{
	tga_file *tga = new tga_file;
    unsigned char ucharBad, imagedescriptor;
    short int sintBad;
    long imageSize;
    int colorMode;
    unsigned char colorSwap;

	loaded = false;

	file.assign(filename);

	SDL_RWops *file = SDL_RWFromFile(filename, "rb");
	if (file == NULL) {std::cout << "ERROR on SDL_RWFromFile while opening texture file: " << filename << std::endl;  return NULL;}

    // Read the two first bytes we don't need.
    SDL_RWread(file, &ucharBad, sizeof(uint8_t), 1);
    SDL_RWread(file, &ucharBad, sizeof(uint8_t), 1);

    // Which type of image gets stored in imageTypeCode.
    SDL_RWread(file, &tga->imageTypeCode, sizeof(uint8_t), 1);

    // For our purposes, the type code should be 2 (uncompressed RGB image)
    // or 3 (uncompressed black-and-white images).
    if (tga->imageTypeCode != 2 && tga->imageTypeCode != 3)
    {
        SDL_RWclose(file);
        return NULL;
    }

    // Read 13 bytes of data we don't need.
    SDL_RWread(file, &sintBad, sizeof(int16_t), 1);
    SDL_RWread(file, &sintBad, sizeof(int16_t), 1);
    SDL_RWread(file, &ucharBad, sizeof(uint8_t), 1);
    SDL_RWread(file, &sintBad, sizeof(int16_t), 1);
    SDL_RWread(file, &sintBad, sizeof(int16_t), 1);

    // Read the image's width and height.
    SDL_RWread(file, &tga->imageWidth, sizeof(int16_t), 1);
    SDL_RWread(file, &tga->imageHeight, sizeof(int16_t), 1);
	size_x = tga->imageWidth; size_y = tga->imageHeight;

    // Read the bit depth.
    SDL_RWread(file, &tga->bitCount, sizeof(uint8_t), 1);

    // Read one byte of data we don't need.
    SDL_RWread(file, &imagedescriptor, sizeof(uint8_t), 1);

    // Color mode -> 3 = BGR, 4 = BGRA.
    colorMode = tga->bitCount / 8;
    imageSize = tga->imageWidth * tga->imageHeight * colorMode;

    // Allocate memory for the image data.
    tga->imageData = new uint8_t[imageSize];

	if( (imagedescriptor & 0x0020) == 0 )
	{
		uint8_t *buf = new uint8_t[imageSize];
		uint32_t prog = 0;
		SDL_RWread(file, buf, sizeof(uint8_t), imageSize);
		
		// read image data from back to front
		int index = (tga->imageHeight-1) * tga->imageWidth;
        for (int y = (tga->imageHeight-1); y >= 0; y--)
		{
			for (int x = 0; x < tga->imageWidth; x++)
			{
				tga->imageData[colorMode*(index + x)] = buf[prog];
				tga->imageData[colorMode*(index + x)+1] = buf[prog+1];
				tga->imageData[colorMode*(index + x)+2] = buf[prog+2];
				if (colorMode >= 4) tga->imageData[colorMode*(index + x)+3] = buf[prog+3];
				prog += colorMode;
				//SDL_RWread(file, &(tga->imageData[colorMode*(index + x)]), colorMode, 1); too slow on android
			}
			index -= tga->imageWidth;
        }
        delete[] buf;
	}
	else
	{
		// Read the image data normally
		SDL_RWread(file, tga->imageData, sizeof(uint8_t), imageSize);
	}

    // Change from BGR to RGB so OpenGL can read the image data.
    for (int imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode)
    {
        colorSwap = tga->imageData[imageIdx];
        tga->imageData[imageIdx] = tga->imageData[imageIdx + 2];
        tga->imageData[imageIdx + 2] = colorSwap;
    }

	SDL_RWclose(file);
	
	return tga;
}

bool Texture::uploadTGA(const tga_file *tga)
{
	// now upload texture to OpenGL
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int colorMode = tga->bitCount / 8;
	if (colorMode == 3) // BGR
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tga->imageWidth, tga->imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, tga->imageData);
	if (colorMode == 4) // BGRA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tga->imageWidth, tga->imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, tga->imageData);

	int err = glGetError();
	if (err != 0) {
		std::cout << "OGL error code " << err << " after uploading tga texture" << std::endl;
		return false;
	}

	return true;
}


Texture::~Texture()
{
	if (tgaFile)
	{
		delete[] (tgaFile->imageData);
		delete tgaFile;
	}
	glDeleteTextures(1, &mTextureID);
}
