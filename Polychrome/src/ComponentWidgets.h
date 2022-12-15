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

		static void DrawObject(Chroma::Component* c, Chroma::Reflection::AnyRef val);

		static bool show_context;
		static std::function<void(const std::string&, unsigned int)> context_function;

		static void DrawTransform(Chroma::Component* c);
		static void DrawCSharpScript(Chroma::Component* c);
		static void DrawAudioSource(Chroma::Component* c);
		static void DrawCollider(Chroma::Component* c);
		static void DrawSpriteRenderer(Chroma::Component* c);
		static void DrawCameraComponent(Chroma::Component* c);
		static void DrawParticleEmitter(Chroma::Component* c);

		static void DrawBool(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
		static void DrawFloat(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
		static void DrawDouble(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
		static void DrawInt32(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
		static void DrawUInt32(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
		static void DrawInt64(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
		static void DrawUInt64(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
		static void DrawChar(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
		static void DrawUChar(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
		static void DrawString(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
		static void DrawAsset(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
		static void DrawVec2(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
		static void DrawVec3(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
		static void DrawVec4(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
		static void DrawUVec2(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
		static void DrawUVec3(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
		static void DrawUVec4(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
		static void DrawEntity(Chroma::Component* c, Chroma::Reflection::Data data,  Chroma::Reflection::AnyRef handle);
	};
}