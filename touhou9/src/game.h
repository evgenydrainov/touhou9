#pragma once

#include "common.h"

#include "world.h"
#include "renderer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <miniaudio/miniaudio.h>

struct Game {
	union {
		World world_instance = {};
	};

	Renderer renderer_instance;

	GLFWwindow* window;
	Texture game_texture;

	ma_engine audio_engine;		bool audio_engine_initialized;
	ma_context audio_context;	bool audio_context_initialized;
	ma_log audio_log;			bool audio_log_initialized;
	bool audio_logging_enabled = true;
	u32 audio_log_level = MA_LOG_LEVEL_WARNING;

	u32 key_pressed[(GLFW_KEY_LAST + 31) / 32];
	u32 key_repeat[(GLFW_KEY_LAST + 31) / 32];

	double fps;
	double update_took;
	double draw_took;
	bool frame_advance;
	bool skip_frame;

	int u_shd_sharp_bilinear_source_size = -1;
	int u_shd_sharp_bilinear_scale = -1;

	bool init();
	void init_audio();
	void destroy();

	void run();
	void update(float delta);
	void draw(float delta);

	bool is_key_pressed(int key, bool repeat = false);

};

extern Game* game;
