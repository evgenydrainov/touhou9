#include "game.h"

#include "assets.h"
#include "utils.h"

#include "platform.h"
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_sprintf/stb_sprintf.h>

Game* game;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		game->key_pressed[key / 32] |= 1 << (key % 32);

		switch (key) {
			case GLFW_KEY_F4: {
				if (mods != 0) {
					break;
				}

				GLFWmonitor* monitor = glfwGetWindowMonitor(window);
				if (monitor) {
					int window_w = GAME_W;
					int window_h = GAME_H;

					int work_x;
					int work_y;
					int work_w;
					int work_h;
					glfwGetMonitorWorkarea(monitor, &work_x, &work_y, &work_w, &work_h);

					glfwSetWindowMonitor(window, nullptr,
										 work_x + (work_w - window_w) / 2,
										 work_y + (work_h - window_h) / 2,
										 window_w, window_h,
										 GLFW_DONT_CARE);
				} else {
					const GLFWvidmode* mode;

					monitor = get_current_monitor(window);
					mode = glfwGetVideoMode(monitor);

					glfwSetWindowMonitor(window, monitor,
										 0, 0,
										 mode->width, mode->height,
										 mode->refreshRate);
				}
				break;
			}
		}
	}
}

static void error_callback(int error_code, const char* description) {
	log_error("GLFW ERROR: %s", description);
}

static void log_callback(void* user, u32 level, const char* message) {
	if (level > game->audio_log_level) {
		return;
	}

	printf("[%s] %s", ma_log_level_to_string(level), message);
}

void Game::init_audio() {
	ma_result result;

	// ma_backend backends[MA_BACKEND_COUNT];
	// size_t count;
	// ma_get_enabled_backends(backends, MA_BACKEND_COUNT, &count);
	// for (size_t i = 0; i < count; i++) {
	// 	log_info("%s", ma_get_backend_name(backends[i]));
	// }

	// 
	// Wasapi takes a while to initialize sometimes.
	// Prefer directsound? (for now).
	// 

	log_info("Miniaudio %s", ma_version_string());

	ma_context_config context_config = ma_context_config_init();

	result = ma_log_init(nullptr, &audio_log);
	if (result != MA_SUCCESS) {
		log_error("Couldn't create audio logger");
	} else {
		context_config.pLog = &audio_log;

		if (audio_logging_enabled) {
			result = ma_log_register_callback(&audio_log, ma_log_callback_init(log_callback, nullptr));
			if (result != MA_SUCCESS) {
				log_error("Couldn't set audio log callback");
			}
		}
	}

	ma_backend backends[] = {ma_backend_dsound};
	result = ma_context_init(backends, ArrayLength(backends), &context_config, &audio_context);

	if (result != MA_SUCCESS) {
		result = ma_context_init(nullptr, 0, &context_config, &audio_context);
	}

	if (result != MA_SUCCESS) {
		log_error("Couldn't initialize audio context.");
		return;
	}

	{
		ma_backend backend = audio_context.backend;
		log_info("Audio backend: %s.", ma_get_backend_name(backend));
	}

	ma_engine_config engine_config = ma_engine_config_init();
	engine_config.pContext = &audio_context;
	result = ma_engine_init(&engine_config, &audio_engine);
	if (result != MA_SUCCESS) {
		log_error("Couldn't initialize audio engine.");
		return;
	}
}

static const char* platform_get_name(int platform) {
	switch (platform) {
		case GLFW_PLATFORM_WIN32:   return "Win32";
		case GLFW_PLATFORM_COCOA:   return "Cocoa";
		case GLFW_PLATFORM_WAYLAND: return "Wayland";
		case GLFW_PLATFORM_X11:     return "X11";
		case GLFW_PLATFORM_NULL:    return "Null";
	}
	return "Unknown";
}

