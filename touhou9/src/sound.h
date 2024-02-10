#pragma once

#include "common.h"

#include <miniaudio/miniaudio.h>

struct Sound {
	ma_sound sound;
	ma_audio_buffer audio_buffer;
	void* framedata;
	u64 framecount;

	bool create(u8* filedata, int filesize, const char* name_for_debug);
	void destroy();
};

void play_sound(u32 sound_index);
