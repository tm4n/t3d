#ifndef __MESHGUI_H__
#define __MESHGUI_H__

#include "Mesh.h"

class MeshGUI: public Mesh
{
public:

	MeshGUI(const char *mesh_file, const char *tex_file);

	virtual void draw(const glm::mat4 &mVPMatrix);

protected:
	GLuint mTransHandle;
	glm::vec3 translation;
};



#endif