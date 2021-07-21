#include "chromapch.h"
#include "SpriteRenderer.h"
#include "Chroma/ImGui/Widgets/Vec3WithLabels.h"
#include "Chroma/ImGui/Widgets/EditableList.h"
#include "Chroma/ImGui/Widgets/AlternateCheckBox.h"
#include "imgui_internal.h"
#include "Chroma/Core/Application.h"
#include "Chroma/Assets/AssetManager.h"

namespace Chroma
{
	
	void SpriteRenderer::DrawImGui()
	{
		DrawComponentValue("Color");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		float col[4]{ Color.r, Color.b, Color.g, Color.a };
		if (ImGui::ColorEdit4("##Color", col))
		{
			Color = { col[0], col[1], col[2], col[3] };
		}

		
		DrawComponentValue("Offset");
		Chroma::Vec3IntWithLabels("##offset", Offset);

		DrawComponentValue("Scale");
		Chroma::Vec3FloatWithLabels("##scale", Scale, false);

		DrawComponentValue("Play On Start");
		ImGui::Checkbox("##play_on_start", &PlayOnStart);

		DrawComponentValue("Playing");
		ImGui::Checkbox("##playing", &Playing);

		//DrawComponentValue("Layer");

		//static bool editing = false;

		/*
		if (!editing && ImGui::BeginCombo("##Layer", Layer.c_str()))
		{
			for (std::string name : Application::Get().m_ActiveScene->Layers)
			{
				bool isSelected = (name == Layer);
				if (ImGui::Selectable(name.c_str(), isSelected))
					Layer = name;

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::SameLine();

		if (std::find(Application::Get().m_ActiveScene->Layers.begin(), Application::Get().m_ActiveScene->Layers.end(), "Default") == Application::Get().m_ActiveScene->Layers.end())
		{
			Application::Get().m_ActiveScene->Layers.push_back("Default");
		}

		if (std::find(Application::Get().m_ActiveScene->Layers.begin(), Application::Get().m_ActiveScene->Layers.end(), Layer) == Application::Get().m_ActiveScene->Layers.end())
		{
			Layer = "Default";
		}


		if (!editing && ImGui::Button(ICON_FK_COG))
		{
			editing = true;
		}

		if (editing)
		{
			DrawComponentValue("    " ICON_FK_ANGLE_DOUBLE_RIGHT);
			ImGui::EditableList(Application::Get().m_ActiveScene->Layers, true);
			if (ImGui::Button(ICON_FK_CHECK " Accept"))
			{
				editing = false;
			}
		}

		*/


		//bool open = DrawComponentValueCollapsible("Layer");
		//ImGui::EditableList(this->ListTest, open);
	

	}

	void SpriteRenderer::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Color";
		out << YAML::Value << Color;

		out << YAML::Key << "Offset";
		out << YAML::Value << Offset;

		out << YAML::Key << "Scale";
		out << YAML::Value << Scale;

		out << YAML::Key << "Layer";
		out << YAML::Value << Layer;

		out << YAML::Key << "PlayOnStart";
		out << YAML::Value << PlayOnStart;

		out << YAML::Key << "Sprite";
		out << YAML::Value << Sprite;


	}
	void SpriteRenderer::Deserialize(YAML::Node& node)
	{
		auto val = node["Color"];
		if (val)
		{
			Color = val.as<Math::vec4>();
		}
		val = node["Offset"];
		if (val)
		{
			Offset = val.as<Math::vec3>();
		}
		val = node["Scale"];
		if (val)
		{
			Scale = val.as<Math::vec3>();
		}
		val = node["Layer"];
		if (val)
		{
			Layer = val.as<std::string>();
		}
		val = node["PlayOnStart"];
		if (val)
		{
			PlayOnStart = val.as<bool>();
		}
		val = node["Sprite"];
		if (val)
		{
			Sprite = val.as<std::string>();
		}

	}
	Ref<Chroma::Sprite> SpriteRenderer::GetSprite()
	{
		return _sprite;
	}
}


