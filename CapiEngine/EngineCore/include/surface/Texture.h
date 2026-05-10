#ifndef _H_Texture_H_
#define _H_Texture_H_
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <string>

namespace cme {
	enum class TextureType {
		DEFAULT,    // sRGB estándar (GL_SRGB / GL_SRGB_ALPHA)
		LINEAR,     // No sRGB, lineal (normal maps, roughness, metallic...)
		HDR,        // Float HDR (.hdr para skybox/IBL)
		CUBEMAP     // Skybox (6 caras)
	};

	class Texture
	{
	public:
		Texture() = default;
		~Texture();

		Texture(const Texture& tex) = delete;            // no copy constructor
		Texture& operator=(const Texture& tex) = delete; // no copy assignment

		void load(const std::string& filename,
			TextureType type = TextureType::DEFAULT,
			GLubyte alpha = 255);

		void loadCubemap(const std::array<GLuint, 6>& texIds, const std::array<GLsizei, 6>& widths, const std::array<GLsizei, 6>& heights);

		void bind();
		void unbind() const { glBindTexture(GL_TEXTURE_2D, 0); };

		GLuint width() const { return mWidth; };
		GLuint height() const { return mHeight; };
		GLuint id() { return mId; }
		TextureType type() { return mType; }

		void setWrap(GLuint wp); // GL_REPEAT, GL_CLAMP_TO_EDGE, ...

		void loadColorBuffer(GLsizei width, GLsizei height, GLuint buffer = GL_FRONT);

		std::string name() { return _name; }
		void setName(std::string n) { _name = n; }
	protected:
		void init(GLenum target);

		int mWidth = 0;
		int mHeight = 0;
		GLuint mId = 0;
		TextureType mType = TextureType::DEFAULT;
		GLenum      mTarget = GL_TEXTURE_2D;

		std::string _name;
		unsigned char* _image = nullptr;
	};
}
#endif //_H_Texture_H_
