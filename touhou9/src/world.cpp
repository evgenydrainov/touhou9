#include "world.h"

#include "game.h"
#include "assets.h"

#include "cpml.h"
#include "utils.h"
#include <stdio.h>

#define PLAYER_STARTING_X (PLAY_AREA_W / 2)
#define PLAYER_STARTING_Y (384)

World* world;

void World::init() {
	{
		bullets = (Bullet*) calloc(MAX_BULLETS, sizeof(bullets[0]));
		assert(bullets);
	}

	{
		player.instance_id = create_instance_id(OBJECT_TYPE_PLAYER);
		player.x = PLAYER_STARTING_X;
		player.y = PLAYER_STARTING_Y;
		player.sprite_index = spr_char_reimu_idle;
	}

	{
		// kill boss
		boss.flags |= FLAG_INSTANCE_DEAD;
	}

	{
		extern void stage_script(mco_coro* co);

		mco_desc desc = mco_desc_init(stage_script, 0);
		mco_create(&co, &desc);
		// mco_init(co, &desc);
	}
}

static void instance_cleanup(Player* p) {}

static void instance_cleanup(Boss* b) {
	if (b->co) {
		mco_destroy(b->co);
	}
}

static void instance_cleanup(Bullet* b) {}

void World::destroy() {
	for (int i = 0; i < bullet_count; i++) {
		instance_cleanup(&bullets[i]);
	}

	if (!(boss.flags & FLAG_INSTANCE_DEAD)) {
		instance_cleanup(&boss);
	}

	instance_cleanup(&player);

	mco_destroy(co);

	free(bullets);
}

static void instance_animate(Object* inst, float delta) {
	assert(inst);
	assert_sprite_index(inst->sprite_index);

	Sprite* sprite = &Sprites[inst->sprite_index];

	assert(sprite->frame_count >= 1);

	if (sprite->frame_count == 1) return;
	if (sprite->anim_spd == 0) return;

	inst->frame_index += sprite->anim_spd * delta;
	if (inst->frame_index >= float(sprite->frame_count)) {
		float a = inst->frame_index - float(sprite->loop_frame);
		float b = float(sprite->frame_count - sprite->loop_frame);

		inst->frame_index = float(sprite->loop_frame) + fmodf(a, b);
	}
}

void World::update_state_playing(float delta) {

	{
		player.hsp = 0;
		player.vsp = 0;
		player.focused = false;

		switch (player.state) {
			case PLAYER_STATE_NORMAL: {
				float move_x = 0;
				float move_y = 0;
				if (glfwGetKey(game->window, GLFW_KEY_UP)) {
					move_y -= 1;
				}
				if (glfwGetKey(game->window, GLFW_KEY_DOWN)) {
					move_y += 1;
				}
				if (glfwGetKey(game->window, GLFW_KEY_LEFT)) {
					move_x -= 1;
				}
				if (glfwGetKey(game->window, GLFW_KEY_RIGHT)) {
					move_x += 1;
				}

				player.focused = glfwGetKey(game->window, GLFW_KEY_LEFT_SHIFT);

				fnormalize0f(&move_x, &move_y);

				float spd = (player.focused) ? 2.0f : 4.0f;

				player.hsp = spd * move_x;
				player.vsp = spd * move_y;

				if (move_x < 0) {
					instance_animate(&player, delta);

					if (player.sprite_index != spr_char_reimu_left) {
						player.sprite_index = spr_char_reimu_left;
						player.frame_index = 0;
					}
				} else if (move_x > 0) {
					instance_animate(&player, delta);

					if (player.sprite_index != spr_char_reimu_right) {
						player.sprite_index = spr_char_reimu_right;
						player.frame_index = 0;
					}
				} else {
					if (player.sprite_index == spr_char_reimu_left || player.sprite_index == spr_char_reimu_right) {
						player.frame_index -= sprite_get_anim_spd(player.sprite_index) * delta;
						player.frame_index = fminf(player.frame_index, 3);
						if (player.frame_index < 0) {
							player.sprite_index = spr_char_reimu_idle;
							player.frame_index = 0;
						}
					} else {
						instance_animate(&player, delta);
					}
				}

				player.hitbox_alpha = approachf(player.hitbox_alpha,
												(player.focused) ? 1.0f : 0.0f,
												0.1f * delta);
				break;
			}
		}
	}

	if (!(boss.flags & FLAG_INSTANCE_DEAD)) {
		instance_animate(&boss, delta);
	}

	{
		player.x += player.hsp * delta;
		player.y += player.vsp * delta;

		for (int i = 0; i < bullet_count; i++) {
			Bullet* b = &bullets[i];

			b->x += lengthdir_x(b->spd, b->dir) * delta;
			b->y += lengthdir_y(b->spd, b->dir) * delta;

			b->spd += b->acc * delta;
		}
	}

	{
		player.x = fclampf(player.x, 0, PLAY_AREA_W - 1);
		player.y = fclampf(player.y, 0, PLAY_AREA_H - 1);

		for (int i = 0; i < bullet_count; i++) {
			Bullet* b = &bullets[i];

			float off = 50.0f;
			if (b->x < -off
				|| b->y < -off
				|| b->x >= float(PLAY_AREA_W) + off
				|| b->y >= float(PLAY_AREA_H) + off) {
				bullet_destroy_idx(i--);
			}
		}
	}

	{
		if (!(boss.flags & FLAG_INSTANCE_DEAD)) {
			if (boss.co) {
				if (mco_status(boss.co) == MCO_SUSPENDED) {
					boss.co->user_data = &boss;
					mco_resume(boss.co);
				}
			}
		}

		if (mco_status(co) == MCO_SUSPENDED) {
			mco_resume(co);
		}
	}

	if (game->is_key_pressed(GLFW_KEY_ESCAPE)) {
		state = STATE_PAUSED;
	}

	if (!glfwGetWindowAttrib(game->window, GLFW_FOCUSED) || glfwGetWindowAttrib(game->window, GLFW_ICONIFIED)) {
		state = STATE_PAUSED;
		log_info("Lost focus or iconified. Pausing.");
	}

}

