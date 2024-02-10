#include "renderer.h"

#include "game.h"
#include "assets.h"

#include "cpml.h"
#include "utils.h"
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>

Renderer* renderer;

void Renderer::init() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, BATCH_MAX_VERTICIES * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	batch.reserve(BATCH_MAX_VERTICIES);

	{
		std::vector<u32> indicies;
		indicies.resize(BATCH_MAX_INDICIES);

		u32 offset = 0;
		for (size_t i = 0; i < indicies.size(); i += 6) {
			indicies[i + 0] = offset + 0;
			indicies[i + 1] = offset + 1;
			indicies[i + 2] = offset + 2;

			indicies[i + 3] = offset + 2;
			indicies[i + 4] = offset + 3;
			indicies[i + 5] = offset + 0;

			offset += 4;
		}

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(indicies[0]), indicies.data(), GL_STATIC_DRAW);
	}

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(batch[0]), (void*) 0); // position

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(batch[0]), (void*) 12); // color

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(batch[0]), (void*) 28); // texcoord

	proj  = mat4{1.0f};
	view  = mat4{1.0f};
	model = mat4{1.0f};
	MVP   = mat4{1.0f};

	current_shader = &Shaders[shd_default];

	if (current_shader->program != 0) glUseProgram(current_shader->program);
}

