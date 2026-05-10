#include "surface/Texture.h"
#define STBI_WINDOWS_UTF8
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <utils/logger.h>
#include <format>
#include <array>

namespace cme {
	Texture::~Texture()
	{
		if (_image) {
			stbi_image_free(_image);
			_image = nullptr;
		}

		if (mId != 0)
			glDeleteTextures(1, &mId);
	}

	void
		Texture::init(GLenum target)
	{
		glGenTextures(1, &mId);
		glBindTexture(GL_TEXTURE_2D, mId);

		// Filters and clamping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     // GL_CLAMP
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     // GL_CLAMP
	}

	void Texture::load(const std::string& name, TextureType type, GLubyte alpha) {
		mType = type;
		if (mId == 0) init(GL_TEXTURE_2D);

		int channels;

		if (type == TextureType::HDR) {
			// HDR — floats, necesario para skybox/IBL
			stbi_set_flip_vertically_on_load(true);
			float* data = stbi_loadf(name.c_str(), &mWidth, &mHeight, &channels, 0);
			if (!data) {
				LOG_ERROR(std::format("No se pudo cargar HDR: {}", name));
				return;
			}

			glBindTexture(GL_TEXTURE_2D, mId);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mWidth, mHeight, 0,
				GL_RGB, GL_FLOAT, data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(data);
			stbi_set_flip_vertically_on_load(false);
			return;
		}

		// DEFAULT y LINEAR — unsigned byte
		_image = stbi_load(name.c_str(), &mWidth, &mHeight, &channels, 0);
		if (!_image) {
			LOG_ERROR(std::format("No se pudo cargar la textura: {}", name));
			return;
		}

		// Formato interno: sRGB si DEFAULT, lineal si LINEAR
		GLenum internalFormat, dataFormat;
		if (channels == 1) {
			internalFormat = GL_RED;
			dataFormat = GL_RED;
		}
		else if (channels == 3) {
			internalFormat = (type == TextureType::DEFAULT) ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else {
			internalFormat = (type == TextureType::DEFAULT) ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, mId);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
			mWidth, mHeight, 0, dataFormat, GL_UNSIGNED_BYTE, _image);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::loadCubemap(const std::array<GLuint, 6>& texIds, const std::array<GLsizei, 6>& widths, const std::array<GLsizei, 6>& heights) {
		mType = TextureType::CUBEMAP;
		if (mId == 0) init(GL_TEXTURE_CUBE_MAP);

		// Crear las 6 caras vacías
		glBindTexture(GL_TEXTURE_CUBE_MAP, mId);
		for (int i = 0; i < 6; i++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB,
				widths[i], heights[i], 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		}

		// Copiar cada textura 2D de la GPU a su cara
		GLuint fbo;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

		for (int i = 0; i < 6; i++) {
			glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D, texIds[i], 0);
			glCopyTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
				0, 0, 0, 0, widths[i], heights[i]);
		}

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &fbo);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void Texture::bind() {
		glBindTexture(GL_TEXTURE_2D, mId);
	}

	void
		Texture::setWrap(GLuint wp) // GL_REPEAT, GL_CLAMP_TO_EDGE, ...
	{
		glBindTexture(GL_TEXTURE_2D, mId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wp);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wp);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::loadColorBuffer(GLsizei width, GLsizei height, GLuint buffer) {
		glReadBuffer(buffer);
		glBindTexture(GL_TEXTURE_2D, mId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, width, height, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}