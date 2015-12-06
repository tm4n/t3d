#ifndef __RENDEROBJECT_H__
#define __RENDEROBJECT_H__

#include <glm/glm.hpp>
#include <vec.h>
#include <list>

class RenderObject
{
public:
	glm::vec3 translation;
	
	glm::vec3 rotation;
	
	glm::vec3 scale;
	
	int animFrame;
	int animNextFrame;
	float animProgress;
	float alpha;
	glm::vec3 coloring;

	std::list<int> attachedSndChannels; // is disabled when detached from Mesh

	bool visible;


	RenderObject();
	RenderObject(float *translation, float *rotation);
	RenderObject(vec *translation, vec *rotation);
private:
	void setDefaults();
};


#endif