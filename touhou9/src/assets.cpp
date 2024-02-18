#include "assets.h"

#include "game.h"

#include "platform.h"
#include <stb_image/stb_image.h>
#include <glad/gl.h>

Texture Textures[TEXTURE_COUNT] = {};
Shader  Shaders[SHADER_COUNT]   = {};
Sound   Sounds[SOUND_COUNT]     = {};


static_assert(TEXTURE_COUNT == 7, "");
static const char* texture_filepaths[TEXTURE_COUNT] = {
	/* tex_white       */        "textures/white.png",
	/* tex_characters  */        "textures/characters.png",
	/* tex_fonts       */        "textures/fonts.png",
	/* tex_background  */        "textures/background.png",
	/* tex_projectiles */        "textures/projectiles.png",
	/* tex_boss_cirno  */        "textures/boss_cirno.png",
	/* tex_misty_lake_texture */ "textures/misty_lake_texture.png",
};


static_assert(SPRITE_COUNT == 21, "");
Sprite Sprites[SPRITE_COUNT] = {
	/* spr_white            */ {tex_white,         0,   0, 16, 16,  0,  0,  1,  1},
	/* spr_char_reimu_idle  */ {tex_characters,    0,   0, 32, 48, 16, 24,  8,  8, 0.20f,  0, 32, 48},
	/* spr_char_reimu_left  */ {tex_characters,    0,  48, 32, 48, 16, 24,  8,  8, 0.20f,  4, 32, 48},
	/* spr_char_reimu_right */ {tex_characters,    0,  96, 32, 48, 16, 24,  8,  8, 0.20f,  4, 32, 48},
	/* spr_font_main        */ {tex_fonts,        16,  48, 15, 15,  0,  0, 96, 16, 0.00f,  0, 16, 16},
	/* spr_player_hitbox    */ {tex_characters,  192, 144, 64, 64, 32, 32,  1,  1},
	/* spr_enemy_label      */ {tex_fonts,        16, 256, 48, 16, 24,  0,  1,  1},
	/* spr_reimu_shot_card  */ {tex_characters,    0, 144, 16, 16,  8,  8,  1,  1},
	/* spr_reimu_shot_orb   */ {tex_characters,   16, 144, 16, 16,  8,  8,  1,  1},

	/* spr_pickup */           {tex_projectiles,   0, 608, 16, 16,  8,  8, 16, 16, 0.00f,  0, 16, 16},

	/* spr_lazer          */   {tex_projectiles,   0,   0, 16, 16,  8,  0, 16, 16, 0.00f,  0, 16, 16},
	/* spr_bullet_arrow   */   {tex_projectiles,   0,  16, 16, 16,  8,  8, 16, 16, 0.00f,  0, 16, 16},
	/* spr_bullet_outline */   {tex_projectiles,   0,  32, 16, 16,  8,  8, 16, 16, 0.00f,  0, 16, 16},
	/* spr_bullet_filled  */   {tex_projectiles,   0,  48, 16, 16,  8,  8, 16, 16, 0.00f,  0, 16, 16},
	/* spr_bullet_rice    */   {tex_projectiles,   0,  64, 16, 16,  8,  8, 16, 16, 0.00f,  0, 16, 16},
	/* spr_bullet_kunai   */   {tex_projectiles,   0,  80, 16, 16,  8,  8, 16, 16, 0.00f,  0, 16, 16},
	/* spr_bullet_shard   */   {tex_projectiles,   0,  96, 16, 16,  8,  8, 16, 16, 0.00f,  0, 16, 16},
	/* spr_bullet_card    */   {tex_projectiles,   0, 112, 16, 16,  8,  8, 16, 16, 0.00f,  0, 16, 16},
	/* spr_bullet_bullet  */   {tex_projectiles,   0, 128, 16, 16,  8,  8, 16, 16, 0.00f,  0, 16, 16},

	/* spr_bullet_small */     {tex_projectiles,   0, 240,  8,  8,  4,  4, 16,  8, 0.00f,  0,  8,  8},

	/* spr_boss_cirno_idle */  {tex_boss_cirno,  176, 160, 64, 64, 32, 32,  4,  4, 0.15f,  0, 64, 64},

};


