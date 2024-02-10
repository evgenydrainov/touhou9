#pragma once

#include "common.h"

struct Texture {
	u32 id;
	u32 fbo;
	int width;
	int height;

	void create(int width, int height, u8* buffer,
				int internal_format, u32 format, int filter);
	void create_framebuffer(int width, int height,
							int internal_format, u32 format, int filter);
	void destroy();
};