void Renderer::destroy() {
	glDeleteBuffers(1, &ibo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Renderer::break_batch() {
	if (batch.empty()) {
		return;
	}
	if (current_texture == 0) {
		batch.clear();
		return;
	}

#if 1
	// glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, batch.size() * sizeof(batch[0]), batch.data());

	// glEnableVertexAttribArray

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, current_texture);

	MVP = (proj * view) * model;

	if (current_shader->program != 0) {
		if (current_shader->u_texture != -1) glUniform1i(current_shader->u_texture, 0);
		if (current_shader->u_mvp     != -1) glUniformMatrix4fv(current_shader->u_mvp, 1, GL_FALSE, &MVP[0][0]);
	}

	switch (mode) {
		case MODE_QUADS: {
			// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

			assert(batch.size() % VERTICIES_PER_QUAD == 0);

			glDrawElements(GL_TRIANGLES, ((int)batch.size() / VERTICIES_PER_QUAD) * INDICIES_PER_QUAD, GL_UNSIGNED_INT, nullptr);
			break;
		}

		case MODE_TRIANGLES: {
			// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			assert(batch.size() % 3 == 0);

			glDrawArrays(GL_TRIANGLES, 0, (int)batch.size());
			break;
		}

		case MODE_LINES: {
			break;
		}

		case MODE_POINTS: {
			break;
		}
	}
#endif

	draw_calls++;

	max_batch_this_frame = max(max_batch_this_frame, (int)batch.size());

	batch.clear();
	// current_texture = 0;

}

void Renderer::begin_frame() {
	batch.clear();
	current_texture = 0;

	draw_calls = 0;
	max_batch_this_frame = 0;
	total_objects_drawn = 0;
}

static void gl_clear_errors() {
	while (glGetError() != GL_NO_ERROR) {}
}

static void gl_print_errors() {
	while (u32 error = glGetError()) {
		log_error("GL ERROR: %u", error);
	}
}

void Renderer::end_frame() {
	break_batch();

	gl_print_errors();

	assert(current_shader == &Shaders[shd_default]);
}

void Renderer::set_render_target(Texture* texture) {
	if (texture) {
		assert(texture->fbo != 0);

		break_batch();
		glBindFramebuffer(GL_FRAMEBUFFER, texture->fbo);
	} else {
		break_batch();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Renderer::set_viewport(int x, int y, int width, int height) {
	break_batch();
	glViewport(x, y, width, height);
}

void Renderer::clear(float r, float g, float b, float a) {
	break_batch();
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::set_proj(mat4 proj) {
	break_batch();
	this->proj = proj;
}

void Renderer::set_view(mat4 view) {
	break_batch();
	this->view = view;
}

void Renderer::set_model(mat4 model) {
	break_batch();
	this->model = model;
}

void Renderer::set_shader(u32 shader_index) {
	assert_shader_index(shader_index);
	Shader* shader = &Shaders[shader_index];

	if (shader != current_shader) {
		break_batch();

		current_shader = shader;

		// 
		// Fallback to default?
		// 
		if (current_shader->program == 0) {
			current_shader = &Shaders[shd_default];
		}

		if (current_shader->program != 0) glUseProgram(current_shader->program);
	}
}

void Renderer::reset_shader() {
	set_shader(shd_default);
}

void Renderer::set_shader_uniform(int location, float value) {
	if (location == -1) return;
	if (current_shader->program == 0) return;
	glUniform1f(location, value);
}
void Renderer::set_shader_uniform(int location, vec2 value) {
	if (location == -1) return;
	if (current_shader->program == 0) return;
	glUniform2f(location, value.x, value.y);
}
void Renderer::set_shader_uniform(int location, vec3 value) {
	if (location == -1) return;
	if (current_shader->program == 0) return;
	glUniform3f(location, value.x, value.y, value.z);
}
void Renderer::set_shader_uniform(int location, vec4 value) {
	if (location == -1) return;
	if (current_shader->program == 0) return;
	glUniform4f(location, value.x, value.y, value.z, value.w);
}
void Renderer::set_shader_uniform(int location, int value) {
	if (location == -1) return;
	if (current_shader->program == 0) return;
	glUniform1i(location, value);
}
void Renderer::set_shader_uniform(int location, mat4 value) {
	if (location == -1) return;
	if (current_shader->program == 0) return;
	glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void Renderer::draw_textured_quad(Texture* texture,
								  Vertex verticies[4]) {
	assert(verticies);

	if (mode != MODE_QUADS) {
		break_batch();

		mode = MODE_QUADS;
	}

	if (texture && texture->id != current_texture) {
		break_batch();

		current_texture = texture->id;
	}

	if (batch.size() + VERTICIES_PER_QUAD > BATCH_MAX_VERTICIES) {
		break_batch();
	}

	batch.push_back(verticies[0]);
	batch.push_back(verticies[1]);
	batch.push_back(verticies[2]);
	batch.push_back(verticies[3]);

	total_objects_drawn++;
}

void Renderer::draw_textured_triangle(Texture* texture,
									  Vertex verticies[3]) {
	assert(verticies);

	if (mode != MODE_TRIANGLES) {
		break_batch();

		mode = MODE_TRIANGLES;
	}

	if (texture && texture->id != current_texture) {
		break_batch();

		current_texture = texture->id;
	}

	if (batch.size() + 3 > BATCH_MAX_VERTICIES) {
		break_batch();
	}

	batch.push_back(verticies[0]);
	batch.push_back(verticies[1]);
	batch.push_back(verticies[2]);

	total_objects_drawn++;
}

void Renderer::draw_texture(Texture* texture,
							float x, float y) {
	assert(texture);

	float x1 = x;
	float y1 = y;
	float x2 = x + float(texture->width);
	float y2 = y + float(texture->height);

	float u1 = 0;
	float v1 = 0;
	float u2 = 1;
	float v2 = 1;

	Vertex verticies[4] = {
		{{x1, y1, 0}, {1, 1, 1, 1}, {u1, v1}},
		{{x2, y1, 0}, {1, 1, 1, 1}, {u2, v1}},
		{{x2, y2, 0}, {1, 1, 1, 1}, {u2, v2}},
		{{x1, y2, 0}, {1, 1, 1, 1}, {u1, v2}},
	};

	draw_textured_quad(texture, verticies);
}

void Renderer::draw_texture_stretched(Texture* texture,
									  float x, float y,
									  float width, float height) {
	assert(texture);

	float x1 = x;
	float y1 = y;
	float x2 = x + width;
	float y2 = y + height;

	float u1 = 0;
	float v1 = 0;
	float u2 = 1;
	float v2 = 1;

	Vertex verticies[4] = {
		{{x1, y1, 0}, {1, 1, 1, 1}, {u1, v1}},
		{{x2, y1, 0}, {1, 1, 1, 1}, {u2, v1}},
		{{x2, y2, 0}, {1, 1, 1, 1}, {u2, v2}},
		{{x1, y2, 0}, {1, 1, 1, 1}, {u1, v2}},
	};

	draw_textured_quad(texture, verticies);
}

static void draw_sprite_common_part(Sprite* sprite,
									int frame_index,
									Vertex verticies[4]) {
	assert(sprite);
	assert_texture_index(sprite->texture_index);
	assert(verticies);

	Texture* texture = &Textures[sprite->texture_index];

	frame_index = clamp(frame_index, 0, sprite->frame_count - 1);

	float x1 = float(-sprite->xorigin);
	float y1 = float(-sprite->yorigin);
	float x2 = float(sprite->width  - sprite->xorigin);
	float y2 = float(sprite->height - sprite->yorigin);

	assert(sprite->frames_in_row != 0);

	int u = sprite->u + (frame_index % sprite->frames_in_row) * sprite->xstride;
	int v = sprite->v + (frame_index / sprite->frames_in_row) * sprite->ystride;

	assert(texture->width  != 0);
	assert(texture->height != 0);

	float u1 = u / float(texture->width);
	float v1 = v / float(texture->height);
	float u2 = (u + sprite->width)  / float(texture->width);
	float v2 = (v + sprite->height) / float(texture->height);

	verticies[0].pos = {x1, y1, 0};
	verticies[1].pos = {x2, y1, 0};
	verticies[2].pos = {x2, y2, 0};
	verticies[3].pos = {x1, y2, 0};

	verticies[0].uv = {u1, v1};
	verticies[1].uv = {u2, v1};
	verticies[2].uv = {u2, v2};
	verticies[3].uv = {u1, v2};
}

void Renderer::draw_sprite(Sprite* sprite, int frame_index,
						   float x, float y) {
	assert(sprite);
	assert_texture_index(sprite->texture_index);

	Texture* texture = &Textures[sprite->texture_index];

	Vertex verticies[4];

	draw_sprite_common_part(sprite, frame_index, verticies);

	verticies[0].color = {1, 1, 1, 1};
	verticies[1].color = {1, 1, 1, 1};
	verticies[2].color = {1, 1, 1, 1};
	verticies[3].color = {1, 1, 1, 1};

	verticies[0].pos += vec3{x, y, 0};
	verticies[1].pos += vec3{x, y, 0};
	verticies[2].pos += vec3{x, y, 0};
	verticies[3].pos += vec3{x, y, 0};

	draw_textured_quad(texture, verticies);
}

void Renderer::draw_sprite(Sprite* sprite, int frame_index,
						   float x, float y,
						   float xscale, float yscale,
						   float angle, u32 color) {
	assert(sprite);
	assert_texture_index(sprite->texture_index);

	Texture* texture = &Textures[sprite->texture_index];

	Vertex verticies[4];

	draw_sprite_common_part(sprite, frame_index, verticies);

	float r, g, b, a;
	extract_rgba(color, &r, &g, &b, &a);

	verticies[0].color = {r, g, b, a};
	verticies[1].color = {r, g, b, a};
	verticies[2].color = {r, g, b, a};
	verticies[3].color = {r, g, b, a};

	// double mouse_x;
	// double mouse_y;
	// glfwGetCursorPos(g_window, &mouse_x, &mouse_y);

	mat4 model{1.0f};
	model = glm::translate(model, {x, y, 0});

	model = glm::rotate(model, glm::radians(-angle), {0, 0, 1});
	model = glm::scale(model, {xscale, yscale, 1});

	// branches ?

	// if (angle != 0) {
	// 	model = glm::rotate(model, glm::radians(-angle), {0, 0, 1});
	// }
	// if (xscale != 1 || yscale != 1) {
	// 	model = glm::scale(model, {xscale, yscale, 1});
	// }

	verticies[0].pos = model * vec4{verticies[0].pos, 1};
	verticies[1].pos = model * vec4{verticies[1].pos, 1};
	verticies[2].pos = model * vec4{verticies[2].pos, 1};
	verticies[3].pos = model * vec4{verticies[3].pos, 1};

	draw_textured_quad(texture, verticies);
}

vec2 Renderer::draw_text(Sprite* sprite,
						 const char* text,
						 float x, float y,
						 int halign, int valign) {
	assert(sprite);
	assert(text);

	if (valign == VALIGN_MIDDLE) {
		y -= get_text_size(sprite, text).y / 2.0f;
	} else if (valign == VALIGN_BOTTOM) {
		y -= get_text_size(sprite, text).y;
	}

	float ch_x = x;
	float ch_y = y;

	if (halign == HALIGN_CENTER) {
		ch_x -= get_text_size(sprite, text, true).x / 2.0f;
	} else if (halign == HALIGN_RIGHT) {
		ch_x -= get_text_size(sprite, text, true).x;
	}

	int ch;
	for (const char* ptr = text; ch = *ptr; ptr++) {
		if (33 <= ch && ch <= 127) {
			draw_sprite(sprite, ch - 32, ch_x, ch_y);
			total_objects_drawn--;
		}

		ch_x += float(sprite->width);

		if (ch == '\n') {
			ch_x = x;
			ch_y += float(sprite->height);

			if (halign == HALIGN_CENTER) {
				ch_x -= get_text_size(sprite, ptr + 1, true).x / 2.0f;
			} else if (halign == HALIGN_RIGHT) {
				ch_x -= get_text_size(sprite, ptr + 1, true).x;
			}
		}
	}

	total_objects_drawn++;
	return {ch_x, ch_y};
}

vec2 Renderer::get_text_size(Sprite* sprite,
							 const char* text,
							 bool only_one_line) {
	assert(sprite);
	assert(text);

	float ch_x = 0;
	float ch_y = 0;

	float width = 0;
	float height = float(sprite->height);

	int ch;
	for (const char* ptr = text; ch = *ptr; ptr++) {
		if (33 <= ch && ch <= 127) {
			width = fmaxf(width, ch_x + float(sprite->width));
			height = fmaxf(height, ch_y + float(sprite->height));
		}

		ch_x += float(sprite->width);

		if (ch == '\n') {
			if (only_one_line) {
				return {width, height};
			}

			ch_x = 0;
			ch_y += float(sprite->height);

			height = fmaxf(height, ch_y + float(sprite->height));
		}
	}

	return {width, height};
}

void Renderer::draw_rectangle(float x, float y,
							  float width, float height,
							  u32 color) {
	Texture* texture = &Textures[tex_white];

	float x1 = x;
	float y1 = y;
	float x2 = x + width;
	float y2 = y + height;

	float u1 = 0;
	float v1 = 0;
	float u2 = 1;
	float v2 = 1;

	float r, g, b, a;
	extract_rgba(color, &r, &g, &b, &a);

	Vertex verticies[4] = {
		{{x1, y1, 0}, {r, g, b, a}, {u1, v1}},
		{{x2, y1, 0}, {r, g, b, a}, {u2, v1}},
		{{x2, y2, 0}, {r, g, b, a}, {u2, v2}},
		{{x1, y2, 0}, {r, g, b, a}, {u1, v2}},
	};

	draw_textured_quad(texture, verticies);
}

void Renderer::draw_triangle(float x1, float y1,
							 float x2, float y2,
							 float x3, float y3,
							 u32 color) {
	Texture* texture = &Textures[tex_white];

	float r, g, b, a;
	extract_rgba(color, &r, &g, &b, &a);

	Vertex verticies[3] = {
		{{x1, y1, 0}, {r, g, b, a}, {0, 0}},
		{{x2, y2, 0}, {r, g, b, a}, {0, 0}},
		{{x3, y3, 0}, {r, g, b, a}, {0, 0}},
	};

	draw_textured_triangle(texture, verticies);
}

void Renderer::draw_circle(float x, float y, float radius,
						   u32 color, int segments) {
	for (int i = 0; i < segments; i++) {
		float x1 = x;
		float y1 = y;

		float x2 = x + lengthdir_x(radius, ((i + 1) / float(segments)) * 360.0f);
		float y2 = y + lengthdir_y(radius, ((i + 1) / float(segments)) * 360.0f);

		float x3 = x + lengthdir_x(radius, (i / float(segments)) * 360.0f);
		float y3 = y + lengthdir_y(radius, (i / float(segments)) * 360.0f);

		draw_triangle(x1, y1, x2, y2, x3, y3, color);
		total_objects_drawn--;
	}
	total_objects_drawn++;
}

void Renderer::draw_texture(u32 texture_index,
							float x, float y) {
	assert_texture_index(texture_index);
	Texture* texture = &Textures[texture_index];
	draw_texture(texture, x, y);
}

void Renderer::draw_texture_stretched(u32 texture_index,
									  float x, float y,
									  float width, float height) {
	assert_texture_index(texture_index);
	Texture* texture = &Textures[texture_index];
	draw_texture_stretched(texture, x, y, width, height);
}

void Renderer::draw_sprite(u32 sprite_index, int frame_index,
						   float x, float y) {
	assert_sprite_index(sprite_index);
	Sprite* sprite = &Sprites[sprite_index];
	draw_sprite(sprite, frame_index, x, y);
}

void Renderer::draw_sprite(u32 sprite_index, int frame_index,
						   float x, float y,
						   float xscale, float yscale,
						   float angle, u32 color) {
	assert_sprite_index(sprite_index);
	Sprite* sprite = &Sprites[sprite_index];
	draw_sprite(sprite, frame_index, x, y, xscale, yscale, angle, color);
}

vec2 Renderer::draw_text(u32 sprite_index,
						 const char* text,
						 float x, float y,
						 int halign, int valign) {
	assert_sprite_index(sprite_index);
	Sprite* sprite = &Sprites[sprite_index];
	vec2 result = draw_text(sprite, text, x, y, halign, valign);
	return result;
}

vec2 Renderer::get_text_size(u32 sprite_index,
							 const char* text,
							 bool only_one_line) {
	assert_sprite_index(sprite_index);
	Sprite* sprite = &Sprites[sprite_index];
	vec2 result = get_text_size(sprite, text, only_one_line);
	return result;
}

