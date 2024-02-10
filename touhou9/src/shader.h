#pragma once

#include "common.h"

struct Shader {
	u32 program;
	int u_texture = -1;
	int u_mvp = -1;

	bool create(const char* vertex_source, int vertex_length,
				const char* fragment_source, int fragment_length,
				const char* vertex_name_for_debug,
				const char* fragment_name_for_debug);
	void destroy();
};

int shader_get_uniform(u32 shader_index, const char* name);
