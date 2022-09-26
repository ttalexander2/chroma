#include "AnimationEditor.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

#include "Fonts/IconsForkAwesome.h"
#include "EditorApp.h"
#include "Hierarchy.h"
#include "Utilities/ReflectionWidget.h"
#include "thid_party/BezierWidget.h"
#include <Chroma/Animation/Animation.h>
#include <string>
#include <ios>
#include <iomanip>
#include <map>
#include <sstream>
#include <Chroma/Core/Input.h>
#include <Chroma/Reflection/Reflection.h>
#include <Chroma/Utilities/StringHash.h>
#include <Chroma/Components/Tag.h>
#include <Chroma/Components/AnimationPlayer.h>

namespace Polychrome
{

	bool AnimationEditor::Open = true;
	bool AnimationEditor::Playing = false;
	bool AnimationEditor::Snap = false;
	float AnimationEditor::Step = 0.5f;
	float AnimationEditor::TimeSelection = 1.f;
	float AnimationEditor::Zoom = 1.f;

	static float trackHeight = 4.f;

	static Chroma::Animation::Keyframe* selectedKeyframe = nullptr;
	static Chroma::Animation::Track* selectedTrack = nullptr;

	inline float snap(float original, float step)
	{
		return roundf(original * (1/step)) * step;
	}

	inline float clamp(float original, float min, float max)
	{
		if (original < min)
			return min;
		if (original > max)
			return max;
		return original;
	}

	inline float distance(ImVec2 v1, ImVec2 v2)
	{
		float x = v1.x - v2.x, y = v1.y - v2.y;
		return sqrtf(x * x + y * y);
	}

	