static_assert(SHADER_COUNT == 3, "");
static const char* shader_vertex_filepaths[SHADER_COUNT] = {
	"shaders/default.vert",
	"shaders/sharp_bilinear.vert",
	"shaders/stage1_background.vert",
};
static const char* shader_fragment_filepaths[SHADER_COUNT] = {
	"shaders/default.frag",
	"shaders/sharp_bilinear.frag",
	"shaders/stage1_background.frag",
};


static_assert(SOUND_COUNT == 3, "");
static const char* sound_filepath[SOUND_COUNT] = {
	"sounds/se_cancel00.wav",
	"sounds/se_select00.wav",
	"sounds/se_ok00.wav",
};

static void load_texture(Texture* texture, const char* filepath) {
	u8* filedata = nullptr;
	u8* buffer = nullptr;

	{
		int filesize;
		filedata = platform_read_entire_file(filepath, &filesize);

		if (!filedata) {
			log_error("Couldn't load texture %s", filepath);
			texture->width = 16;
			texture->height = 16;
			goto out;
		}

		// stbi_set_flip_vertically_on_load(1);

		int width;
		int height;
		int channels;
		buffer = stbi_load_from_memory(filedata, filesize, &width, &height, &channels, 4);

		if (!buffer) {
			log_error("Couldn't decode texture %s", filepath);
			texture->width = 16;
			texture->height = 16;
			goto out;
		}

		texture->create(width, height, buffer,
						GL_RGBA8, GL_RGBA, GL_NEAREST);

		log_info("Loaded texture %s", filepath);
	}

out:
	if (buffer) stbi_image_free(buffer);
	if (filedata) free(filedata);
}

void load_all_textures() {
	for (int i = 0; i < TEXTURE_COUNT; i++) {
		load_texture(&Textures[i], texture_filepaths[i]);
	}


}

void unload_all_textures() {
	for (int i = 0; i < TEXTURE_COUNT; i++) {
		Textures[i].destroy();
	}
}

static void load_shader(Shader* shader,
						const char* vertex_filepath,
						const char* fragment_filepath) {
	
	char* vertex_source = nullptr;
	char* fragment_source = nullptr;

	{
		int vertex_length;
		vertex_source = (char*) platform_read_entire_file(vertex_filepath, &vertex_length);

		if (!vertex_source) {
			log_error("Couldn't load shader %s", vertex_filepath);
			goto out;
		}

		int fragment_length;
		fragment_source = (char*) platform_read_entire_file(fragment_filepath, &fragment_length);

		if (!fragment_source) {
			log_error("Couldn't load shader %s", fragment_filepath);
			goto out;
		}

		bool result = shader->create(vertex_source, vertex_length,
									 fragment_source, fragment_length,
									 vertex_filepath,
									 fragment_filepath);
		if (!result) {
			goto out;
		}

		log_info("Loaded shaders %s, %s", vertex_filepath, fragment_filepath);
	}

out:
	if (fragment_source) free(fragment_source);
	if (vertex_source) free(vertex_source);
}

void load_all_shaders() {
	for (int i = 0; i < SHADER_COUNT; i++) {
		load_shader(&Shaders[i],
					shader_vertex_filepaths[i],
					shader_fragment_filepaths[i]);
	}
}

void unload_all_shaders() {
	for (int i = 0; i < SHADER_COUNT; i++) {
		Shaders[i].destroy();
	}

}

static void load_sound(Sound* sound, const char* filepath) {
	u8* filedata = nullptr;

	{
		int filesize;
		filedata = platform_read_entire_file(filepath, &filesize);

		if (!filedata) {
			log_error("Couldn't open sound %s", filepath);
			goto out;
		}

		bool result = sound->create(filedata, filesize, filepath);
		if (!result) {
			goto out;
		}

		log_info("Loaded sound %s", filepath);
	}

out:
	if (filedata) free(filedata);
}

void load_all_sounds() {
	for (int i = 0; i < SOUND_COUNT; i++) {
		load_sound(&Sounds[i], sound_filepath[i]);
	}
}

void unload_all_sounds() {
	for (int i = 0; i < SOUND_COUNT; i++) {
		Sounds[i].destroy();
	}
}

float sprite_get_anim_spd(u32 sprite_index) {
	assert_sprite_index(sprite_index);
	Sprite* sprite = &Sprites[sprite_index];
	return sprite->anim_spd;
}
