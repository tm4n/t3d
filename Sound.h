#ifndef __SOUND_H__
#define __SOUND_H__

#include "SDL2/SDL_mixer.h"
#include <string>
//#include "actor.h"
#include "RenderObject.h"

#define SND_CHANNELS_ALLOC 16 // will allocate twice as many, half for 3D and half for 2D
#define SND_GROUP_2D 1

class Sound {

public:
	bool loaded;

	std::string file;

	Sound(const char* filename);
	~Sound();

	void play(int num, float volume, float balance = 0.f);
	/*void play3D(int num, actor *ac, float volume);
	void play3D(int num, RenderObject *ro, float volume);*/

	static bool channels3D_free[SND_CHANNELS_ALLOC];

private:
	Mix_Chunk *chunk;

};

#endif //__SOUND_H__
