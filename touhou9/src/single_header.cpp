#include "common.h"


#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION


#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image/stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION


#define STB_SPRINTF_IMPLEMENTATION
#include <stb_sprintf/stb_sprintf.h>
#undef STB_SPRINTF_IMPLEMENTATION


#define MINICORO_IMPL
// #define MCO_MIN_STACK_SIZE     (32768/2)
// #define MCO_DEFAULT_STACK_SIZE (32768/2)
#define MCO_LOG log_error
#include <minicoro/minicoro.h>
#undef MINICORO_IMPL


