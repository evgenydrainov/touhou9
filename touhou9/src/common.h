#pragma once

#include <stdint.h>

#define GAME_W 640
#define GAME_H 480
#define GAME_FPS 60

#define ArrayLength(a) (sizeof(a)/sizeof(*(a)))

#define GLCheck(code) code; assert(glGetError() == GL_NO_ERROR)

#define log_info(fmt, ...)  printf(fmt "\n", ##__VA_ARGS__)
#define log_error(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
