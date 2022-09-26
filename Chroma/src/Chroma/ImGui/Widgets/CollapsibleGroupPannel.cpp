#include "chromapch.h"
#include "CollapsibleGroupPannel.h"


#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "Chroma/ImGui/IconsForkAwesome.h"
#include <Chroma/ImGui/ImGuiDebugMenu.h>
#include <Chroma/ImGui/ImGuiHelper.h>

namespace ImGui
{
	static ImVector<ImRect> s_GroupPanelLabelStack;
	static std::unordered_map<ImGuiID, ImVec2> minV;
	static std::unordered_map<ImGuiID, ImVec2> maxV;
	static bool isOpen;
	static ImGuiID ID;

	bool ImGui::BeginCollapsibleGroupPanel(ImGuiID id, const ImVec2 &size, bool open)
	{
		ID = id;

		if (minV.size() > 100)
			minV.clear();
		if (maxV.size() > 100)
			maxV.clear();

		isOpen = open;
		//ImGui::BeginGroup();

		ImVec2 pos = GetCursorPos();
		SetCursorPos({ pos.x, pos.y - 7.0f });

		auto cursorPos = GetCursorScreenPos();
		auto itemSpacing = GetStyle().ItemSpacing;
		PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
		PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		auto frameHeight = GetFrameHeight();
		BeginGroup();

		ImVec2 effectiveSize = size;
		if (size.x < 0.0f)
			effectiveSize.x = GetContentRegionAvailWidth();
		else
			effectiveSize.x = size.x;
		Dummy(ImVec2(effectiveSize.x, 0.0f));

		Dummy(ImVec2(frameHeight * 0.5f, 0.0f));

		SameLine(0.0f, 0.0f);
		BeginGroup();
		Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
		SameLine(0.0f, 0.0f);

		ImVec2 halfFrame = ImVec2(frameHeight * 0.25f, frameHeight) * 0.5f;

		if (!minV.contains(id))
			minV.emplace(id, ImVec2(0, 0));
		if (!maxV.contains(id))
			maxV.emplace(id, ImVec2(0, 0));

		if (open)
			GetWindowDrawList()->AddRectFilled(
					minV[id],
					maxV[id],
					ImColor({ 30, 30, 30, 256 }),
					halfFrame.x);

		PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.0f, 0.0f));

		auto labelMin = GetItemRectMin();
		auto labelMax = GetItemRectMax();
		SameLine(0.0f, 0.0f);
		Dummy(ImVec2(0.0, frameHeight + itemSpacing.y));
		BeginGroup();

		//ImGui::GetWindowDrawList()->AddRect(labelMin, labelMax, IM_COL32(255, 0, 255, 255));

#if IMGUI_VERSION_NUM >= 17301
		GetCurrentWindow()->ContentRegionRect.Max.x -= frameHeight * 0.5f;
		GetCurrentWindow()->WorkRect.Max.x -= frameHeight * 0.5f;
		GetCurrentWindow()->InnerRect.Max.x -= frameHeight * 0.5f;
#else
        ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x -= frameHeight * 0.5f;
#endif
		GetCurrentWindow()->Size.x -= frameHeight;

		auto itemWidth = CalcItemWidth();
		PushItemWidth(ImMax(0.0f, itemWidth - frameHeight));

		s_GroupPanelLabelStack.push_back(ImRect(labelMin, labelMax));

		PopStyleVar(3);

		if (open)
			Dummy({ 0.0f, 5.0f });

		return open;
	}

	void ImGui::EndCollapsibleGroupPanel()
	{
		PopItemWidth();

		PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
		PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5.0f, 0.0f));

		auto itemSpacing = GetStyle().ItemSpacing;

		auto frameHeight = GetFrameHeight();

		EndGroup();

		EndGroup();

		SameLine(0.0f, 0.0f);
		Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
		Dummy(ImVec2(0.0, frameHeight - frameHeight * 0.5f - itemSpacing.y));

		EndGroup();

		auto itemMin = GetItemRectMin();
		auto itemMax = GetItemRectMax();
		//ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 0, 0, 64), 4.0f);

		auto labelRect = s_GroupPanelLabelStack.back();
		s_GroupPanelLabelStack.pop_back();

		ImVec2 halfFrame = ImVec2(frameHeight * 0.25f, frameHeight) * 0.5f;
		auto frameRect = ImRect(itemMin + halfFrame, itemMax - ImVec2(halfFrame.x, 0.0f));
		labelRect.Min.x -= itemSpacing.x;
		labelRect.Max.x += itemSpacing.x;
		for (int i = 0; i < 4; ++i)
		{
			switch (i)
			{
				// left half-plane
				case 0:
					PushClipRect(ImVec2(-FLT_MAX, -FLT_MAX), ImVec2(labelRect.Min.x, FLT_MAX), true);
					break;
				// right half-plane
				case 1:
					PushClipRect(ImVec2(labelRect.Max.x, -FLT_MAX), ImVec2(FLT_MAX, FLT_MAX), true);
					break;
				// top
				case 2:
					PushClipRect(ImVec2(labelRect.Min.x, -FLT_MAX), ImVec2(labelRect.Max.x, labelRect.Min.y), true);
					break;
				// bottom
				case 3:
					PushClipRect(ImVec2(labelRect.Min.x, labelRect.Max.y), ImVec2(labelRect.Max.x, FLT_MAX), true);
					break;
			}

			if (isOpen)
				GetWindowDrawList()->AddRect(
						frameRect.Min,
						frameRect.Max,
						ImColor(GetStyleColorVec4(ImGuiCol_Border)),
						halfFrame.x);

			PopClipRect();
		}

		minV[ID] = frameRect.Min;
		maxV[ID] = frameRect.Max;

		PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
		GetCurrentWindow()->ContentRegionRect.Max.x += frameHeight * 0.5f;
		GetCurrentWindow()->WorkRect.Max.x += frameHeight * 0.5f;
		GetCurrentWindow()->InnerRect.Max.x += frameHeight * 0.5f;
#else
        ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x += frameHeight * 0.5f;
#endif
		GetCurrentWindow()->Size.x += frameHeight;

		//ImGui::Dummy(ImVec2(0.0f, 0.0f));

		//ImGui::EndGroup();
	}
}
