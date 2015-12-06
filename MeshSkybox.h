#ifndef __MESHSKYBOX_H__
#define __MESHSKYBOX_H__

#include "Mesh.h"
#include "CubeTexture.h"

class MeshSkybox: public Mesh
{
public:

	MeshSkybox(CubeTexture *cubeTex);

	virtual void setShader();
	virtual void initShader();
	virtual void draw(const glm::mat4 &projection, const glm::mat4 &view);

protected:
	CubeTexture *tex;
	GLuint mVertexBuffer, mProjectionHandle, mViewHandle;
};



#endif