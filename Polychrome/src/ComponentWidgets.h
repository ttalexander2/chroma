#include <Chroma/Scene/Component.h>

namespace Polychrome
{
	class ComponentWidgets
	{

		friend class Inspector;

	public:
		static void Draw(Chroma::Component* c);
	private:


		static void DrawComponentValue(Chroma::Component* c, const std::string& label);
		static bool DrawComponentValueCollapsible(Chroma::Component* c, const std::string& label);

		static bool show_context;
		static std::function<void(const std::string&, unsigned int)> context_function;

		static void DrawTransform(Chroma::Component* c);
		static void DrawCSharpScript(Chroma::Component* c);
		static void DrawAudioSource(Chroma::Component* c);
		static void DrawBoxCollider2D(Chroma::Component* c);
		static void DrawCircleCollider2D(Chroma::Component* c);
		static void DrawLuaScript(Chroma::Component* c);
		static void DrawSpriteRenderer(Chroma::Component* c);
	};
}