#pragma once

#include <stdint.h>

#define GAME_W 640
#define GAME_H 480
#define GAME_FPS 60

#define ArrayLength(a) (sizeof(a)/sizeof(*(a)))

#ifdef NDEBUG
#define GLCheck(code) code
#else
#define GLCheck(code)							\
	do {										\
		code;									\
		assert(glGetError() == GL_NO_ERROR);	\
	} while (0)
#endif

#define log_info(fmt, ...)  printf(fmt "\n", ##__VA_ARGS__)
#define log_error(fmt, ...) printf(KRED fmt "\n" KNRM, ##__VA_ARGS__)

#define TIME(code)								\
	do {										\
		double ttttt = glfwGetTime();			\
		code;									\
		log_info("%.2fms\t" #code, (glfwGetTime() - ttttt) * 1000.0); \
	} while (0)

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
