#pragma once

#include "common.h"

u8* platform_read_entire_file(const char* filepath, int* filesize);

void platform_sleep(u32 ms);

struct GLFWwindow;
void platform_message_box(GLFWwindow* window, const char* text, const char* caption);
