#ifndef __GUIOBJECT_H__
#define __GUIOBJECT_H__

#include "Texture.h"

#include <vector>

class GUICallback {

	public:
		//enum Type {mousedown, mouseup}; delayed

		virtual void callback(int obj_id) = 0;
		virtual ~GUICallback();

};


class GUIObject
{
public:
	enum Types {button, panel, text, input, selectionbox, checkbox};
	enum Alignment {center, upleft, upcenter, upright, downleft, downcenter, downright, scaled}; // scaled starts from upleft

	Types type;
	Alignment alignment;

	std::vector<Texture*> textures;
	
	std::string txt; // for storing string text
	TTF_Font *fnt;

	GUICallback *callback;

	int layer;
	float x,y;
	int size_x, size_y;
	float scale_x, scale_y;
	float alpha;

	int current_tex;

	int switch_up_id, switch_down_id, switch_left_id, switch_right_id;

	int group_id;

	bool visible;
	bool centered;

	GUIObject(GUIObject::Types, Texture *tex, int layer, Alignment align, float x, float y, GUICallback *, int group_id);
	GUIObject(GUIObject::Types, std::vector<Texture*>, int layer, Alignment align, float x, float y, GUICallback *, int group_id);
	~GUIObject();
};


#endif
