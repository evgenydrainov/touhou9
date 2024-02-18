#include "texture.h"

#include <glad/gl.h>
#include <assert.h>

void Texture::create(int width, int height, u8* buffer,
					 int internal_format, u32 format, int filter) {
	u32 texture;
	GLCheck(glGenTextures(1, &texture));
	GLCheck(glBindTexture(GL_TEXTURE_2D, texture));
	GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter));
	GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter));
	GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCheck(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, buffer));

	this->id = texture;
	this->width = width;
	this->height = height;
}

void Texture::create_framebuffer(int width, int height,
								 int internal_format, u32 format, int filter) {
	u32 fbo;
	GLCheck(glGenFramebuffers(1, &fbo));
	GLCheck(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

	u32 texture;
	GLCheck(glGenTextures(1, &texture));
	GLCheck(glBindTexture(GL_TEXTURE_2D, texture));
	GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter));
	GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter));
	GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCheck(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr));

	// GLCheck(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0));
	// u32 draw_buffers[] = {GL_COLOR_ATTACHMENT0};
	// GLCheck(glDrawBuffers(ArrayLength(draw_buffers), draw_buffers));

	GLCheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0));

	{
		u32 status;
		GLCheck(status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
		assert(status == GL_FRAMEBUFFER_COMPLETE);
	}

	this->id = texture;
	this->fbo = fbo;
	this->width = width;
	this->height = height;
}

void Texture::destroy() {
	if (id != 0) {
		GLCheck(glDeleteTextures(1, &id));
	}

	if (fbo != 0) {
		GLCheck(glDeleteFramebuffers(1, &fbo));
	}
}
