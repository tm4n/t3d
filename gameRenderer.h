#ifndef __GAMERENDERER_H__
#define __GAMERENDERER_H__

#include "SDL2/SDL.h"

#ifdef ANDROID
#include "SDL_opengles2.h"
#include <GLES2/gl2.h>
#else
#include "gl_core_2_1.h"
#include "SDL2/SDL_opengl.h"
#endif

#include "GUI.h"
#include "particlemgr.h"
#include "glm/glm.hpp"
#include "MeshSkybox.h"
#include <vector>

class gameRenderer
{
public:
	GUI *gui;
	particlemgr *partmgr;

	std::vector<Mesh*> meshlist;
	MeshSkybox *meshSkybox;

	gameRenderer(int ss_x, int ss_y, float ratio, bool fullscreen, bool antialias);
	~gameRenderer();

	void drawFrame(double time_delta);

	glm::vec3 CameraPos;
	glm::vec3 CameraAngle;
	
	float CameraJoyInputY;
	float CameraJoyInputX;
private:
	SDL_Window *window;
	SDL_GLContext glcontext;

	glm::mat4 mVPMatrix;
	glm::mat4 mProjMatrix;
	glm::mat4 mVMatrix;
};


#endif
