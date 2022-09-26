#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <set>
#include <string>

// imgui combo filter v1.0, by @r-lyeh (public domain)
// contains code by @harold-b (public domain?)

/*  Demo: */
/*
    {
        // requisite: hints must be alphabetically sorted beforehand
        const char *hints[] = {
            "AnimGraphNode_CopyBone",
            "ce skipaa",
            "ce skipscreen",
            "ce skipsplash",
            "ce skipsplashscreen",
            "client_unit.cpp",
            "letrograd",
            "level",
            "leveler",
            "MacroCallback.cpp",
            "Miskatonic university",
            "MockAI.h",
            "MockGameplayTasks.h",
            "MovieSceneColorTrack.cpp",
            "r.maxfps",
            "r.maxsteadyfps",
            "reboot",
            "rescale",
            "reset",
            "resource",
            "restart",
            "retrocomputer",
            "retrograd",
            "return",
            "slomo 10",
            "SVisualLoggerLogsList.h",
            "The Black Knight",
        };
        static ComboFilterState s = {0};
        static char buf[128] = "type text here...";
        if( ComboFilter("my combofilter", buf, IM_ARRAYSIZE(buf), hints, IM_ARRAYSIZE(hints), s) ) {
            puts( buf );
        }
    }
*/

#pragma once

struct ComboFilterState
{
	int activeIdx; // Index of currently 'active' item by use of up/down keys
	bool selectionChanged; // Flag to help focus the correct item when selecting active item
};

static bool ComboFilter__DrawPopup(ComboFilterState &state, int START, std::vector<std::string> ENTRIES, int ENTRY_COUNT, const char *unique_id)
{
	using namespace ImGui;
	bool clicked = false;

	static int val;

	// Grab the position for the popup
	ImVec2 pos = GetItemRectMin();
	pos.y += GetItemRectSize().y;
	auto size = ImVec2(GetItemRectSize().x - 60, GetTextLineHeightWithSpacing() * 4);

	PushStyleVar(ImGuiStyleVar_WindowRounding, 0);

	ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_HorizontalScrollbar |
			ImGuiWindowFlags_NoSavedSettings |
			0; //ImGuiWindowFlags_ShowBorders;

	//SetNextWindowFocus();

	SetNextWindowPos(pos);
	SetNextWindowSize(size);
	Begin(("##COMBO_FILTER_" + std::string(unique_id)).c_str(), nullptr, flags);

	//PushAllowKeyboardFocus(false);

	for (int i = 0; i < ENTRY_COUNT; i++)
	{
		// Track if we're drawing the active index so we
		// can scroll to it if it has changed
		bool isIndexActive = state.activeIdx == i;

		if (isIndexActive)
		{
			// Draw the currently 'active' item differently
			// ( used appropriate colors for your own style )
			PushStyleColor(ImGuiCol_Border, ImVec4(1, 1, 0, 1));
		}

		PushID(i);
		if (Selectable((ENTRIES[i] + "##COMBO_FILTER_" + unique_id).c_str(), isIndexActive))
		{
			// And item was clicked, notify the input
			// callback so that it can modify the input buffer
			state.activeIdx = i;
			clicked = true;
		}
		if (IsItemFocused() && IsKeyPressed(GetIO().KeyMap[ImGuiKey_Enter]))
		{
			// Allow ENTER key to select current highlighted item (w/ keyboard navigation)
			state.activeIdx = i;
			clicked = true;
		}
		PopID();

		if (isIndexActive)
		{
			if (state.selectionChanged)
			{
				// Make sure we bring the currently 'active' item into view.
				SetScrollHere();
				state.selectionChanged = false;
			}

			PopStyleColor(1);
		}
	}

	//PopAllowKeyboardFocus();
	End();
	PopStyleVar(1);

	return clicked;
}

static bool ComboFilter(const char *id, char *buffer, int bufferlen, std::vector<std::string> hints, int num_hints, ComboFilterState &s)
{
	struct fuzzy
	{
		static int score(const char *str1, const char *str2)
		{
			int score = 0, consecutive = 0, maxerrors = 0;
			while (*str1 && *str2)
			{
				int is_leading = (*str1 & 64) && !(str1[1] & 64);
				if ((*str1 & ~32) == (*str2 & ~32))
				{
					int had_separator = (str1[-1] <= 32);
					int x = had_separator || is_leading ? 10 : consecutive * 5;
					consecutive = 1;
					score += x;
					++str2;
				}
				else
				{
					int x = -1, y = is_leading * -3;
					consecutive = 0;
					score += x;
					maxerrors += y;
				}
				++str1;
			}
			return score + (maxerrors < -9 ? -9 : maxerrors);
		}

		static int search(const char *str, int num, std::vector<std::string> words)
		{
			int scoremax = 0;
			int best = -1;
			for (int i = 0; i < num; ++i)
			{
				int score = fuzzy::score(words[i].c_str(), str);
				int record = (score >= scoremax);
				int draw = (score == scoremax);
				if (record)
				{
					scoremax = score;
					if (!draw)
						best = i;
					else
						best = best >= 0 && strlen(words[best].c_str()) < strlen(words[i].c_str()) ? best : i;
				}
			}
			return best;
		}
	};
	using namespace ImGui;
	bool done = InputText((std::string(id)).c_str(), buffer, bufferlen, ImGuiInputTextFlags_EnterReturnsTrue);
	bool hot = s.activeIdx >= 0 && strcmp(buffer, hints[s.activeIdx].c_str());
	if (hot)
	{
		int new_idx = fuzzy::search(buffer, num_hints, hints);
		int idx = new_idx >= 0 ? new_idx : s.activeIdx;
		s.selectionChanged = s.activeIdx != idx;
		s.activeIdx = idx;
		if (done || ComboFilter__DrawPopup(s, idx, hints, num_hints, id))
		{
			int i = s.activeIdx;
			if (i >= 0)
			{
				strcpy(buffer, hints[i].c_str());
				done = true;
			}
		}
	}
	return done;
}
