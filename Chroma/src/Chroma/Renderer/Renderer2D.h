#pragma once
#include <Chroma/Renderer/OrthographicCamera.h>
#include <Chroma/Renderer/Texture.h>
#include <Chroma/Math/Math.h>
#include <Chroma/Renderer/Frustum.h>
#include <Chroma/Images/Color.h>
#include <Chroma/Components/Camera.h>
#include <Chroma/Renderer/Shader.h>
#include <Chroma/Assets/Font.h>

namespace Chroma
{
	class Scene;

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void Begin(const Camera &camera);
		static void Begin(const OrthographicCamera &camera);
		static void Begin(const Math::mat4 &viewProjMat);
		static void StartBatch();
		static void NextBatch();
		static void End();
		static void Flush();
		static void Flush(const Ref<Shader> &shader);

		static void DrawSprite(int entityID, const Math::vec2 &position, const Math::vec2 &size, const Ref<Texture2D> &texture, const Math::vec4 &color, float rotation);

		static void DrawQuad(const Math::vec2 &position, const Math::vec2 &size, const Math::vec4 &color, float rotation = 0.0f);
		static void DrawQuad(const Math::vec3 &position, const Math::vec3 &size, const Math::vec4 &color, float rotation = 0.0f);

		static void DrawQuad(const Math::vec2 &position, const Math::vec2 &size, const Ref<Texture2D> &texture, float rotation);
		static void DrawQuad(const Math::vec3 &position, const Math::vec3 &size, const Ref<Texture2D> &texture, float rotation);

		static void DrawQuad(const Math::mat4 &transform, const Math::vec4 &color = Math::vec4(1.0f));
		static void DrawQuad(const Math::mat4 &transform, const Ref<Texture2D> &texture, const Math::vec4 &color = Math::vec4(1.0f));

		static void DrawQuad(const Math::vec2 &position, const Math::vec2 &size, const Ref<Texture2D> &texture, const Math::vec4 &color = Math::vec4(1.0f), float rotation = 0.0f);
		static void DrawQuad(const Math::vec3 &position, const Math::vec3 &size, const Ref<Texture2D> &texture, const Math::vec4 &color = Math::vec4(1.0f), float rotation = 0.0f);

		static void DrawRect(const Math::vec2 &position, const Math::vec2 &size, float line_width, const Math::vec4 &color, float rotation = 0.f);
		static void DrawLine(const Math::vec2 &p1, const Math::vec2 &p2, float line_width, const Math::vec4 &color);

		static void DrawTriangle(const Math::vec2 &p1, const Math::vec2 &p2, const Math::vec2 &p3, const Math::vec4 &color);
		static void DrawTriangle(const Math::vec3 &p1, const Math::vec3 &p2, const Math::vec3 &p3, const Math::vec4 &color);

		static void DrawString(const std::string &string, const Ref<Font> &font, const Math::vec3 &position, float maxWidth, const glm::vec4 &color, float scale, float lineHeightOffset, float kerningOffset);
		static void DrawString(const std::u16string &string, const Ref<Font> &font, const Math::vec3 &position, float maxWidth, const glm::vec4 &color, float scale, float lineHeightOffset, float kerningOffset);
		static void DrawString(const std::u32string &string, const Ref<Font> &font, const Math::vec3 &position, float maxWidth, const glm::vec4 &color, float scale, float lineHeightOffset, float kerningOffset);

		static void Clear();
		static void SetClearColor(const Color &color);
		static void SetClearColor(const Math::vec4 &color);

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
