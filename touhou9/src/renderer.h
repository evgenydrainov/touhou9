#pragma once

#include "common.h"

#include "texture.h"
#include "sprite.h"
#include "shader.h"

#include <glm/glm.hpp>
#include <vector>

#define BATCH_MAX_QUADS 10000
#define VERTICIES_PER_QUAD 4
#define INDICIES_PER_QUAD 6
#define BATCH_MAX_VERTICIES (BATCH_MAX_QUADS * VERTICIES_PER_QUAD)
#define BATCH_MAX_INDICIES (BATCH_MAX_QUADS * INDICIES_PER_QUAD)

#define color_white 0xffffffff
#define color_black 0x000000ff
#define color_red   0xff0000ff
#define color_green 0x00ff00ff
#define color_blue  0x0000ffff

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;


struct Vertex {
	vec3 pos;
	// vec3 normal;
	vec4 color;
	vec2 uv;
};

enum {
	HALIGN_LEFT,
	HALIGN_CENTER,
	HALIGN_RIGHT,
};

enum {
	VALIGN_TOP,
	VALIGN_MIDDLE,
	VALIGN_BOTTOM,
};

struct Renderer {
	enum {
		MODE_QUADS,
		MODE_TRIANGLES,
		MODE_LINES,
		MODE_POINTS,
	};

	mat4 proj;
	mat4 view;
	mat4 model;
	mat4 MVP;

	std::vector<Vertex> batch;
	u32 current_texture;
	Shader* current_shader;
	int mode = MODE_QUADS;
	u32 vao;
	u32 vbo;
	u32 ibo;

	int draw_calls;
	int max_batch_this_frame;
	int total_objects_drawn;

	void init();
	void destroy();

	void break_batch();

	void begin_frame();
	void end_frame();

	void set_render_target(Texture* texture);
	void set_viewport(int x, int y, int width, int height);
	void clear(float r, float g, float b, float a);
	void set_proj(mat4 proj);
	void set_view(mat4 view);
	void set_model(mat4 model);
	void set_shader(u32 shader_index);
	void reset_shader();

	void set_shader_uniform(int location, float value);
	void set_shader_uniform(int location, vec2 value);
	void set_shader_uniform(int location, vec3 value);
	void set_shader_uniform(int location, vec4 value);
	void set_shader_uniform(int location, int value);
	void set_shader_uniform(int location, mat4 value);


	void draw_textured_quad(Texture* texture,
							Vertex verticies[4]);

	void draw_textured_triangle(Texture* texture,
								Vertex verticies[3]);



	void draw_texture(Texture* texture,
					  float x, float y);

	void draw_texture_stretched(Texture* texture,
								float x, float y,
								float width, float height);

	void draw_sprite(Sprite* sprite, int frame_index,
					 float x, float y);

	void draw_sprite(Sprite* sprite, int frame_index,
					 float x, float y,
					 float xscale, float yscale,
					 float angle, u32 color);

	vec2 draw_text(Sprite* sprite,
				   const char* text,
				   float x, float y,
				   int halign = HALIGN_LEFT, int valign = VALIGN_TOP);

	vec2 get_text_size(Sprite* sprite,
					   const char* text,
					   bool only_one_line = false);



	void draw_rectangle(float x, float y,
						float width, float height,
						u32 color = color_white);

	void draw_triangle(float x1, float y1,
					   float x2, float y2,
					   float x3, float y3,
					   u32 color = color_white);

	void draw_circle(float x, float y, float radius,
					 u32 color = color_white, int segments = 12);



	void draw_texture(u32 texture_index,
					  float x, float y);

	void draw_texture_stretched(u32 texture_index,
								float x, float y,
								float width, float height);

	void draw_sprite(u32 sprite_index, int frame_index,
					 float x, float y);

	void draw_sprite(u32 sprite_index, int frame_index,
					 float x, float y,
					 float xscale, float yscale,
					 float angle, u32 color);

	vec2 draw_text(u32 sprite_index,
				   const char* text,
				   float x, float y,
				   int halign = HALIGN_LEFT, int valign = VALIGN_TOP);

	vec2 get_text_size(u32 sprite_index,
					   const char* text,
					   bool only_one_line = false);

};

extern Renderer* renderer;

