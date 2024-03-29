#include "chromapch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Chroma
{



	void OpenGLRendererAPI::Init()
	{

		glEnable(GL_BLEND);
		glDisable(GL_DITHER);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		const double g_width = 1920;
		const double g_height = 1080;
		glMatrixMode(GL_PROJECTION_MATRIX);
		glLoadIdentity();

		glOrtho(0.f, g_width, g_height, 0.f, 1.f, -1.f);

		auto ratioX = width / (float)g_width;
		auto ratioY = height / (float)g_height;
		auto ratio = ratioX < ratioY ? ratioX : ratioY;

		auto viewWidth = (uint32_t)(g_width * ratio);
		auto viewHegiht = (uint32_t)(g_height * ratio);

		auto viewX = (uint32_t)((width - g_width * ratio) / 2.f);
		auto viewY = (uint32_t)((height - g_height * ratio) / 2.f);

		glViewport(viewX, viewY, viewWidth, viewHegiht);
		glMatrixMode(GL_MODELVIEW_MATRIX);
		glLoadIdentity();
	}

	void OpenGLRendererAPI::DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest)
	{
		if (!depthTest)
			glDisable(GL_DEPTH_TEST);

		GLenum glPrimitiveType = 0;
		switch (type)
		{
		case PrimitiveType::Triangles:
			glPrimitiveType = GL_TRIANGLES;
			break;
		case PrimitiveType::Lines:
			glPrimitiveType = GL_LINES;
			break;
		}

		glDrawElements(glPrimitiveType, count, GL_UNSIGNED_INT, nullptr);

		if (!depthTest)
			glEnable(GL_DEPTH_TEST);

	}

}