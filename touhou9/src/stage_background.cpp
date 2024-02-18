#include "renderer.h"

#include "game.h"
#include "assets.h"

#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>

u32 vbo;
int u_Time;

void init_stage_background() {
	float x1 = -100;
	float z1 = -100;
	float x2 = 100;
	float z2 = 100;

	float u1 = 0;
	float v1 = 0;
	float u2 = 1;
	float v2 = 1;

	Vertex verticies_quad[] = {
		{{x1, 0, z1}, {1, 1, 1, 1}, {u1, v1}},
		{{x2, 0, z1}, {1, 1, 1, 1}, {u2, v1}},
		{{x2, 0, z2}, {1, 1, 1, 1}, {u2, v2}},
		{{x1, 0, z2}, {1, 1, 1, 1}, {u1, v2}},
	};

	Vertex verticies[] = {
		verticies_quad[0],
		verticies_quad[1],
		verticies_quad[2],

		verticies_quad[2],
		verticies_quad[3],
		verticies_quad[0],
	};

	GLCheck(glGenBuffers(1, &vbo));
	GLCheck(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GLCheck(glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW));

	{
		Shader* s = &Shaders[shd_stage1_background];
		if (s->program != 0) {
			GLCheck(u_Time = glGetUniformLocation(s->program, "u_Time"));
		}
	}

	{
		Texture* t = &Textures[tex_misty_lake_texture];
		if (t->id != 0) {
			GLCheck(glBindTexture(GL_TEXTURE_2D, t->id));
			GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		}
	}

	renderer->reset_state();
}

void draw_stage_background(float delta) {
	renderer->break_batch();

	GLCheck(glEnable(GL_SCISSOR_TEST));
	GLCheck(glScissor(PLAY_AREA_X, PLAY_AREA_Y, PLAY_AREA_W, PLAY_AREA_H));

	GLCheck(glClearColor(1, 1, 1, 1));
	GLCheck(glClear(GL_COLOR_BUFFER_BIT));

	// mat4 model{1};
	mat4 view = glm::lookAt<float>(vec3{0, 10, 0}, vec3{0, 0, -10}, vec3{0, -1, 0});
	mat4 proj = glm::perspectiveFov<float>(glm::radians(60.0f), PLAY_AREA_W, PLAY_AREA_H, 0.1f, 10'000.0f);
	mat4 MVP = (proj * view); // * model;

	{
		Texture* t = &Textures[tex_misty_lake_texture];
		GLCheck(glActiveTexture(GL_TEXTURE0));
		GLCheck(glBindTexture(GL_TEXTURE_2D, t->id));
	}

	{
		Shader* s = &Shaders[shd_stage1_background];
		if (s->program != 0) {
			GLCheck(glUseProgram(s->program));
			if (s->u_texture != -1) { GLCheck(glUniform1i(s->u_texture, 0)); }
			if (s->u_mvp != -1)     { GLCheck(glUniformMatrix4fv(s->u_mvp, 1, GL_FALSE, &MVP[0][0])); }
			if (u_Time != -1)       { GLCheck(glUniform1f(u_Time, (float) glfwGetTime());); }
		}
	}

	GLCheck(glBindBuffer(GL_ARRAY_BUFFER, vbo));

	Vertex::set_vertex_attrib();

	GLCheck(glDrawArrays(GL_TRIANGLES, 0, 6));
	renderer->draw_calls++;

	GLCheck(glDisable(GL_SCISSOR_TEST));

	renderer->reset_state();
}

