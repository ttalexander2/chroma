#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Audio/Audio.h"
#include "Chroma/ImGui/ImGuiComponents/ComboFilter.h"

namespace Chroma
{
	struct AudioSource : Component
	{
		std::string Event;
		FMOD_GUID GUID;
		bool Mute = false;
		bool PlayOnInit = false;

		AudioSource() = default;
		AudioSource(const AudioSource&) = default;
		AudioSource(const std::string& evnt, bool loop = false, bool mute = false, bool play_on_init = false)
			: Event(evnt), Mute(mute), PlayOnInit(play_on_init), GUID()
		{
		}

		const std::string Name() const override
		{
			return "Audio Source";
		}

		void DrawImGui() override
		{
			if (ImGui::BeginTable("##audio_source_table_inspector", 2, ImGuiTableFlags_None | ImGuiTableFlags_SizingFixedFit))
			{
				ImGui::TableNextColumn();

				ImGui::AlignTextToFramePadding(); ImGui::Text("Event");
				ImGui::AlignTextToFramePadding(); ImGui::Text("Mute");
				ImGui::AlignTextToFramePadding(); ImGui::Text("Play on Init");

				ImGui::TableNextColumn();

				ImGui::PushItemWidth(0);

				auto hints = Audio::GetEventPathList();
				if (hints.size() <= 0)
					return;

				std::sort(hints.begin(), hints.end());

				static ComboFilterState s = { 0 };
				static char buf[512] = "type text here...";
				//if (ComboFilter("##AUDIO_SOURCE_COMPONENT_" + GetUniqueID(), buf, IM_ARRAYSIZE(buf), hints, hints.size(), s))
				//{
				//	Event = std::string(buf);
				//	GUID = Audio::GetEventGuid(Event);
				//}

				if (ImGui::InputText("##AUDIO_SOURCE_COMPONENT_" + GetUniqueID(), buf, IM_ARRAYSIZE(buf)))
				{
					Event = std::string(buf);
					GUID = Audio::GetEventGuid(Event);
				}


				if (ImGui::Checkbox("##AUDIO_SOURCE_Mute", &Mute))
				{

				}


				if (ImGui::Checkbox("##AUDIO_SOURCE_Play on Init", &PlayOnInit))
				{

				}


				



				ImGui::PopItemWidth();

				ImGui::EndTable();
			}

			
		}

		void Serialize(YAML::Emitter& out) override
		{
			out << YAML::Key << "Event";
			out << YAML::Value << Event;
		}

		void Deserialize(YAML::Node& node) override
		{
			auto val = node["Event"];
			if (val)
			{
				Event = val.as<std::string>();
			}

		}

	};
}

