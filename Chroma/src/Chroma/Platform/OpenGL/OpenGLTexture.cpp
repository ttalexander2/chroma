#include "chromapch.h"
#include "OpenGLTexture.h"


#include "Chroma/IO/File.h"
#include "Chroma/IO/FileSystem.h"

#include "stb_image.h"


#include <glad/glad.h>


namespace Chroma
{



	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, bool flip_vertically)
		:m_Path(path)
	{
		int width, height, channels;

		if (flip_vertically)
			stbi_set_flip_vertically_on_load(1);
		else
			stbi_set_flip_vertically_on_load(0);

		auto file = Chroma::File::Open(path);
		if (!file.Good())
		{
			CHROMA_CORE_ERROR("Failed to load texture: [{}]", path);
			return;
		}

		int64_t len = file.Length();
		std::vector<unsigned char> raw_data(len);
		file.Read(raw_data.data(), len);

		file.Close();

		stbi_uc* data = stbi_load_from_memory(raw_data.data(), static_cast<int>(len), &width, &height, &channels, 0);


		CHROMA_ASSERT(data, "Failed to load image \'{0}\'", path);
		m_Width = width;
		m_Height = height;

		//CHROMA_CORE_TRACE("{0}: Channels: {1}", path, channels);

		if (channels == 4)
		{
			m_InternalFormat = GL_RGBA8;
			m_DataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			m_InternalFormat = GL_RGB8;
			m_DataFormat = GL_RGB;
		}

		CHROMA_ASSERT(m_InternalFormat & m_DataFormat, "Texture format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, width, height, m_DataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		:m_Width(width), m_Height(height)
	{

		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		CHROMA_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Texture format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bytesPerPixel = m_DataFormat == GL_RGBA ? 4 : 3;
		CHROMA_CORE_ASSERT(size == m_Width * m_Height * bytesPerPixel, "Data does not match texture size");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::SetTextureFormat(TextureFormat format)
	{
		switch (format)
		{
			case TextureFormat::RGB8:
			{
				m_DataFormat = GL_RGB;
				m_InternalFormat = GL_RGB8;
				break;
			}
			case TextureFormat::RGBA8:
			{
				m_DataFormat = GL_RGBA;
				m_InternalFormat = GL_RGBA8;
				break;
			}
		}
	}

	void OpenGLTexture2D::SetFiltering(FilterMethod method, FilterType type)
	{
		int gl_method = GL_LINEAR;
		int gl_type = GL_TEXTURE_MIN_FILTER;

		switch (method)
		{
			case FilterMethod::LINEAR:
			{
				gl_method = GL_LINEAR;
				break;
			}
			case FilterMethod::NEAREST:
			{
				gl_method = GL_NEAREST;
				break;
			}
		}

		switch (type)
		{
			case FilterType::MIN:
			{
				gl_type = GL_TEXTURE_MIN_FILTER;
				break;
			}
			case FilterType::MAG:
			{
				gl_type = GL_TEXTURE_MAG_FILTER;
				break;
			}
		}

		glTextureParameteri(m_RendererID, gl_type, gl_method);
	}

}