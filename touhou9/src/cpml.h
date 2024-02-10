#pragma once

// Cirno's perfect math library

#include <math.h>
#include <glm/glm.hpp>

static int min(int a, int b) {
	return (a < b) ? a : b;
}

static int max(int a, int b) {
	return (a > b) ? a : b;
}

static int clamp(int a, int _min, int _max) {
	a = min(a, _max);
	a = max(a, _min);
	return a;
}

static float fclampf(float a, float _min, float _max) {
	a = fminf(a, _max);
	a = fmaxf(a, _min);
	return a;
}

static float flerpf(float a, float b, float f) {
	return a + (b - a) * f;
}

static void fnormalize0f(float* _x, float* _y) {
	float x = *_x;
	float y = *_y;
	float l = sqrtf(x * x + y * y);
	if (l != 0) {
		*_x /= l;
		*_y /= l;
	}
}

static float point_distance(float x1, float y1, float x2, float y2) {
	float dx = x2 - x1;
	float dy = y2 - y1;
	return sqrtf(dx * dx + dy * dy);
}

static float point_direction(float x1, float y1, float x2, float y2) {
	return glm::degrees(atan2f(y1 - y2, x2 - x1));
}

static float dcosf(float deg) {
	return cosf(glm::radians(deg));
}

static float dsinf(float deg) {
	return sinf(glm::radians(deg));
}

static float lengthdir_x(float len, float dir) {
	return len * dcosf(dir);
}

static float lengthdir_y(float len, float dir) {
	return len * -dsinf(dir);
}

static float approachf(float start, float end, float shift) {
	return start + fclampf(end - start, -shift, shift);
}
