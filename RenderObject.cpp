#include "RenderObject.h"

RenderObject::RenderObject()
{
	translation[0] = 0.f;
	translation[1] = 0.f;
	translation[2] = 0.f;
	
	rotation[0] = 0.f;
	rotation[1] = 0.f;
	rotation[2] = 0.f;

	setDefaults();
}

RenderObject::RenderObject(float *atranslation, float *arotation)
{
	translation[0] = atranslation[0];
	translation[1] = atranslation[1];
	translation[2] = atranslation[2];
	
	rotation[0] = arotation[0];
	rotation[1] = arotation[1];
	rotation[2] = arotation[2];

	setDefaults();
}

RenderObject::RenderObject(vec *atranslation, vec *arotation)
{
	translation[0] = atranslation->x;
	translation[1] = atranslation->y;
	translation[2] = atranslation->z;
	
	rotation[0] = arotation->x;
	rotation[1] = arotation->y;
	rotation[2] = arotation->z;

	setDefaults();
}

void RenderObject::setDefaults()
{
	scale[0] = 1.f;
	scale[1] = 1.f;
	scale[2] = 1.f;
	
	animFrame = 0;
	animNextFrame = 0;
	animProgress = 0.0f;

	visible = true;

	alpha = 1.0f;
	coloring[0] = 0.f;
	coloring[1] = 0.f;
	coloring[2] = 0.f;
}