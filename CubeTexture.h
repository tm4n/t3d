#ifndef __CUBETEXTURE_H__
#define __CUBETEXTURE_H__

#include <Texture.h>
#include <vector>

#ifdef ANDROID
#include "SDL_opengles2.h"
#else
#include "gl_core_2_1.h"
#endif



class CubeTexture : public Texture {

public:

	std::vector<std::string> files;

	int size_x, size_y;

	GLuint *mTextureIDs;

	CubeTexture(std::vector<std::string> file);
	~CubeTexture();
	
protected:
	tga_file **tgaFiles;

private:
	
};


#endif