	void AnimationEditor::Draw(float deltaTime)
	{
		static bool created = false;
		static Chroma::Animation animation;

		static Chroma::AnimationPlayer animationPlayer;


		//Update animation values
		if (Playing)
		{
			//CHROMA_CORE_INFO("TimeSelection: {}, deltaTime: {}", TimeSelection, deltaTime);

			TimeSelection += deltaTime;

			if (TimeSelection >= animation.length)
				Playing = false;

			for (auto& track : animation.tracks)
			{
				Chroma::Component* comp = EditorApp::CurrentScene->GetComponent(track.componentID, track.entityID);

				auto type = Chroma::Reflection::Resolve(track.componentID);

				for (auto& keyframe : track.keyframes)
				{
					if (TimeSelection < keyframe.time)
					{
						type.Set(track.propertyID, Chroma::Reflection::Handle(comp), keyframe.value);
						break;
					}
						
				}
			}
		}


		std::map<Chroma::EntityID, std::map<uint32_t, std::map<uint32_t, Chroma::Animation::Track*>>> track_map;
		std::map<Chroma::EntityID, std::map<uint32_t, std::map<uint32_t, Chroma::Animation::Track*>>> visible_track_map;

		for (Chroma::Animation::Track& track : animation.tracks)
		{
			track_map[track.entityID][track.componentID][track.propertyID] = &track;
		}

		if (!created)
		{
			animation.length = 5.f;
			animation.loop_type = Chroma::Animation::LoopType::loop;
			if (Hierarchy::SelectedEntity != Chroma::ENTITY_NULL && EditorApp::CurrentScene->Registry.valid(Hierarchy::SelectedEntity))
			{
				animation.length = 5.f;
				{				
					Chroma::Animation::Track track;
					track.componentID = Chroma::Reflection::Resolve<Chroma::Transform>().Id();
					track.propertyID = Chroma::Reflection::Resolve<Chroma::Transform>().Data("Position").Id();
					track.update = Chroma::Animation::UpdateType::continuous;
					track.entityID = Hierarchy::SelectedEntity;
					Chroma::Animation::Keyframe kf1;
					kf1.time = 0.f;
					kf1.value = Math::vec2(99, 22);
					track.keyframes.emplace(kf1);
					Chroma::Animation::Keyframe kf2;
					kf2.time = 2.3f;
					kf2.value = Math::vec2(11, 32);
					track.keyframes.emplace(kf2);
					Chroma::Animation::Keyframe kf3;
					kf3.time = 1.9f;
					kf3.value = Math::vec2(-138, 2);
					track.keyframes.emplace(kf3);
					Chroma::Animation::Keyframe kf4;
					kf4.time = 5.f;
					kf4.value = Math::vec2(1, 1);
					track.keyframes.emplace(kf4);
					animation.tracks.push_back(track);
				}

				{
					Chroma::Animation::Track track;
					track.componentID = entt::resolve<Chroma::Transform>().id();
					track.propertyID = entt::resolve<Chroma::Transform>().data(StringHash::Hash("Rotation")).id();
					track.update = Chroma::Animation::UpdateType::continuous;
					track.entityID = Hierarchy::SelectedEntity;
					Chroma::Animation::Keyframe kf1;
					kf1.time = 0.f;
					kf1.value = 0.f;
					track.keyframes.emplace(kf1);
					Chroma::Animation::Keyframe kf2;
					kf2.time = 2.3f;
					kf2.value = 90.f;
					track.keyframes.emplace(kf2);
					Chroma::Animation::Keyframe kf3;
					kf3.time = 1.9f;
					kf3.value = 270.f;
					track.keyframes.emplace(kf3);
					Chroma::Animation::Keyframe kf4;
					kf4.time = 5.f;
					kf4.value = 0.f;
					track.keyframes.emplace(kf4);
					animation.tracks.push_back(track);
				}




				Chroma::Animation::Track track2;
				track2.componentID = entt::resolve<Chroma::Transform>().id();
				track2.propertyID = entt::resolve<Chroma::Transform>().data(StringHash::Hash("Scale")).id();
				track2.update = Chroma::Animation::UpdateType::continuous;
				track2.entityID = Hierarchy::SelectedEntity;
				Chroma::Animation::Keyframe kf1;
				kf1.time = 3.f;
				kf1.value = Math::vec2(1, 4444);
				track2.keyframes.emplace(kf1);
				animation.tracks.push_back(track2);
				//{
				//	Chroma::Animation::Track track3;
				//	track3.componentID = entt::resolve<Chroma::Transform>().id();
				//	track3.propertyID = entt::resolve<Chroma::Transform>().data(StringHash::Hash("Position")).id();
				//	track3.update = Chroma::Animation::UpdateType::continuous;
				//	track3.entityID = (Chroma::EntityID)((uint32_t)Hierarchy::SelectedEntity + 1);
				//	kf1.time = 0.1f;
				//	kf1.value = Math::vec2(99, 22);
				//	track3.keyframes.emplace(kf1);
				//	kf1.time = 3.3f;
				//	kf1.value = Math::vec2(11, 32);
				//	track3.keyframes.emplace(kf1);
				//	kf1.time = 1.9f;
				//	kf1.value = Math::vec2(-138, 2);
				//	track3.keyframes.emplace(kf1);
				//	kf1.time = 9.f;
				//	kf1.value = Math::vec2(1, 1);
				//	track3.keyframes.emplace(kf1);
				//	animation.tracks.push_back(track3);
				//}
				//{
				//	Chroma::Animation::Track track3;
				//	track3.componentID = entt::resolve<Chroma::Transform>().id();
				//	track3.propertyID = entt::resolve<Chroma::Transform>().data(StringHash::Hash("Rotation")).id();
				//	track3.update = Chroma::Animation::UpdateType::continuous;
				//	track3.entityID = (Chroma::EntityID)((uint32_t)Hierarchy::SelectedEntity + 1);
				//	kf1.time = 0.f;
				//	kf1.value = 0.f;
				//	track3.keyframes.emplace(kf1);
				//	Chroma::Animation::Keyframe kf2;
				//	kf2.time = 2.3f;
				//	kf2.value = 90.f;
				//	track3.keyframes.emplace(kf2);
				//	Chroma::Animation::Keyframe kf3;
				//	kf3.time = 1.9f;
				//	kf3.value = 270.f;
				//	track3.keyframes.emplace(kf3);
				//	Chroma::Animation::Keyframe kf4;
				//	kf4.time = 5.f;
				//	kf4.value = 0.f;
				//	track3.keyframes.emplace(kf4);
				//	animation.tracks.push_back(track3);
				//}
				//{
				//	Chroma::Animation::Track track3;
				//	track3.componentID = entt::resolve<Chroma::Transform>().id();
				//	track3.propertyID = entt::resolve<Chroma::Transform>().data(StringHash::Hash("Position")).id();
				//	track3.update = Chroma::Animation::UpdateType::continuous;
				//	track3.entityID = (Chroma::EntityID)((uint32_t)Hierarchy::SelectedEntity + 2);
				//	kf1.time = 0.1f;
				//	kf1.value = Math::vec2(99, 22);
				//	track3.keyframes.emplace(kf1);
				//	kf1.time = 3.3f;
				//	kf1.value = Math::vec2(11, 32);
				//	track3.keyframes.emplace(kf1);
				//	kf1.time = 1.9f;
				//	kf1.value = Math::vec2(-138, 2);
				//	track3.keyframes.emplace(kf1);
				//	kf1.time = 9.f;
				//	kf1.value = Math::vec2(1, 1);
				//	track3.keyframes.emplace(kf1);
				//	animation.tracks.push_back(track3);
				//}
				//{
				//	Chroma::Animation::Track track3;
				//	track3.componentID = entt::resolve<Chroma::Transform>().id();
				//	track3.propertyID = entt::resolve<Chroma::Transform>().data(StringHash::Hash("Rotation")).id();
				//	track3.update = Chroma::Animation::UpdateType::continuous;
				//	track3.entityID = (Chroma::EntityID)((uint32_t)Hierarchy::SelectedEntity + 2);
				//	kf1.time = 0.f;
				//	kf1.value = 0.f;
				//	track3.keyframes.emplace(kf1);
				//	Chroma::Animation::Keyframe kf2;
				//	kf2.time = 2.3f;
				//	kf2.value = 90.f;
				//	track3.keyframes.emplace(kf2);
				//	Chroma::Animation::Keyframe kf3;
				//	kf3.time = 1.9f;
				//	kf3.value = 270.f;
				//	track3.keyframes.emplace(kf3);
				//	Chroma::Animation::Keyframe kf4;
				//	kf4.time = 5.f;
				//	kf4.value = 0.f;
				//	track3.keyframes.emplace(kf4);
				//	animation.tracks.push_back(track3);
				//}


				//YAML::Emitter emitter;
				//emitter << YAML::BeginMap;
				//
				//emitter << YAML::Key << "Entities";
				//emitter << YAML::Value << YAML::BeginSeq;
				//
				//auto view = EditorApp::CurrentScene->Registry.view<Chroma::Transform>();
				//for (auto id : view)
				//{
				//	emitter << YAML::BeginMap;
				//	emitter << YAML::Key << "Enitty" << YAML::Value << (uint32_t)id;
				//
				//	std::vector<Chroma::Component*> components = EditorApp::CurrentScene->GetAllComponents(id);
				//	std::sort(components.begin(), components.end(), [](Chroma::Component* a, Chroma::Component* b) {return a->GetOrderID() < b->GetOrderID(); });
				//
				//	emitter << YAML::Key << "Components" << YAML::Value;
				//	emitter << YAML::BeginMap;
				//
				//	for (auto component : components)
				//	{
				//		auto meta_handle = component->GetMetaHandle();
				//		auto meta_type = component->GetMetaType();
				//
				//		std::string component_name = Chroma::Reflection::ResolveComponentID(meta_type.id());
				//		emitter << YAML::Key << component_name;
				//		emitter << YAML::Value << YAML::BeginMap;
				//		
				//		for (auto prop : meta_type.data())
				//		{
				//			std::string prop_name = Chroma::Reflection::ResolveComponentPropertyID(meta_type.id(), prop.id());
				//
				//			entt::meta_any property_data = meta_handle->get(prop.id());
				//			Chroma::Reflection::SerializeProperty(emitter, prop_name, property_data);
				//		}
				//
				//		emitter << YAML::EndMap;
				//	}
				//	emitter << YAML::EndMap;
				//	emitter << YAML::EndMap;
				//
				//}
				//
				//emitter << YAML::EndSeq;
				//emitter << YAML::EndMap;


				//data.get();
				created = true;
			}

		}

		if (TimeSelection < 0)
		{
			TimeSelection = 0;
		}
		if (TimeSelection > animation.length)
		{
			TimeSelection = animation.length;
		}



		if (ImGui::Begin("Animation Editor##editor", &Open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{

			ImVec2 window_size = ImGui::GetContentRegionAvail();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
			if (ImGui::Button(ICON_FK_FAST_BACKWARD))
			{
				std::vector<Chroma::Animation::Keyframe*> keyframes;
				for (auto map_it = track_map.begin(); map_it != track_map.end(); map_it++)
				{
					auto& properties = map_it->second;
					for (auto prop_it = properties.begin(); prop_it != properties.end(); prop_it++)
					{
						auto& track = prop_it->second;
						for (auto track_it = track.begin(); track_it != track.end(); track_it++)
						{
							for (auto& kf : track_it->second->keyframes)
							{
								if (selectedTrack == nullptr || (selectedTrack != nullptr && selectedTrack == track_it->second))
									keyframes.push_back(const_cast<Chroma::Animation::Keyframe*>(&kf));

							}
						}
					}
				}

				std::sort(keyframes.begin(), keyframes.end(), [](Chroma::Animation::Keyframe* kf1, Chroma::Animation::Keyframe* kf2) {
					return kf1->time < kf2->time;
					});

				if (keyframes.size() > 0)
				{
					selectedKeyframe = keyframes[0];
					TimeSelection = selectedKeyframe->time;
				}


			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_FK_STEP_BACKWARD))
			{
				std::vector<Chroma::Animation::Keyframe*> keyframes;
				for (auto map_it = track_map.rbegin(); map_it != track_map.rend(); map_it++)
				{
					auto& properties = map_it->second;
					for (auto prop_it = properties.rbegin(); prop_it != properties.rend(); prop_it++)
					{
						auto& track = prop_it->second;
						for (auto track_it = track.rbegin(); track_it != track.rend(); track_it++)
						{
							for (auto& kf : track_it->second->keyframes) 
							{
								if (selectedTrack == nullptr || (selectedTrack != nullptr && selectedTrack == track_it->second))
									keyframes.push_back(const_cast<Chroma::Animation::Keyframe*>(&kf));

							}
						}
					}
				}

				std::sort(keyframes.begin(), keyframes.end(), [](Chroma::Animation::Keyframe* kf1, Chroma::Animation::Keyframe* kf2) {
					return kf1->time > kf2->time;
				});


				bool found_selected = false;
				for (Chroma::Animation::Keyframe* kf : keyframes)
				{
					if (selectedKeyframe == nullptr)
					{
						if (kf->time <= TimeSelection)
						{
							selectedKeyframe = kf;
							TimeSelection = selectedKeyframe->time;
							break;
						}
					}
					else
					{
						if (kf == selectedKeyframe)
						{
							found_selected = true;
							continue;
						}

						if (!found_selected)
							continue;

						if (kf->time <= selectedKeyframe->time)
						{
							selectedKeyframe = kf;
							TimeSelection = selectedKeyframe->time;
							break;
						}
					}
				}
				
			}
			ImGui::SameLine();
			if (Playing)
			{
				if (ImGui::Button(ICON_FK_STOP))
				{
					Playing = false;
				}
			}
			else
			{
				if (ImGui::Button(ICON_FK_PLAY))
				{
					animationPlayer.SetAnimation(&animation);
					animationPlayer.Start();
					TimeSelection = 0.f;
					Playing = true;
				}
			}
				
			ImGui::SameLine();
			if (ImGui::Button(ICON_FK_STEP_FORWARD))
			{
				std::vector<Chroma::Animation::Keyframe*> keyframes;
				for (auto& [entity, components] : track_map)
				{
					for (auto& [component, properties] : components)
					{
						for (auto& [prop, track] : properties)
						{
							for (auto& kf : track->keyframes) 
							{
								if (selectedTrack == nullptr || (selectedTrack != nullptr && selectedTrack == track))
									keyframes.push_back(const_cast<Chroma::Animation::Keyframe*>(&kf));
							}
						}
					}
				}

				std::sort(keyframes.begin(), keyframes.end(), [](Chroma::Animation::Keyframe* kf1, Chroma::Animation::Keyframe* kf2) {
					return kf1->time < kf2->time;
					});

				bool foundSelected = false;
				for (Chroma::Animation::Keyframe* kf : keyframes)
				{
					if (selectedKeyframe == nullptr)
					{
						if (kf->time >= TimeSelection)
						{
							selectedKeyframe = kf;
							TimeSelection = selectedKeyframe->time;
							break;
						}
					}
					else
					{
						if (kf == selectedKeyframe)
						{
							foundSelected = true;
							continue;
						}

						if (!foundSelected)
							continue;

						if (kf->time >= selectedKeyframe->time)
						{
							selectedKeyframe = kf;
							TimeSelection = selectedKeyframe->time;
							break;
						}
					}
				}
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_FK_FAST_FORWARD))
			{
				std::vector<Chroma::Animation::Keyframe*> keyframes;
				for (auto map_it = track_map.rbegin(); map_it != track_map.rend(); map_it++)
				{
					auto& properties = map_it->second;
					for (auto prop_it = properties.rbegin(); prop_it != properties.rend(); prop_it++)
					{
						auto& track = prop_it->second;
						for (auto track_it = track.rbegin(); track_it != track.rend(); track_it++)
						{
							for (auto& kf : track_it->second->keyframes)
							{
								if (selectedTrack == nullptr || (selectedTrack != nullptr && selectedTrack == track_it->second))
									keyframes.push_back(const_cast<Chroma::Animation::Keyframe*>(&kf));

							}
						}
					}
				}

				std::sort(keyframes.begin(), keyframes.end(), [](Chroma::Animation::Keyframe* kf1, Chroma::Animation::Keyframe* kf2) {
					return kf1->time > kf2->time;
					});

				if (keyframes.size() > 0) 
				{
					selectedKeyframe = keyframes[0];
					TimeSelection = selectedKeyframe->time;
				}

			}


			ImGui::SameLine();

			ImGui::SetNextItemWidth(300);
			ImGui::InputFloat("##time_marker_input", &TimeSelection, Step, (0.0F), "%.2f");

			ImGui::SameLine();

			static bool was_toggled_set = false;
			static bool was_toggled = Snap;
			if (ImGui::IsKeyDown((int)CHROMA_KEY_LEFT_ALT))
			{
				if (!was_toggled_set)
				{
					was_toggled_set = true;
					was_toggled = Snap;
				}

				Snap = false;
			}

			if (ImGui::IsKeyReleased((int)CHROMA_KEY_LEFT_ALT) && was_toggled_set)
			{
				Snap = was_toggled;
			}

			if (Snap)
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Button));
			else
				ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
			if (ImGui::Button(ICON_FK_MAGNET))
				Snap = !Snap;

			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Snap timeline to step intervals.\nHold (left-alt) to temporary disable snap");

