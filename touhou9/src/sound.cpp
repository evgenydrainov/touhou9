#include "sound.h"

#include "assets.h"
#include "game.h"

#include <assert.h>
#include <stdio.h>

bool Sound::create(u8* filedata, int filesize, const char* name_for_debug) {
	ma_result result;

	ma_decoder_config decoder_config = {};
	result = ma_decode_memory(filedata, filesize, &decoder_config, &framecount, &framedata);
	if (result != MA_SUCCESS) {
		log_error("Couldn't decode sound %s.", name_for_debug);
		return false;
	}

	ma_audio_buffer_config audio_buffer_config = ma_audio_buffer_config_init(decoder_config.format, decoder_config.channels, framecount, framedata, nullptr);
	audio_buffer_config.sampleRate = decoder_config.sampleRate;
	result = ma_audio_buffer_init(&audio_buffer_config, &audio_buffer);
	if (result != MA_SUCCESS) {
		log_error("Couldn't create audio buffer for sound %s.", name_for_debug);
		return false;
	}

	result = ma_sound_init_from_data_source(&game->audio_engine, &audio_buffer, 0, nullptr, &sound);
	if (result != MA_SUCCESS) {
		log_error("Couldn't load sound %s.", name_for_debug);
		return false;
	}

	return true;
}

void Sound::destroy() {
	ma_sound_uninit(&sound);
	ma_audio_buffer_uninit(&audio_buffer);
	if (framedata) ma_free(framedata, nullptr);
}


void play_sound(u32 sound_index) {
	assert_sound_index(sound_index);

	Sound* sound = &Sounds[sound_index];

	if (ma_sound_is_playing(&sound->sound)) {
		ma_sound_stop(&sound->sound);
	}
	ma_sound_seek_to_pcm_frame(&sound->sound, 0);
	ma_sound_start(&sound->sound);
}

