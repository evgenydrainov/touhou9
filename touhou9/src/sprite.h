#pragma once

#include "common.h"

struct Sprite {
	u32 texture_index;
	int u;
	int v;
	int width;
	int height;
	int xorigin;
	int yorigin;
	int frame_count;
	int frames_in_row;
	float anim_spd;
	int loop_frame;
	int xstride;
	int ystride;
};
