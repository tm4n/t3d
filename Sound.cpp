#include "Sound.h"

#include <iostream>
#include <string>
#include <algorithm>

/*#include "npc_cl.h"
#include "player_cl.h"
#include "collectible_cl.h"
#include "box_cl.h"*/


bool Sound::channels3D_free[SND_CHANNELS_ALLOC] = { true, true, true, true, true, true, true, true,
													true, true, true, true, true, true, true, true };

Sound::Sound(const char* filename)
{
	chunk = NULL;
	loaded = false;

	file.assign(filename);

	SDL_RWops *file = SDL_RWFromFile(filename, "rb");
	if (file == NULL) { std::cout << "ERROR on SDL_RWFromFile while opening sound file: " << filename << std::endl;  return; }

	chunk = Mix_LoadWAV_RW(file, 1);
	if (chunk == NULL) { std::cout << "ERROR in Mix_LoadWAV_RW wh while opening sound file: " << filename << std::endl;  return; }

	loaded = true;
}

/*void Sound::play(int num, float volume, float balance)
{
	b_settings *set = b_settings::instance();
	int ch = Mix_GroupAvailable(SND_GROUP_2D);
	if (ch == -1) { std::cout << "ERROR in Mix_PlayChannel in play: No free channels." << std::endl;  return; }

	Mix_SetPosition(ch, 0, 0);
	Mix_Volume(ch, (int)(volume*(set->sound_volume / 100.f)));

	float left = 255.f*(1.f - (balance / 100.f));
	float right = 255.f*(1.f + (balance / 100.f));
	int ileft = std::min((int)left, 255);
	int iright = std::min((int)right, 255);

	Mix_SetPanning(ch, ileft, iright);
	if (Mix_PlayChannel(ch, chunk, num-1) == -1)
	{
		std::cout << "ERROR in Mix_PlayChannel in play: " << Mix_GetError() << std::endl;  return;
	}
}

// relay for this games logic
void Sound::play3D(int num, actor *ac, float volume)
{
	RenderObject *ro = NULL;

	if (ac->type == ACTOR_TYPE_BOX) ro = ((box_cl*)ac)->ro;
	if (ac->type == ACTOR_TYPE_COLLECTIBLE) ro = ((collectible_cl*)ac)->ro;
	if (ac->type == ACTOR_TYPE_NPC) ro = ((npc_cl*)ac)->ro;
	if (ac->type == ACTOR_TYPE_PLAYER) ro = ((player_cl*)ac)->ro;

	if (ro != NULL) play3D(num, ro, volume);
}

void Sound::play3D(int num, RenderObject *ro, float volume)
{
	b_settings *set = b_settings::instance();
	int ch = -1;
	// find free channel
	for (int i = 0; i < SND_CHANNELS_ALLOC; i++)
	{
		if (channels3D_free[i]) { ch = i; break; }
	}
	if (ch == -1) { std::cout << "ERROR in Mix_PlayChannel in play3D: No free channels." << std::endl;  return; }

	Mix_Volume(ch, (int)(volume*(set->sound_volume / 100.f)));

	// direction effect
	// is set later in gameRenderer::frame

	if (Mix_PlayChannel(ch, chunk, num - 1) == -1)
	{
		std::cout << "ERROR in Mix_PlayChannel in play3D: " << Mix_GetError() << std::endl;  return;
	}
	channels3D_free[ch] = false;
	// safe for being attached
	ro->attachedSndChannels.push_back(ch);
}*/
