#pragma once

#include <glm/glm.hpp>

namespace Chroma
{
	enum class PrimitiveType
	{
		None = 0,
		Triangles,
		Lines
	};

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL = 1,
			DirectX = 2,
			Vulkan = 3
		};

	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;

		virtual void SetClearColor(const glm::vec4 &color) = 0;
		virtual void Clear() = 0;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest = true) = 0;

		static API GetAPI() { return s_API; }

	private:
		static API s_API;
	};
}
