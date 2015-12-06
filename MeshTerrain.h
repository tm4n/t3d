#ifndef __MESHTERRAIN_H__
#define __MESHTERRAIN_H__

#include "Mesh.h"

class MeshTerrain: public Mesh
{
public:

	MeshTerrain(const char *mesh_file, const char *tex_file);

	virtual void setShader();
	virtual void initShader();
	virtual void draw(const glm::mat4 &mVPMatrix);

protected:
	GLuint mTexHandle2;

};



#endif