			ImGui::PopStyleColor();

			ImGui::Separator();


			//
			//float curr_step = Step;
			//float num = animation.length/Step;
			//float line_pos = start - 14;
			//while (curr_step < animation.length)
			//{
			//	ImGui::SameLine();
			//	line_pos += (end - start)/num * Zoom;
			//	ImGui::SetCursorPosX(line_pos);
			//	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
			//	ImGui::SameLine();
			//	std::stringstream stream;
			//	stream << std::fixed << std::setprecision(2) << curr_step;
			//	std::string s = stream.str();
			//	ImGui::Text(s.c_str());
			//	curr_step += Step;
			//}


			//ImGui::PopStyleVar();




			//if (timeline_moved)
			//{
			//	float mouse_x = mouse_pos.x;
			//	if (mouse_x < start)
			//		mouse_x = start;
			//	if (mouse_x > end)
			//		mouse_x = end;
			//	float normal_end = end - start;
			//	float normal_pos = mouse_x - start;
			//	float p = normal_pos / normal_end;
			//	if (animation.length * p != TimeSelection)
			//	{
			//		TimeSelection = animation.length * p / Zoom;
			//		if (Snap)
			//			TimeSelection = snap(TimeSelection, Step);
			//	}
			//
			//}
			//
			//if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			//{
			//	timeline_moved = false;
			//}
			//
			//float percent = TimeSelection / animation.length * Zoom;
			//ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(start + (end - start) * percent - 2, top - 2), ImVec2(start + (end - start) * percent + 4, top), ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive)));
			//ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(start + (end - start)* percent, top), ImVec2(start + (end - start) * percent + 2, bottom), ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive)));
			//ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(start + (end - start) * percent - 2, bottom), ImVec2(start + (end - start) * percent + 4, bottom + 2), ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive)));



