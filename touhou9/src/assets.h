#pragma once

#include "common.h"

#include "texture.h"
#include "sprite.h"
#include "shader.h"
#include "sound.h"

#define assert_texture_index(texture_index) assert((texture_index) < TEXTURE_COUNT)
#define assert_sprite_index(sprite_index) assert((sprite_index) < SPRITE_COUNT)
#define assert_shader_index(shader_index) assert((shader_index) < SHADER_COUNT)
#define assert_sound_index(sound_index) assert((sound_index) < SOUND_COUNT)

enum {
	tex_white,
	tex_characters,
	tex_fonts,
	tex_background,
	tex_projectiles,
	tex_boss_cirno,
	tex_misty_lake_texture,

	TEXTURE_COUNT
};

enum {
	spr_white,
	spr_char_reimu_idle,
	spr_char_reimu_left,
	spr_char_reimu_right,
	spr_font_main,
	spr_player_hitbox,
	spr_enemy_label,
	spr_reimu_shot_card,
	spr_reimu_shot_orb,

	spr_pickup,

	spr_lazer,
	spr_bullet_arrow,
	spr_bullet_outline,
	spr_bullet_filled,
	spr_bullet_rice,
	spr_bullet_kunai,
	spr_bullet_shard,
	spr_bullet_card,
	spr_bullet_bullet,

	spr_bullet_small,

	spr_boss_cirno_idle,

	SPRITE_COUNT
};

enum {
	shd_default,
	shd_sharp_bilinear,
	shd_stage1_background,

	SHADER_COUNT
};

enum {
	snd_cancel,
	snd_select,
	snd_ok,

	SOUND_COUNT
};

extern Texture Textures[TEXTURE_COUNT];
extern Sprite Sprites[SPRITE_COUNT];
extern Shader Shaders[SHADER_COUNT];
extern Sound Sounds[SOUND_COUNT];

void load_all_textures();
void unload_all_textures();

void load_all_shaders();
void unload_all_shaders();

void load_all_sounds();
void unload_all_sounds();

float sprite_get_anim_spd(u32 sprite_index);
