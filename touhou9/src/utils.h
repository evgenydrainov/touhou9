#pragma once

#include "common.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

GLFWmonitor* get_current_monitor(GLFWwindow *window);

inline u32 make_color_rgba(float r, float g, float b, float a) {
	u32 result;

	result =  (u32(a * 255.0f) & 0xff);
	result |= (u32(b * 255.0f) & 0xff) << 8;
	result |= (u32(g * 255.0f) & 0xff) << 16;
	result |= (u32(r * 255.0f) & 0xff) << 24;

	return result;
}

inline void extract_rgba(u32 color, float* r, float* g, float* b, float* a) {
	*a = (color & 0xff) / 255.0f;
	*b = ((color >> 8) & 0xff) / 255.0f;
	*g = ((color >> 16) & 0xff) / 255.0f;
	*r = (color >> 24) / 255.0f;
}