			ImVec2 available = ImGui::GetContentRegionAvail();
			ImVec2 position = ImGui::GetCursorPos();

			static float scroll_y = 0.0f;
			static float scroll_x = 0.0f;
			static float scroll_y_max = 1.0f;
			static float scroll_x_max = 1.0f;
			static float track_width = available.x * 0.15f;
			static float keyframe_width = available.x * 0.15f;
			static bool was_dragging = false;


			ImVec2 start, end = ImVec2(0, 0);



			if (track_width > available.x - keyframe_width - 100.f)
			{
				track_width = available.x - keyframe_width - 100.f;
			}

			if (track_width < 100.f)
			{
				track_width = 100.f;
			}

			if (keyframe_width > available.x - track_width - 100.f)
			{
				keyframe_width = available.x - track_width - 100.f;
			}

			if (keyframe_width < 100.f)
			{
				keyframe_width = 100.f;
			}


			ImGui::BeginGroup();
			ImGui::Dummy({1, ImGui::GetTextLineHeightWithSpacing()});
			//ImGui::Separator();

			//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, ImGui::GetStyle().ItemSpacing.y });

			float track_window_y_size = available.y - 25.f - ImGui::GetTextLineHeightWithSpacing();
			if (Zoom > 1)
			{
				track_window_y_size -= ImGui::GetStyle().ScrollbarSize;
			}

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
			ImVec2 track_frame_padding = ImVec2(2, trackHeight);
			ImVec2 old_frame_padding = ImGui::GetStyle().FramePadding;
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, track_frame_padding);

			bool track_hovered = false;
			ImGuiWindow* trackWindow = nullptr;

			if (ImGui::BeginChild("##track_window", ImVec2(track_width, track_window_y_size), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
			{
				ImGui::Dummy({ 1.f, 1.f });
				trackWindow = ImGui::GetCurrentWindow();

				ImGui::BeginTable("##track_window_atable", 2);

				ImGui::TableSetupColumn("CCC", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("AAA", ImGuiTableColumnFlags_WidthFixed);

				int j = 0;

				for (auto& [entity_id, _components] : track_map) 
				{
					if (visible_track_map.find(entity_id) == visible_track_map.end())
						visible_track_map[entity_id] = std::map<uint32_t, std::map<uint32_t, Chroma::Animation::Track*>>();
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					std::string entity_name = EditorApp::CurrentScene->GetComponent<Chroma::Tag>(entity_id).EntityName;
					if (ImGui::TreeNodeEx(entity_name.c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen))
					{
						for (auto& [component_id, _properties] : _components)
						{
							if (visible_track_map[entity_id].find(component_id) == visible_track_map[entity_id].end())
								visible_track_map[entity_id][component_id] = std::map<uint32_t, Chroma::Animation::Track*>();
							ImGui::TableNextRow();
							ImGui::TableNextColumn();
							std::string component_name = Chroma::Reflection::Resolve(component_id).GetName();

							if (ImGui::TreeNodeEx(component_name.c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen))
							{
								int i = 0;
								for (auto& [property_id, track] : _properties)
								{
									visible_track_map[entity_id][component_id][property_id] = track;
									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_TableRowBgAlt)));
									std::string property_name = Chroma::Reflection::Resolve(component_id).Data(property_id).GetName();
									ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_FramePadding;
									if (selectedTrack == track)
										flags |= ImGuiTreeNodeFlags_Selected;
									if (ImGui::TreeNodeEx(property_name.c_str(), flags))
									{
										ImGui::TreePop();
									}

									if (ImGui::IsItemClicked())
										selectedTrack = track;

									std::string updateStr = ICON_FK_LONG_ARROW_RIGHT;
									if (track->update == Chroma::Animation::UpdateType::discrete)
										updateStr = ICON_FK_ELLIPSIS_H;
									
									
									ImGui::TableNextColumn();
									ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_TableRowBgAlt)));

									ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, ImGui::GetStyle().ItemSpacing.y });

									ImGui::SetNextItemWidth(50.f);

									if (ImGui::BeginCombo(("##update_type_track" + std::to_string(i)).c_str(), updateStr.c_str()))
									{
										if (ImGui::Selectable((ICON_FK_LONG_ARROW_RIGHT + std::string(" Continuous")).c_str(), (const char*)0, track->update == Chroma::Animation::UpdateType::continuous))
											track->update = Chroma::Animation::UpdateType::continuous;
										if (ImGui::Selectable((ICON_FK_ELLIPSIS_H + std::string(" Discrete")).c_str(), (const char*)0, track->update == Chroma::Animation::UpdateType::discrete))
											track->update = Chroma::Animation::UpdateType::discrete;
										ImGui::EndCombo();
									}

									ImGui::SameLine();
									ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
									if (ImGui::Button((ICON_FK_PLUS "##create_keyframe_button" + std::to_string(j)).c_str()))
									{
										bool keyframe_exists = false;
										for (const Chroma::Animation::Keyframe& keyframe_to_check : track->keyframes)
										{
											if (keyframe_to_check.time == TimeSelection)
											{
												selectedKeyframe = const_cast<Chroma::Animation::Keyframe*>(&keyframe_to_check);
												keyframe_exists = true;
											}
										}
										if (!keyframe_exists)
										{
											Chroma::Animation::Keyframe kf;
											kf.time = TimeSelection;
											Chroma::Component* component = EditorApp::CurrentScene->GetComponent(component_name, entity_id);
											auto type = Chroma::Reflection::Resolve(component_id);
											kf.value = type.Get(property_id, Chroma::Reflection::Handle(component));
											track->keyframes.emplace(kf);
											selectedKeyframe = const_cast<Chroma::Animation::Keyframe*>(&*std::prev(track->keyframes.end()));
										}

									}
									ImGui::SameLine();
									ImGui::Dummy({ 5, 1 });

									ImGui::PopStyleVar();

									if (i < _properties.size() - 1)
										ImGui::Separator();
									i++;
									j++;
								}

								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}

				}

				ImGui::EndTable();

				ImGui::SetScrollY(scroll_y);


				if (ImGui::IsWindowHovered())
				{
					track_hovered = true;
				}

				ImGui::EndChild();
			}




			ImGui::PopStyleVar();

			ImGui::PopStyleColor();

			//ImGui::Separator();

			ImGui::EndGroup();



			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::Dummy({ 1, ImGui::GetTextLineHeight() });
			ImVec2 p = ImGui::GetCursorScreenPos();
			p = ImVec2(p.x + 4.f, p.y);
			float splitter_size_y = available.y - 25.f - ImGui::GetTextLineHeightWithSpacing();
			if (Zoom > 1)
				splitter_size_y -=  ImGui::GetStyle().ScrollbarSize;


			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().ItemSpacing.x);
			float cursor_pos_x = ImGui::GetCursorPosX();
			ImGui::SetItemAllowOverlap();
			ImGui::InvisibleButton("##track_splitter", ImVec2(6.0f, splitter_size_y), ImGuiButtonFlags_AllowItemOverlap);
			ImGui::SetCursorPosX(cursor_pos_x - 6.f);


			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
			}


			if (ImGui::IsItemActive())
			{
				track_width += ImGui::GetIO().MouseDelta.x;
			}


			//ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(p.x - ImGui::GetStyle().ItemSpacing.x, p.y + ImGui::GetStyle().ItemSpacing.y), ImVec2(p.x + 8 + ImGui::GetStyle().ItemSpacing.x, p.y + ImGui::GetStyle().ItemSpacing.y + splitter_size_y), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_WindowBg)));

			if (ImGui::IsItemHovered())
				ImGui::GetForegroundDrawList()->AddRectFilled(p + ImVec2(-ImGui::GetStyle().ItemSpacing.x - 3.f, ImGui::GetStyle().ItemSpacing.y), ImVec2(p.x + 2 - ImGui::GetStyle().ItemSpacing.x - 3.f, p.y + ImGui::GetStyle().ItemSpacing.y + splitter_size_y), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_SeparatorHovered)));
			else if (ImGui::IsItemActivated())
				ImGui::GetForegroundDrawList()->AddRectFilled(p + ImVec2(-ImGui::GetStyle().ItemSpacing.x - 3.f, ImGui::GetStyle().ItemSpacing.y), ImVec2(p.x + 2 - ImGui::GetStyle().ItemSpacing.x - 3.f, p.y + ImGui::GetStyle().ItemSpacing.y + splitter_size_y), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_SeparatorActive)));
			else
				ImGui::GetForegroundDrawList()->AddRectFilled(p + ImVec2(-ImGui::GetStyle().ItemSpacing.x - 3.f, ImGui::GetStyle().ItemSpacing.y), ImVec2(p.x + 2 - ImGui::GetStyle().ItemSpacing.x - 3.f, p.y + ImGui::GetStyle().ItemSpacing.y + splitter_size_y), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)));



			ImGui::EndGroup();




			

			float timeline_width = available.x - track_width - keyframe_width;
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().ItemSpacing.x * 2.f + 2.f);


			ImGui::Dummy({ 10, 1 });
			ImGui::SameLine();

			ImGui::BeginGroup();

			start = ImGui::GetCursorScreenPos();



			if (ImGui::BeginChild("##timeline", ImVec2(timeline_width - ImGui::GetStyle().ScrollbarSize, ImGui::GetTextLineHeightWithSpacing()), false, ImGuiWindowFlags_NoScrollbar))
			{
				float curr_step = Step;
				float num = animation.length/Step;
				float line_pos = ImGui::GetCursorPos().x;

				ImGui::Dummy({ (timeline_width - ImGui::GetStyle().ScrollbarSize) * Zoom, ImGui::GetTextLineHeightWithSpacing()});

				while (curr_step < animation.length)
				{
					ImGui::SameLine();
					line_pos += (timeline_width - ImGui::GetStyle().ScrollbarSize)/num * Zoom;
					ImGui::SetCursorPosX(line_pos);
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
					ImGui::SameLine();
					std::stringstream stream;
					stream << std::fixed << std::setprecision(2) << curr_step;
					std::string s = stream.str();
					ImGui::Text(s.c_str());
					curr_step += Step;
				}

				if (ImGui::IsWindowHovered() && ImGui::GetIO().MouseWheel != 0)
					was_dragging = true;
				else
					ImGui::SetScrollX(scroll_x);


				ImGui::EndChild();
			}



			static bool was_timeline_clicked = false;

			if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
			{
				was_timeline_clicked = true;
			}

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				was_timeline_clicked = false;
			}


			if (was_dragging)
			{
				ImGui::SetNextWindowScroll({scroll_x, scroll_y});

				was_dragging = false;
			}

			if (track_hovered && ImGui::GetIO().MouseWheel != 0.f)
			{
				float max_step = trackWindow->InnerRect.GetHeight() * 0.67f;
				float scroll_step = floor(Math::min(5 * trackWindow->CalcFontSize(), max_step));
				ImGui::SetNextWindowScroll({ scroll_x, clamp(scroll_y - ImGui::GetIO().MouseWheel * scroll_step, 0, scroll_y_max)});
			}

			end = ImGui::GetCursorScreenPos() + ImVec2(timeline_width, available.y - 25.f - ImGui::GetTextLineHeightWithSpacing()) + ImVec2(-ImGui::GetStyle().ScrollbarSize, 0);

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, track_frame_padding);


			float selectable_height = ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2;

			ImGuiWindow* childWindow = nullptr;

			Chroma::Animation::Keyframe* keyframeToSet = nullptr;

			float scrollbar_size = 0.f;
			if (Zoom > 1)
			{
				scrollbar_size = ImGui::GetStyle().ScrollbarSize;
			}

			ImGuiWindowFlags flags = ImGuiWindowFlags_None;
			if (Zoom > 1)
			{
				flags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
			}
			if (ImGui::BeginChild("##timeline_keyframes", ImVec2(timeline_width, available.y - 25.f - ImGui::GetTextLineHeightWithSpacing()), false, flags))
			{
				childWindow = ImGui::GetCurrentWindow();
				ImGui::Dummy({(timeline_width - ImGui::GetStyle().ScrollbarSize) * Zoom, 1.f });

				ImGui::BeginTable("##track_window_atable_keyframe", 1, ImGuiTableFlags_SizingStretchProp, ImVec2((timeline_width - scrollbar_size) * Zoom, 0));

				for (auto& [entity_id, _components] : visible_track_map)
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					std::string entity_name = EditorApp::CurrentScene->GetComponent<Chroma::Tag>(entity_id).EntityName;
					ImGui::Selectable(("##" + entity_name).c_str(), false, ImGuiSelectableFlags_Disabled, ImVec2(0 * Zoom, selectable_height));
					for (auto& [component_id, _properties] : _components)
					{
						ImGui::TableNextColumn();
						std::string component_name = Chroma::Reflection::Resolve(component_id).GetName();

						ImGui::Selectable(("##" + component_name).c_str(), false, ImGuiSelectableFlags_Disabled, ImVec2(0 * Zoom, selectable_height));

						int i = 0;
						for (auto& [property_id, track] : _properties)
						{
							ImGui::TableNextRow();
							ImGui::TableNextColumn();
							ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_TableRowBgAlt)));
							std::string property_name = Chroma::Reflection::Resolve(component_id).Data(property_id).GetName();
							ImGui::Selectable(("##" + property_name).c_str(), false, ImGuiSelectableFlags_Disabled, ImVec2(0 * Zoom, selectable_height));
							for (const Chroma::Animation::Keyframe& keyframe : track->keyframes)
							{
								float scroll_offset = 0.f;
								if (ImGui::GetScrollMaxX() > 0.f)
									scroll_offset = ImGui::GetScrollX() / ImGui::GetScrollMaxX();
								float percent = keyframe.time / animation.length;
								float keyframe_x_pos = start.x + (timeline_width - ImGui::GetStyle().ScrollbarSize) * percent * Zoom - ((timeline_width - ImGui::GetStyle().ScrollbarSize) * Zoom - (timeline_width - ImGui::GetStyle().ScrollbarSize)) * scroll_offset;
									

									
								if (keyframe_x_pos >= start.x && keyframe_x_pos <= end.x && (ImGui::GetCursorScreenPos().y - (selectable_height / 2.f)) >= start.y + ImGui::GetTextLineHeightWithSpacing() && (ImGui::GetCursorScreenPos().y - (selectable_height / 2.f)) <= end.y)
								{
									ImVec2 cursorPos = ImGui::GetCursorScreenPos();


									ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_Button);

									float dist = distance(ImGui::GetMousePos(), ImVec2(keyframe_x_pos, ImGui::GetCursorScreenPos().y - (selectable_height / 2.f) - 4.f));

									if (dist < 12.f)
									{
										if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
										{
											keyframeToSet = const_cast<Chroma::Animation::Keyframe*>(&keyframe);
										}
										else
										{
											color = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
										}
									}

									if (selectedKeyframe == &keyframe)
									{
										color = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
									}

									ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(keyframe_x_pos, ImGui::GetCursorScreenPos().y - (selectable_height / 2.f) - 3.f), 6, ImGui::ColorConvertFloat4ToU32(color), 4);
								}
							}
								

							if (i < _properties.size() - 1)
								ImGui::Separator();
							i++;
						}

					}

				}

				ImGui::EndTable();

				if (ImGui::IsItemClicked())
				{
					if (keyframeToSet == nullptr)
					{
						selectedKeyframe = nullptr;
					}

					selectedKeyframe = keyframeToSet;

					if (selectedKeyframe != nullptr)
					{
						TimeSelection = selectedKeyframe->time;
					}
				}

				scroll_y = ImGui::GetScrollY();
				scroll_x = ImGui::GetScrollX();
				scroll_y_max = ImGui::GetScrollMaxY();
				scroll_x_max = ImGui::GetScrollMaxX();

				ImGui::EndChild();
			}

			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			ImGui::EndGroup();



			if (ImGui::IsKeyDown((int)CHROMA_KEY_LEFT_CONTROL) && ImGui::IsItemHovered())
			{
				Zoom += ImGui::GetIO().MouseWheel/5.f;
			}

			if (Zoom < 1.f)
				Zoom = 1.f;
			if (Zoom > 10.f)
				Zoom = 10.f;

			if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
			{

				scroll_x += ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle).x / scroll_x_max * 50.f;
				scroll_y += ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle).y / scroll_y_max * 50.f;
				ImGui::ResetMouseDragDelta();
				scroll_x = clamp(scroll_x, 0, scroll_x_max);
				scroll_y = clamp(scroll_y, 0, scroll_y_max);
				was_dragging = true;
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && ImGui::IsKeyDown((int)CHROMA_KEY_SPACE))
			{
				scroll_x += ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).x / scroll_x_max * 50.f;
				scroll_y += ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).y / scroll_y_max * 50.f;
				ImGui::ResetMouseDragDelta();
				scroll_x = clamp(scroll_x, 0, scroll_x_max);
				scroll_y = clamp(scroll_y, 0, scroll_y_max);
				was_dragging = true;
			}

			ImGui::SameLine();

			//ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
			ImGui::BeginGroup();
			ImGui::Dummy({ 1, ImGui::GetTextLineHeight() });
			p = ImGui::GetCursorScreenPos();
			ImGui::InvisibleButton("##timeline_splitter", ImVec2(8.0f, available.y - 25.f - ImGui::GetStyle().ScrollbarSize - ImGui::GetTextLineHeight()));

			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
			}

			if (ImGui::IsItemHovered())
				ImGui::GetForegroundDrawList()->AddRectFilled(p + ImGui::GetStyle().ItemSpacing, ImVec2(p.x + ImGui::GetStyle().ItemSpacing.x + 2, p.y + ImGui::GetStyle().ItemSpacing.y + splitter_size_y), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_SeparatorHovered)));
			else if (ImGui::IsItemActivated())
				ImGui::GetForegroundDrawList()->AddRectFilled(p + ImGui::GetStyle().ItemSpacing, ImVec2(p.x + ImGui::GetStyle().ItemSpacing.x + 2, p.y + ImGui::GetStyle().ItemSpacing.y + splitter_size_y), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_SeparatorActive)));
			else
				ImGui::GetForegroundDrawList()->AddRectFilled(p + ImGui::GetStyle().ItemSpacing, ImVec2(p.x + ImGui::GetStyle().ItemSpacing.x + 2, p.y + ImGui::GetStyle().ItemSpacing.y + splitter_size_y), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)));
			
			ImGui::EndGroup();

			if (ImGui::IsItemActive())
			{
				keyframe_width -= ImGui::GetIO().MouseDelta.x;
			}

			ImGui::SameLine();

			ImGui::BeginGroup();

			if (ImGui::BeginTabBar("##keyframe_transition_tab"))
			{

				if (ImGui::BeginTabItem("Keyframe"))
				{
					if (ImGui::BeginChild("##keyframe_child", ImVec2(0, ImGui::GetContentRegionAvail().y - ImGui::GetTextLineHeightWithSpacing()))) {
						if (selectedKeyframe != nullptr)
						{
							ImGui::Text("Time");
							ImGui::SameLine();
							ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
							if (ImGui::SliderFloat("##keyframe_time", &selectedKeyframe->time, 0.f, animation.length))
							{
								TimeSelection = selectedKeyframe->time;
							}
							if (Snap && !ImGui::IsKeyDown((int)CHROMA_KEY_LEFT_ALT))
								selectedKeyframe->time = snap(selectedKeyframe->time, Step);

							selectedKeyframe->time = clamp(selectedKeyframe->time, 0, animation.length);

							ImGui::Text("Value");
							ImGui::SameLine();

							DrawMetaAny("##keyframe_value", selectedKeyframe->value);


							ImGui::Text("Transition");
							float v[5] = { selectedKeyframe->transition.a.x, selectedKeyframe->transition.a.y, selectedKeyframe->transition.b.x, selectedKeyframe->transition.b.y };
							ImGui::Bezier("##keyframe_transition", v);
							selectedKeyframe->transition.a = Math::vec2(v[0], v[1]);
							selectedKeyframe->transition.b = Math::vec2(v[2], v[3]);


						}

						ImGui::EndChild();
					}


					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Track"))
				{
					if (selectedTrack != nullptr)
					{
						ImGui::Text("Update Type");
						ImGui::SameLine();

						std::string updateStr = "Continuous";
						if (selectedTrack->update == Chroma::Animation::UpdateType::discrete)
							updateStr = "Discrete";

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());

						if (ImGui::BeginCombo("##update_type", updateStr.c_str())) {
							if (ImGui::Selectable("Continuous", (const char*)0, selectedTrack->update == Chroma::Animation::UpdateType::continuous))
								selectedTrack->update = Chroma::Animation::UpdateType::continuous;
							if (ImGui::Selectable("Discrete", (const char*)0, selectedTrack->update == Chroma::Animation::UpdateType::discrete))
								selectedTrack->update = Chroma::Animation::UpdateType::discrete;
							ImGui::EndCombo();
						}

					}
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}

			ImGui::EndGroup();

			float scroll_offset = 0.f;
			if (scroll_x_max > 0.f)
				scroll_offset = scroll_x / scroll_x_max;

			//Calculate new TimeSelection based on moved playhead
			if (was_timeline_clicked)
			{
				TimeSelection = ((ImGui::GetMousePos().x + ((timeline_width - ImGui::GetStyle().ScrollbarSize) * Zoom - (timeline_width - ImGui::GetStyle().ScrollbarSize)) * scroll_offset) - start.x) / Zoom / (timeline_width - ImGui::GetStyle().ScrollbarSize) * animation.length;
			}

			TimeSelection = clamp(TimeSelection, 0, animation.length);

			if (Snap)
				TimeSelection = snap(TimeSelection, Step);
			

			//Calculate the position of the playhead/set the playhead position based on TimeSelection 
			float percent = TimeSelection / animation.length;

			float pos = start.x + (timeline_width - ImGui::GetStyle().ScrollbarSize) * percent * Zoom - ((timeline_width - ImGui::GetStyle().ScrollbarSize) * Zoom - (timeline_width - ImGui::GetStyle().ScrollbarSize)) * scroll_offset;

			float time_selector_y_pos_end = end.y;
			if (Zoom > 1)
				time_selector_y_pos_end -= ImGui::GetStyle().ScrollbarSize;


			//Draw the playhead
			if (pos >= start.x && pos <= end.x)
				ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(pos - 1, start.y), ImVec2(pos + 1, time_selector_y_pos_end), ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive)));

			if (childWindow != nullptr && was_timeline_clicked && pos < start.x)
			{
				ImGui::SetScrollX(childWindow, floorf(clamp(0, scroll_x - (start.x - pos)/10.f, scroll_x_max)));
			}

			if (childWindow != nullptr && was_timeline_clicked && pos + ImGui::GetStyle().ScrollbarSize > end.x)
			{
				ImGui::SetScrollX(childWindow, ceilf(clamp(0, scroll_x + (pos - end.x)/10.f, scroll_x_max)));
			}


			//ImGui::Separator();
			ImGui::Text(ICON_FK_ARROWS_V);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(window_size.x * 0.10f);
			ImGui::SliderFloat("##animation_track_width", &trackHeight, 1.f, 10.f, "");
			ImGui::SameLine();
			ImGui::Text(ICON_FK_SEARCH);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(window_size.x * 0.10f);
			ImGui::SliderFloat("##animation_timeline_zoom", &Zoom, 1.f, 10.f, "");
			ImGui::SameLine();
			ImGui::Text("Length (s):");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(window_size.x * 0.2f);
			ImGui::InputFloat("##animation_length", &animation.length, 0.f, 0.f, "%.2f");
			if (animation.length <= 0)
				animation.length = Step;
			ImGui::SameLine();
			ImGui::Text("Step (s):");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(window_size.x * 0.2f);
			ImGui::InputFloat("##animation_step", &Step, 0.f, 0.f, "%.2f");
			if (Step < 0.1f)
				Step = 0.1f;
			if (Step > animation.length)
				Step = animation.length;

			ImGui::PopStyleColor();

		}
		ImGui::End();



	}


}