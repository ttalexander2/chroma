#pragma once
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

		static void DrawObject(Chroma::Component* c);

		static bool show_context;
		static std::function<void(const std::string&, unsigned int)> context_function;
		
		static void DrawBool(Chroma::Component* c, Chroma::Reflection::data data);
		static void DrawFloat(Chroma::Component* c, Chroma::Reflection::data data);
		static void DrawDouble(Chroma::Component* c, Chroma::Reflection::data data);
		static void DrawInt32(Chroma::Component* c, Chroma::Reflection::data data);
		static void DrawUInt32(Chroma::Component* c, Chroma::Reflection::data data);
		static void DrawInt64(Chroma::Component* c, Chroma::Reflection::data data);
		static void DrawUInt64(Chroma::Component* c, Chroma::Reflection::data data);
		static void DrawChar(Chroma::Component* c, Chroma::Reflection::data data);
		static void DrawUChar(Chroma::Component* c, Chroma::Reflection::data data);
		static void DrawString(Chroma::Component* c, Chroma::Reflection::data data);
		static void DrawAsset(Chroma::Component* c, Chroma::Reflection::data data);
		static void DrawVec2(Chroma::Component* c, Chroma::Reflection::data data);
		static void DrawVec3(Chroma::Component* c, Chroma::Reflection::data data);
		static void DrawVec4(Chroma::Component* c, Chroma::Reflection::data data);
		static void DrawUVec2(Chroma::Component* c, Chroma::Reflection::data data);
		static void DrawUVec3(Chroma::Component* c, Chroma::Reflection::data data);
		static void DrawUVec4(Chroma::Component* c, Chroma::Reflection::data data);
		static void DrawEntity(Chroma::Component* c, Chroma::Reflection::data data);
		
	};
}