void Game::init() {
	log_info("Starting game...");

	// 
	// Why not
	// 
	log_info("Bits: %d.", 8 * (int)sizeof(void*));

	log_info("GLFW %s", glfwGetVersionString());

	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		log_error("Couldn't initialize GLFW.");
		exit(1);
	}

	{
		int platform = glfwGetPlatform();
		log_info("GLFW platform: %s", platform_get_name(platform));
	}

	int window_w = GAME_W;
	int window_h = GAME_H;

	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		int work_x;
		int work_y;
		int work_w;
		int work_h;
		glfwGetMonitorWorkarea(monitor, &work_x, &work_y, &work_w, &work_h);

		glfwWindowHint(GLFW_POSITION_X, work_x + (work_w - window_w) / 2);
		glfwWindowHint(GLFW_POSITION_Y, work_y + (work_h - window_h) / 2);
	}

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(window_w, window_h, "touhou9", nullptr, nullptr);

	if (!window) {
		log_error("Couldn't create window.");
		exit(1);
	}

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(0);

	{
		const char* string;
		int maj = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
		int min = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
		int rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);

		log_info("OpenGL %d.%d.%d", maj, min, rev);

		string = (const char*) glGetString(GL_VERSION);
		log_info("GL version: %s", string);

		string = (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION);
		log_info("GL shading language version: %s", string);

		string = (const char*) glGetString(GL_VENDOR);
		log_info("GL vendor: %s", string);

		string = (const char*) glGetString(GL_RENDERER);
		log_info("GL renderer: %s", string);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	init_audio();

	log_info("");

	{
		double t = glfwGetTime();
		load_all_textures();
		log_info("%.2fms.", (glfwGetTime() - t) * 1000.0);
		log_info("");
	}
	{
		double t = glfwGetTime();
		load_all_shaders();
		log_info("%.2fms.", (glfwGetTime() - t) * 1000.0);
		log_info("");
	}
	{
		double t = glfwGetTime();
		load_all_sounds();
		log_info("%.2fms.", (glfwGetTime() - t) * 1000.0);
		log_info("");
	}

	game_texture.create_framebuffer(GAME_W, GAME_H,
									GL_RGB8, GL_RGB, GL_LINEAR);

	u_shd_sharp_bilinear_source_size = shader_get_uniform(shd_sharp_bilinear, "u_SourceSize");
	u_shd_sharp_bilinear_scale       = shader_get_uniform(shd_sharp_bilinear, "u_Scale");

	renderer = &renderer_instance;
	renderer->init();

	world = &world_instance;
	world->init();
}

