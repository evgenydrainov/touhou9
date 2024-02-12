#include "platform.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <Windows.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

u8* platform_read_entire_file(const char* filepath, int* filesize) {
	FILE *f;
	fopen_s(&f, filepath, "rb");

	// assert(f);
	if (!f) {
		return nullptr;
	}

	fseek(f, 0, SEEK_END);
	int fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

	u8* buffer = (u8*) malloc(fsize);
	assert(buffer);
	fread(buffer, fsize, 1, f);
	fclose(f);

	*filesize = fsize;

	return buffer;
}

void platform_sleep(u32 ms) {
	timeBeginPeriod(1);
	Sleep(ms);
	timeEndPeriod(1);
}

void platform_message_box(GLFWwindow* window, const char* text, const char* caption) {
	HWND hwnd = (window) ? glfwGetWin32Window(window) : nullptr;
	MessageBoxA(hwnd, text, caption, 0);
}
