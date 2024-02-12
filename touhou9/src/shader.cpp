#include "shader.h"

#include "assets.h"

#include <glad/gl.h>
#include <assert.h>
#include <stdio.h>

#define GL_INFO_LOG_BUFFER_SIZE 1024

static const char* get_shader_type_name(u32 type) {
	switch (type) {
		case GL_VERTEX_SHADER:   return "vertex";
		case GL_FRAGMENT_SHADER: return "fragment";
		case GL_GEOMETRY_SHADER: return "geometry";
	}
	return "unknown";
}

static u32 compile_shader(const char* source,
						  int source_length,
						  u32 type,
						  const char* name_for_debug) {

	u32 shader;
	GLCheck(shader = glCreateShader(type));

	GLCheck(glShaderSource(shader, 1, &source, &source_length));
	GLCheck(glCompileShader(shader));

	int result;
	GLCheck(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE) {
		char message[GL_INFO_LOG_BUFFER_SIZE];
		GLCheck(glGetShaderInfoLog(shader, sizeof(message), nullptr, message));

		log_error("\nGL compile error in %s shader %s:\n%s", get_shader_type_name(type), name_for_debug, message);

		GLCheck(glDeleteShader(shader));
		return 0;
	}

	return shader;
}

static u32 create_shader(const char* vertex_source, int vertex_length,
						 const char* fragment_source, int fragment_length,
						 const char* vertex_name_for_debug,
						 const char* fragment_name_for_debug) {

	u32 program;
	GLCheck(program = glCreateProgram());
	u32 vs = compile_shader(vertex_source, vertex_length,
							GL_VERTEX_SHADER, vertex_name_for_debug);
	u32 fs = compile_shader(fragment_source, fragment_length,
							GL_FRAGMENT_SHADER, fragment_name_for_debug);

	if (vs == 0) {
		GLCheck(glDeleteProgram(program));
		program = 0;
		goto out;
	}
	if (fs == 0) {
		GLCheck(glDeleteProgram(program));
		program = 0;
		goto out;
	}

	GLCheck(glAttachShader(program, vs));
	GLCheck(glAttachShader(program, fs));

	GLCheck(glLinkProgram(program));

	{
		int result;
		GLCheck(glGetProgramiv(program, GL_LINK_STATUS, &result));
		if (result == GL_FALSE) {
			char message[GL_INFO_LOG_BUFFER_SIZE];
			GLCheck(glGetProgramInfoLog(program, sizeof(message), nullptr, message));

			log_error("GL LINK ERROR: %s", message);

			GLCheck(glDeleteProgram(program));
			program = 0;
			goto out;
		}
	}

	GLCheck(glValidateProgram(program));

	{
		int result;
		GLCheck(glGetProgramiv(program, GL_VALIDATE_STATUS, &result));
		if (result == GL_FALSE) {
			char message[GL_INFO_LOG_BUFFER_SIZE];
			GLCheck(glGetProgramInfoLog(program, sizeof(message), nullptr, message));

			log_error("GL VALIDATE ERROR: %s", message);

			GLCheck(glDeleteProgram(program));
			program = 0;
			goto out;
		}
	}

out:
	if (vs != 0) { GLCheck(glDeleteShader(vs)); }
	if (fs != 0) { GLCheck(glDeleteShader(fs)); }

	return program;
}

bool Shader::create(const char* vertex_source, int vertex_length,
					const char* fragment_source, int fragment_length,
					const char* vertex_name_for_debug,
					const char* fragment_name_for_debug) {

	program = create_shader(vertex_source, vertex_length,
							fragment_source, fragment_length,
							vertex_name_for_debug,
							fragment_name_for_debug);

	if (program != 0) {
		GLCheck(u_texture = glGetUniformLocation(program, "u_Texture"));
		GLCheck(u_mvp     = glGetUniformLocation(program, "u_MVP"));

		if (u_texture == -1) {
			log_error("Shader %s doesn't have uniform \"u_Texture\".", fragment_name_for_debug);
		}
		if (u_mvp == -1) {
			log_error("Shader %s doesn't have uniform \"u_MVP\".", vertex_name_for_debug);
		}
	} else {
		u_texture = -1;
		u_mvp     = -1;
	}

	return (program != 0);
}

void Shader::destroy() {
	if (program != 0) { GLCheck(glDeleteProgram(program)); }
}


int shader_get_uniform(u32 shader_index, const char* name) {
	assert_shader_index(shader_index);
	Shader* shader = &Shaders[shader_index];
	int result = -1;
	if (shader->program != 0) {
		GLCheck(result = glGetUniformLocation(shader->program, name));
		if (result == -1) {
			log_info("Shader %u doesn't have uniform \"%s\".", shader_index, name);
		}
	}
	return result;
}
