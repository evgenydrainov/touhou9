#pragma once

#include "common.h"

#include "xoshiro128plus.h"

#include <minicoro/minicoro.h>

#define PLAY_AREA_W 384
#define PLAY_AREA_H 448
#define PLAY_AREA_X 32
#define PLAY_AREA_Y 16

#define MAX_BULLETS 10'000

enum {
	OBJECT_TYPE_PLAYER,
	OBJECT_TYPE_BOSS,
	OBJECT_TYPE_BULLET,
	OBJECT_TYPE_PLAYER_BULLET,

	OBJECT_TYPE_COUNT
};

enum {
	FLAG_INSTANCE_DEAD = 1,
};

struct Object {
	u32 instance_id; // contains type and id
	u32 flags;

	float x;
	float y;
	float radius;

	u32 sprite_index;
	float frame_index;
};

enum {
	PLAYER_STATE_NORMAL,
};

struct Player : Object {
	float hsp;
	float vsp;

	int state;

	bool focused;
	float hitbox_alpha;
};

struct Boss : Object {
	mco_coro* co;
};

struct Bullet : Object {
	float spd;
	float dir;
	float acc;
};

struct PlayerBullet : Object {
	
};

struct World {
	enum {
		STATE_PLAYING,
		STATE_PAUSED,
		STATE_GAMEOVER,
	};

	Player player;
	Boss boss;
	Bullet* bullets;
	int bullet_count;
	PlayerBullet* p_bullets;
	int p_bullet_count;

	int state = STATE_PLAYING;
	xoshiro128plus random = {0x68756F54, 0x7250756F, 0x63656A6F, 0x35393474};
	// xoshiro128plus random = {0x48554F54, 0x5250554F, 0x43454A4F, 0x35393454};
	mco_coro* co;

	u32 next_instance_id;

	void init();
	void destroy();

	void update(float delta);
	void update_state_playing(float delta);
	void update_state_paused(float delta);
	void update_state_gameover(float delta);
	void draw(float delta);

	Bullet* bullet_create();

	void bullet_destroy_idx(int index);

	u32 create_instance_id(u32 object_type) {
		static_assert(OBJECT_TYPE_COUNT < 16, "");
		return (object_type << 28) | (next_instance_id++);
	}
};

extern World* world;
