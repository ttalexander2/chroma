#pragma once
#include <Chroma/Renderer/OrthographicCamera.h>
#include <Chroma/Renderer/Texture.h>
#include <Chroma/Math/Math.h>
#include <Chroma/Renderer/Frustum.h>
#include <Chroma/Images/Color.h>

namespace Chroma
{
	class Renderer2D
	{
	public:

		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		static void DrawQuad(const Math::vec2& position, const Math::vec2& size, const Math::vec4& color, float rotation = 0.0f);
		static void DrawQuad(const Math::vec3& position, const Math::vec3& size, const Math::vec4& color, float rotation = 0.0f);

		static void DrawQuad(const Math::vec2& position, const Math::vec2& size, const Ref<Texture2D>& texture, float rotation);
		static void DrawQuad(const Math::vec3& position, const Math::vec3& size, const Ref<Texture2D>& texture, float rotation);

		static void DrawQuad(const Math::mat4& transform, const Math::vec4& color = Math::vec4(1.0f));
		static void DrawQuad(const Math::mat4& transform, const Ref<Texture2D>& texture, const Math::vec4& color = Math::vec4(1.0f));

		static void DrawQuad(const Math::vec2& position, const Math::vec2& size, const Ref<Texture2D>& texture, const Math::vec4& color = Math::vec4(1.0f), float rotation = 0.0f);
		static void DrawQuad(const Math::vec3& position, const Math::vec3& size, const Ref<Texture2D>& texture, const Math::vec4& color = Math::vec4(1.0f), float rotation = 0.0f);

		static void Clear();
		static void SetClearColor(const Color& color);
		static void SetClearColor(const Math::vec4& color);

		//Stats
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

		static Statistics GetStats();
		static void ResetStats();

	private:
		static void FlushAndReset();
	};
}


