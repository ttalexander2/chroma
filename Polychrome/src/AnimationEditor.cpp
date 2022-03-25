#include "AnimationEditor.h"
#include <imgui.h>
#include <imgui_internal.h>
#include "Fonts/IconsForkAwesome.h"
#include <Chroma/Animation/Animation.h>
#include <string>
#include <ios>
#include <iomanip>
#include <sstream>
#include <Chroma/Core/Input.h>

namespace Polychrome
{

	bool AnimationEditor::Open = true;
	bool AnimationEditor::Playing = false;
	bool AnimationEditor::Snap = false;
	float AnimationEditor::Step = 0.5f;
	float AnimationEditor::TimeSelection = 1.f;
	float AnimationEditor::Zoom = 1.f;

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

	ImVec2 operator+ (ImVec2 v1, ImVec2 v2)
	{
		return ImVec2(v1.x + v2.x, v1.y + v2.y);
	}

	void AnimationEditor::Draw()
	{
		static bool created = false;
		static Chroma::Animation animation;
		if (!created)
		{
			animation.m_Length = 5.f;
			animation.m_LoopType = Chroma::LoopType::Loop;
			Chroma::Track track;
			track.m_Name = "Transform::Position::x";
			track.m_Update = Chroma::UpdateType::Continuous;
			animation.m_Tracks.push_back(track);
			created = true;
		}

		if (TimeSelection < 0)
		{
			TimeSelection = 0;
		}
		if (TimeSelection > animation.m_Length)
		{
			TimeSelection = animation.m_Length;
		}



		if (ImGui::Begin("Animation Editor##editor", &Open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{

			ImVec2 window_size = ImGui::GetContentRegionAvail();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
			ImGui::Button(ICON_FK_FAST_BACKWARD);
			ImGui::SameLine();
			ImGui::Button(ICON_FK_STEP_BACKWARD);
			ImGui::SameLine();
			if (Playing)
				ImGui::Button(ICON_FK_STOP);
			else
				ImGui::Button(ICON_FK_PLAY);
			ImGui::SameLine();
			ImGui::Button(ICON_FK_STEP_FORWARD);
			ImGui::SameLine();
			ImGui::Button(ICON_FK_FAST_FORWARD);


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
			//float num = animation.m_Length/Step;
			//float line_pos = start - 14;
			//while (curr_step < animation.m_Length)
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
			//	if (animation.m_Length * p != TimeSelection)
			//	{
			//		TimeSelection = animation.m_Length * p / Zoom;
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
			//float percent = TimeSelection / animation.m_Length * Zoom;
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
			ImGui::Dummy({1, ImGui::GetTextLineHeight()});
			//ImGui::Separator();

			//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, ImGui::GetStyle().ItemSpacing.y });

			if (ImGui::BeginChild("##track_window", ImVec2(track_width, available.y - 25.f - ImGui::GetStyle().ScrollbarSize - ImGui::GetTextLineHeightWithSpacing()), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
			{
				ImGui::Dummy({ 0.1f, 0.1f });
				for (int i = 0; i < 100; i++)
				{
					ImGui::Selectable(("Track Item " + std::to_string(i)).c_str(), false, ImGuiSelectableFlags_SpanAvailWidth);
				}

				ImGui::SetScrollY(scroll_y);

				ImGui::EndChild();
			}

			//ImGui::Separator();

			ImGui::EndGroup();

			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::Dummy({ 1, ImGui::GetTextLineHeight() });
			ImVec2 p = ImGui::GetCursorScreenPos();
			ImGui::InvisibleButton("##track_splitter", ImVec2(8.0f, available.y - 25.f - ImGui::GetStyle().ScrollbarSize - ImGui::GetTextLineHeight()));

			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
			}

			if (ImGui::IsItemHovered())
				ImGui::GetForegroundDrawList()->AddRectFilled(p + ImVec2(4, 0), ImVec2(p.x + 5, p.y + available.y - 25.f - ImGui::GetStyle().ScrollbarSize - ImGui::GetTextLineHeight()), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_SeparatorHovered)));
			else if (ImGui::IsItemActivated())
				ImGui::GetForegroundDrawList()->AddRectFilled(p + ImVec2(4, 0), ImVec2(p.x + 5, p.y + available.y - 25.f - ImGui::GetStyle().ScrollbarSize - ImGui::GetTextLineHeight()), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_SeparatorActive)));
			else
				ImGui::GetForegroundDrawList()->AddRectFilled(p + ImVec2(4, 0), ImVec2(p.x + 5, p.y + available.y - 25.f - ImGui::GetStyle().ScrollbarSize - ImGui::GetTextLineHeight()), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)));
			ImGui::EndGroup();
			if (ImGui::IsItemActive())
			{
				track_width += ImGui::GetIO().MouseDelta.x;
			}

			float timeline_width = available.x - track_width - keyframe_width;
			ImGui::SameLine();
			ImGui::BeginGroup();

			start = ImGui::GetCursorScreenPos();

			if (ImGui::BeginChild("##timeline", ImVec2(timeline_width - ImGui::GetStyle().ScrollbarSize, ImGui::GetTextLineHeightWithSpacing()), false, ImGuiWindowFlags_NoScrollbar))
			{
				float curr_step = Step;
				float num = animation.m_Length/Step;
				float line_pos = ImGui::GetCursorPos().x;

				ImGui::Dummy({ (timeline_width - ImGui::GetStyle().ScrollbarSize) * Zoom, 0.1f });

				while (curr_step < animation.m_Length)
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


			if (was_dragging)
			{
				ImGui::SetNextWindowScroll({scroll_x, scroll_y});
				was_dragging = false;
			}

			end = ImGui::GetCursorScreenPos() + ImVec2(timeline_width, available.y - 25.f - ImGui::GetTextLineHeightWithSpacing()) + ImVec2(-ImGui::GetStyle().ScrollbarSize, 0);

			if (ImGui::BeginChild("##timeline_keyframes", ImVec2(timeline_width, available.y - 25.f - ImGui::GetTextLineHeightWithSpacing()), false, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar))
			{

				ImGui::Dummy({(timeline_width - ImGui::GetStyle().ScrollbarSize) * Zoom, 0.1f });
				for (int i = 0; i < 100; i++)
				{
					ImGui::Selectable("Keyframe");
				}

				scroll_y = ImGui::GetScrollY();
				scroll_x = ImGui::GetScrollX();
				scroll_y_max = ImGui::GetScrollMaxY();
				scroll_x_max = ImGui::GetScrollMaxX();
				ImGui::EndChild();
			}

			ImGui::EndGroup();

			//ImGui::PopStyleVar();

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
				ImGui::GetForegroundDrawList()->AddRectFilled(p + ImVec2(4, 0), ImVec2(p.x + 5, p.y + available.y - 25.f - ImGui::GetStyle().ScrollbarSize - ImGui::GetTextLineHeight()), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_SeparatorHovered)));
			else if (ImGui::IsItemActivated())
				ImGui::GetForegroundDrawList()->AddRectFilled(p + ImVec2(4, 0), ImVec2(p.x + 5, p.y + available.y - 25.f - ImGui::GetStyle().ScrollbarSize - ImGui::GetTextLineHeight()), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_SeparatorActive)));
			else
				ImGui::GetForegroundDrawList()->AddRectFilled(p + ImVec2(4, 0), ImVec2(p.x + 5, p.y + available.y - 25.f - ImGui::GetStyle().ScrollbarSize - ImGui::GetTextLineHeight()), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Separator)));
			
			ImGui::EndGroup();

			if (ImGui::IsItemActive())
			{
				keyframe_width -= ImGui::GetIO().MouseDelta.x;
			}

			ImGui::SameLine();

			if (ImGui::BeginTabBar("##keyframe_transition_tab"))
			{

				if (ImGui::BeginTabItem("Keyframe"))
				{


					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Transition"))
				{

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}


			float percent = TimeSelection / animation.m_Length * Zoom;
			float length = timeline_width;
			float scroll_offset = 0.f;
			if (scroll_x_max > 0.f)
				scroll_offset = scroll_x / scroll_x_max;
			float pos = start.x + (timeline_width -  ImGui::GetStyle().ScrollbarSize) * percent - scroll_offset * (timeline_width - ImGui::GetStyle().ScrollbarSize) * percent;

			CHROMA_CORE_INFO("scroll_x: {}, scroll_x_max: {}", scroll_x, scroll_x_max);

			if (pos >= start.x || pos <= end.x )
				ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(pos - 1, start.y), ImVec2(pos + 1, end.y), ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive)));


			ImGui::Separator();
			ImGui::Text(ICON_FK_SEARCH);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(window_size.x * 0.15f);
			ImGui::SliderFloat("##animation_timeline_zoom", &Zoom, 1.f, 10.f, "");
			ImGui::SameLine();
			ImGui::Text("Length (s):");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(window_size.x * 0.2f);
			ImGui::InputFloat("##animation_lenght", &animation.m_Length, 0.f, 0.f, "%.2f");
			if (animation.m_Length <= 0)
				animation.m_Length = Step;
			ImGui::SameLine();
			ImGui::Text("Step (s):");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(window_size.x * 0.2f);
			ImGui::InputFloat("##animation_step", &Step, 0.f, 0.f, "%.2f");
			if (Step < 0.1f)
				Step = 0.1f;
			if (Step > animation.m_Length)
				Step = animation.m_Length;

			ImGui::PopStyleColor();

		}
		ImGui::End();



	}


}