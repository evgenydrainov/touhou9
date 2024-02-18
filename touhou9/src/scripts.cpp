#include "game.h"
#include "assets.h"

#include "cpml.h"

#define BOSS_STARTING_X (PLAY_AREA_W / 2)
#define BOSS_STARTING_Y (128)

#define wait(frames) _wait(co, frames)

static void _wait(mco_coro* co, int frames) {
	while (frames--) {
		mco_yield(co);
	}
}

static Bullet* shoot(float x, float y,
					 float spd, float dir, float acc,
					 u32 sprite_index, int color,
					 u32 flags = 0, void (*script)(mco_coro*) = nullptr) {
	Bullet* b = world->bullet_create();

	b->x = x;
	b->y = y;

	b->spd = spd;
	b->dir = dir;
	b->acc = acc;

	b->sprite_index = sprite_index;
	b->frame_index = float(color);

	switch (b->sprite_index) {
		case spr_bullet_arrow:	 b->radius = 2.5f; break;
		case spr_bullet_outline: b->radius = 2.5f; break;
		case spr_bullet_filled:	 b->radius = 3.5f; break;
		case spr_bullet_rice:	 b->radius = 2.5f; break;
		case spr_bullet_kunai:	 b->radius = 2.5f; break;
		case spr_bullet_shard:	 b->radius = 2.5f; break;
		case spr_bullet_card:	 b->radius = 2.5f; break;
		case spr_bullet_bullet:	 b->radius = 2.5f; break;

		case spr_bullet_small:	 b->radius = 2.5f; break;
	}

	b->flags |= flags;
	if (script) {
		
	}

	return b;
}

void stage_script(mco_coro* co) {
	wait(60);

	world->boss = {};
	world->boss.instance_id = world->create_instance_id(OBJECT_TYPE_BOSS);
	world->boss.x = BOSS_STARTING_X;
	world->boss.y = BOSS_STARTING_Y;
	world->boss.sprite_index = spr_boss_cirno_idle;

	{
		extern void boss_script(mco_coro* co);

		mco_desc desc = mco_desc_init(boss_script, 0);
		mco_create(&world->boss.co, &desc);
	}

}

#define self ((Boss*)co->user_data)

//*
void boss_script(mco_coro* co) {
	int i = 0;
	int color = 0;

	while (true) {
		u32 sprite_index[] = {
			spr_bullet_arrow,
			spr_bullet_outline,
			spr_bullet_filled,
			spr_bullet_rice,
			spr_bullet_kunai,
			spr_bullet_shard,
			spr_bullet_card,
			spr_bullet_bullet,
			spr_bullet_small,
		};

		i++;
		i %= ArrayLength(sprite_index);

		color++;
		color %= 16;

		for (int j = 8; j--;) {
			shoot(self->x, self->y,
				  world->random.range(1, 2),
				  // point_direction(self->x, self->y, world->player.x, world->player.y),
				  world->random.range(0, 360),
				  0,
				  sprite_index[i], color);
		}

		wait(2);
	}
}
//*/

/*
void boss_script(mco_coro* co) {
	u32 sprite_index[] = {
		spr_bullet_arrow,
		spr_bullet_outline,
		spr_bullet_filled,
		spr_bullet_rice,
		spr_bullet_kunai,
		spr_bullet_shard,
		spr_bullet_card,
		spr_bullet_bullet,
		spr_bullet_small,
	};


	for (int j = 100'000; j--;) {
		int color = world->random.next() % 16;
		int i = world->random.next() % ArrayLength(sprite_index);

		shoot(self->x, self->y,
			  world->random.range(0.1f, 0.2f),
			  // point_direction(self->x, self->y, world->player.x, world->player.y),
			  world->random.range(0, 360),
			  0,
			  sprite_index[i], color);
	}
}
//*/

#undef self