void World::update_state_paused(float delta) {
	if (game->is_key_pressed(GLFW_KEY_ESCAPE)) {
		state = STATE_PLAYING;
	}
}

void World::update_state_gameover(float delta) {
	
}

void World::update(float delta) {
	switch (state) {
		case STATE_PLAYING:  update_state_playing(delta);  break;
		case STATE_PAUSED:   update_state_paused(delta);   break;
		case STATE_GAMEOVER: update_state_gameover(delta); break;
	}
}

void World::draw(float delta) {

	// int i = 10000;
	// 
	// while (i--) {
	// 	renderer->draw_sprite(spr_char_reimu_idle,
	// 						  0,
	// 						  random.range(0, PLAY_AREA_W),
	// 						  random.range(0, PLAY_AREA_H),
	// 						  2, 2,
	// 						  glfwGetTime() * 10.0);
	// }

	if (!(boss.flags & FLAG_INSTANCE_DEAD)) {
		renderer->draw_sprite(boss.sprite_index, int(boss.frame_index), boss.x, boss.y);
	}

	{
		// renderer->draw_circle(player.x, player.y, 100);

		renderer->draw_sprite(player.sprite_index, int(player.frame_index), player.x, player.y);

		if (player.hitbox_alpha > 0) {
			renderer->draw_sprite(spr_player_hitbox, 0,
								  player.x, player.y,
								  1, 1,
								  float(glfwGetTime()) * 50.0f,
								  make_color_rgba(1, 1, 1, player.hitbox_alpha));
		}
	}

	for (int i = 0; i < bullet_count; i++) {
		Bullet* b = &bullets[i];

		renderer->draw_sprite(b->sprite_index, int(b->frame_index),
							  b->x, b->y,
							  1, 1,
							  b->dir - 90.0f,
							  color_white);
	}

	if (state == STATE_PAUSED) {
		renderer->draw_text(spr_font_main, "PAUSED",
							PLAY_AREA_W / 2, PLAY_AREA_H / 2,
							HALIGN_CENTER, VALIGN_MIDDLE);
	}

}

template <typename T>
static void instance_destroy_idx(T* &storage, int &count, int index) {
	if (count == 0) {
		return;
	}

	instance_cleanup(&storage[index]);

	for (int i = index; i < count - 1; i++) {
		storage[i] = storage[i + 1];
	}

	count--;
}

template <typename T>
static T* instance_create(T* &storage, int &count, int MAX_INSTANCES, u32 object_type) {
	if (count == MAX_INSTANCES) {
		log_info("Instance limit for object %u hit.", object_type);
		instance_destroy_idx(storage, count, 0);
	}

	T* inst = &storage[count++];
	*inst = {};
	inst->instance_id = world->create_instance_id(object_type);

	return inst;
}

Bullet* World::bullet_create() {
	Bullet* b = instance_create(bullets, bullet_count, MAX_BULLETS, OBJECT_TYPE_BULLET);
	return b;
}

void World::bullet_destroy_idx(int index) {
	instance_destroy_idx(bullets, bullet_count, index);
}