void Game::destroy() {
	world->destroy();

	renderer->destroy();

	game_texture.destroy();

	unload_all_sounds();
	unload_all_shaders();
	unload_all_textures();

	ma_engine_uninit(&audio_engine);
	ma_context_uninit(&audio_context);
	ma_log_uninit(&audio_log);

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Game::run() {
	double prev_time = glfwGetTime() - 1.0 / double(GAME_FPS);

	while (!glfwWindowShouldClose(window)) {

		double time = glfwGetTime();
		double frame_end_time = time + 1.0 / double(GAME_FPS);

		fps = 1.0 / (time - prev_time);
		prev_time = time;

		{
			// static int i;
			// if (i++ % 60 == 0) log_info("%ffps", fps);
		}

		{
			double t = glfwGetTime();

			update(1);

			update_took = (glfwGetTime() - t) * 1000.0;
		}

		{
			int width;
			int height;
			glfwGetFramebufferSize(window, &width, &height);

			if (width != 0 && height != 0) {
				double t = glfwGetTime();

				draw(1);

				draw_took = (glfwGetTime() - t) * 1000.0;
			} else {
				// log_info("Skipping draw for this frame.");
			}
		}

		time = glfwGetTime();

		if (time < frame_end_time) {
			double sleep = (frame_end_time - time) * 0.95;

			if (sleep > 0) {
				platform_sleep(u32(sleep * 1000.0));
			}

			while (glfwGetTime() < frame_end_time) {}
		}

		memset(key_pressed, 0, sizeof(key_pressed));

		glfwPollEvents();
	}
}

void Game::update(float delta) {
	world->update(delta);

	if (is_key_pressed(GLFW_KEY_Z)) {
		log_info("Z");
		// ma_sound_start(&Sounds[snd_cancel].sound);
		// ma_engine_play_sound(&audio_engine, "sounds/se_cancel00.wav", nullptr);
	}
}

void Game::draw(float delta) {
	int draw_calls = renderer->draw_calls;
	int total_objects_drawn = renderer->total_objects_drawn;

	renderer->begin_frame();
	{
		renderer->set_render_target(&game_texture);
		{
			renderer->set_viewport(0, 0, GAME_W, GAME_H);
			renderer->set_proj(glm::ortho<float>(0, GAME_W, 0, GAME_H, -1, 1));

			renderer->draw_texture(tex_background, 0, 0);

			renderer->set_viewport(PLAY_AREA_X, PLAY_AREA_Y, PLAY_AREA_W, PLAY_AREA_H);
			renderer->set_proj(glm::ortho<float>(0, PLAY_AREA_W, 0, PLAY_AREA_H, -1, 1));

			// renderer->draw_sprite(spr_white, 0, 0, 0, PLAY_AREA_W / 16, PLAY_AREA_H / 16, 0, 0x000000ff);
			renderer->draw_rectangle(0, 0, PLAY_AREA_W, PLAY_AREA_H, 0x000000ff);

			world->draw(delta);

			renderer->set_proj(glm::ortho<float>(0, GAME_W, 0, GAME_H, -1, 1));
			renderer->set_viewport(0, 0, GAME_W, GAME_H);

			{
				char buf[11]; // "9999.99fps";
				stb_snprintf(buf, sizeof(buf), "%.2ffps", fps);
				renderer->draw_text(spr_font_main, buf, 632, 464, HALIGN_RIGHT, VALIGN_TOP);
			}

			{
				char buf[256];
				stb_snprintf(buf, sizeof(buf),
							 "%d draw calls\n"
							 "%zu batch memory\n"
							 "%d max batch\n"
							 "%.2fms update\n"
							 "%.2fms draw\n"
							 "%d total objects drawn",
							 draw_calls,
							 BATCH_MAX_VERTICIES * sizeof(Vertex) + BATCH_MAX_INDICIES * sizeof(u32),
							 renderer->max_batch_this_frame,
							 update_took,
							 draw_took,
							 total_objects_drawn);
				renderer->draw_text(spr_font_main, buf, PLAY_AREA_X + PLAY_AREA_W, 0);

				{
					// static int i = 0;
					// if (i++ % 60 == 0) log_info("\n\n\n\n%s", buf);
				}
			}
		}

		renderer->set_render_target(nullptr);
		{
			int width;
			int height;
			glfwGetFramebufferSize(window, &width, &height);

			renderer->set_viewport(0, 0, width, height);
			renderer->set_proj(glm::ortho<float>(0, (float)width, (float)height, 0, -1, 1));

			glClear(GL_COLOR_BUFFER_BIT);

			float xscale = (float)width  / (float)GAME_W;
			float yscale = (float)height / (float)GAME_H;
			float scale = fminf(xscale, yscale);

			float iscale = fmaxf(floorf(scale), 1.0f);

			int game_texture_w = (int) roundf((float)GAME_W * scale);
			int game_texture_h = (int) roundf((float)GAME_H * scale);

			renderer->set_shader(shd_sharp_bilinear);

			renderer->set_shader_uniform(u_shd_sharp_bilinear_source_size, vec2{GAME_W, GAME_H});
			renderer->set_shader_uniform(u_shd_sharp_bilinear_scale,       vec2{iscale, iscale});

			renderer->draw_texture_stretched(&game_texture,
											 (float) ((width  - game_texture_w) / 2),
											 (float) ((height - game_texture_h) / 2),
											 (float) game_texture_w,
											 (float) game_texture_h);

			renderer->reset_shader();
		}
	}
	renderer->end_frame();


	glfwSwapBuffers(window);

}
