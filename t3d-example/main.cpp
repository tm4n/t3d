#include "SDL2/SDL.h"
#include "SDL2/SDL_timer.h"
#include "SDL2/SDL_ttf.h"
#undef main
#include "gameRenderer.h"
#include "GUI.h"
#include "Timer.h"
#include <thread>

const int FRAMES_PER_SECOND = 60;

gameRenderer *renderer = NULL;
bool quit = false;


//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#ifdef ANDROID
#include <android/log.h>

int SDL_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	// open renderer with correct settings
	// SDL initialization is done by renderer
	int screenres_x = 1204;
	int screenres_y = 768;
	float screnaspect = ((float)screenres_x / (float)screenres_y);
	bool fullscreen = false;
	bool antialias = false;
	renderer = new gameRenderer(screenres_x, screenres_y, screnaspect, fullscreen, antialias);

	// Timer used to calculate time_delta (frame time)
    Timer frametime;
	double time_delta = 0.;
	Timer fps;
	
	// Initialize Joystick, needed for android console gamepad input
	SDL_Joystick *joystick = NULL;
    SDL_JoystickEventState(SDL_ENABLE);


	// Example of how to create game objects
	// loading files and managing the meshlist should be done by a resource loader component
	Mesh* mc = new Mesh("assets/mc.mdl", NULL);
	if (mc->loaded == false) { std::cout << "ERROR loading model for Metalcrate" << std::endl; exit(-1); }
	mc->initShader();
	renderer->meshlist.push_back(mc);

	// after mesh ist loaded, attach a renderobject to get it rendered
	// you can and should have only one instance of mesh, but several instance of renderObject
	// if you want to display an object several times in the world
	RenderObject * ro = new RenderObject();
	ro->translation[0] = 200.f;
	mc->addRenderObject(ro);

	// Create skybox
	std::vector<std::string> skytx_files{ "assets/sky_2.tga", "assets/sky_0.tga", "assets/sky_top.tga",
		"assets/sky_bot.tga", "assets/sky_1.tga", "assets/sky_3.tga" };
	CubeTexture *skytx = new CubeTexture(skytx_files);
	renderer->meshSkybox = new MeshSkybox(skytx);
	renderer->meshSkybox->initShader();

	std::cout << "Enter main loop" << std::endl;

	while (!quit)
	{
		frametime.start();
		//Start the frame limit timer
        fps.start();

		// turn instance of the box
		ro->rotation[0] += (float)time_delta * 2.f;


		// always try tp keep a joystick open
		if (joystick == NULL)
		{
			for (int i = 0; i < SDL_NumJoysticks(); i++)
			{
				SDL_Joystick *tmp_joystick = SDL_JoystickOpen(i);
				if (tmp_joystick && SDL_JoystickNumButtons(tmp_joystick) > 8) {joystick = tmp_joystick; i = 10000;}
					else SDL_JoystickClose(tmp_joystick);
			}
		}
		
		// Query SDL events
		SDL_Event evt;
		while( SDL_PollEvent(&evt) )
		{
			switch (evt.type)
			{
			case SDL_QUIT:  quit = true;   break;

				// process other events you want to handle here

			case SDL_KEYDOWN:
			case SDL_KEYUP:
				if ((evt.key.keysym.sym == SDLK_ESCAPE) || evt.key.keysym.sym == SDLK_F10)
				{
					quit = true;
				}
				break;
			}
		}

		// draw frame by renderer
		renderer->drawFrame(time_delta);

		// cap the frame rate
        if (fps.get_ticks() < 1000 / FRAMES_PER_SECOND)
        {
            //Sleep the remaining frame time
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
        }

		time_delta = ((double)frametime.get_ticks()) / (1000./16.);
	}

	delete renderer;

	return 0;
}
