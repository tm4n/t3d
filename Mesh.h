#ifndef __MESH_H__
#define __MESH_H__

#include "RenderObject.h"
#include <list>
#include <vector>
#include "vec.h"
#include <string>
#include "glm/glm.hpp"

#ifdef ANDROID
#include "SDL_opengles2.h"
#else
#include "gl_core_2_1.h"
#endif

class Mesh
{
public:
	bool loaded;

	std::string filename;

	int numskins;
	int numverts;
	int numtris;
	int numframes;

	vec bb_min;
	vec bb_max;

	std::list<RenderObject*> objectList;

	Mesh();
	Mesh(const char *mesh_file, const char *tex_file);

	virtual void draw(const glm::mat4 &mVPMatrix);

	virtual void initShader();
	virtual void setShader();

	void addRenderObject(RenderObject *obj);
    
	void removeRenderObject(RenderObject *obj);

	int animate(RenderObject* ro, const char* frame, float progress, int flags);

protected:
	const char *vertexShaderCode;
	const char *fragmentShaderCode;

	std::vector<std::string> frameNames;

	glm::mat4 mTransformationMatrix;
	glm::mat4 mFinalMatrix;

	GLuint *mTextureID;
    GLuint mProgram;
    GLuint mTexCoordHandle;
    GLuint mPositionHandle;
    GLuint mNextPositionHandle;
    GLuint mAnimProgressHandle;
	GLuint mAlphaHandle;
	GLuint mColoringHandle;
	GLuint mTexHandle;

    GLuint *mVertexBuffer;
    GLuint mTexCoordBuffer;
    GLuint mMVPMatrixHandle;

	int loadShader(int type, const char *code);
};



#endif
