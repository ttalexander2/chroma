#include "GameSettings.h"
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>
#include <Chroma/Scene/World.h>
#include <Chroma/ImGui/Widgets/CollapsibleGroupPannel.h>
#include <Chroma/ImGui/IconsForkAwesome.h>
#include <Chroma/Utilities/ContainerHelpers.h>
#include <unordered_map>
#include "EditorApp.h"
#include <regex>
//#include <Chroma/Utilities/CSharpIdentifier.h>


namespace Polychrome
{
	bool GameSettings::Open;
	static bool layerListOpen = true;

    static std::unordered_map<ImGuiID, int> active;
    static std::unordered_map<ImGuiID, int> hovered;
    static std::unordered_map<ImGuiID, int> clicked;
    static std::unordered_map<ImGuiID, int> editing;


    static bool EditableList(std::vector<Chroma::Layer>& list, bool open)
    {

        ImGui::PushID(&list);
        ImGuiID id = ImGui::GetID(&list);


        if (active.size() > 100)
            active.clear();
        if (hovered.size() > 100)
            hovered.clear();
        if (clicked.size() > 100)
            clicked.clear();
        if (editing.size() > 100)
            editing.clear();

        if (open)
        {
            if (!active.contains(id))
                active.emplace(id, -1);
            if (!hovered.contains(id))
                hovered.emplace(id, -1);
            if (!clicked.contains(id))
                clicked.emplace(id, 0);
            if (!editing.contains(id))
                editing.emplace(id, -1);
        }


        if (ImGui::BeginCollapsibleGroupPanel(id, ImGui::GetContentRegionAvail(), open))
        {
            bool any_hovered = false;
            bool mouseReleased = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
            for (int i = 0; i < list.size(); i++)
            {
                ImGui::PushID(i);
                std::string item = list[i].Name;

                ImGui::AlignTextToFramePadding();

                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

                ImVec4 col = ImGui::GetStyleColorVec4(ImGuiCol_Header);
                //ImGui::PushStyleColor(ImGuiCol_Header, { col.x, col.y, col.z, 0.8f });
                if (hovered[id] == i)
                    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { col.x, col.y, col.z, 0.7f });
                ImVec4 selectableColor = ImGui::GetStyleColorVec4(ImGuiCol_PopupBg);
                //ImGui::PushStyleColor(ImGuiCol_PopupBg, { selectableColor.x, selectableColor.y, selectableColor.z, selectableColor.w * 0.0f });

                bool is_selected = (i == clicked[id]);
                ImGui::Selectable((std::string("##selectable") + std::to_string(i)).c_str(), &is_selected, ImGuiSelectableFlags_AllowItemOverlap, { ImGui::GetContentRegionAvail().x - 3, 0 });

                if (ImGui::IsItemClicked(ImGuiMouseButton_Right) && list[i].Name != "Default")
                    ImGui::OpenPopup("rename_item");

                //ImGui::SetNextWindowPos(ImGui::GetWindowSize())
                ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
                if (ImGui::BeginPopup("rename_item", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_Modal))
                {
                    bool valid = true;
                    ImGui::InputText("##name_item", &list[i].Name);
                   
                    //list[i].Name.erase(std::remove_if(list[i].Name.begin(), list[i].Name.end(), [](const char& c) {return !std::isalnum(c); }));

                    if (list[i].Name.empty())
                        valid = false;


                    //valid = valid && Chroma::CSharpIdentifier::IsValidIdentifier(list[i].Name);

                    if (!valid)
                    {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                    }

                    if (ImGui::Button("Rename") && valid)
                        ImGui::CloseCurrentPopup();

                    if (ImGui::IsKeyPressed((int)Chroma::Input::Key::ENTER) && valid)
                        ImGui::CloseCurrentPopup();

                    if (!valid)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }
                    
                    ImGui::EndPopup();
                }

                //ImGui::PopStyleColor();

                if (ImGui::IsItemClicked())
                {
                    clicked[id] = i;
                }

                if (hovered[id] == i)
                    ImGui::PopStyleColor();
                //ImGui::PopStyleColor();


                if (active[id] == -1 && ImGui::IsItemActive())
                {
                    active[id] = i;
                }

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
                {
                    hovered[id] = i;
                    any_hovered = true;
                }

                ImGui::SameLine(-1);
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

                //if (is_selected)
                //    ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0,0,0,0 });

                if (hovered[id] == i && any_hovered)
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0,0,0,0 });

                ImGui::AlignTextToFramePadding();
                ImGui::Text(ICON_FK_BARS);

                ImGui::SameLine(24.0f);
                //ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                std::string input_hash = std::string("##input_text") + std::to_string(i);

                //ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, ImGui::GetStyleColorVec4(ImGuiCol_BorderShadow));

                bool pushed = false;
                if (list[i].Name == "Default")
                {
                    pushed = true;
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                }

                ImGui::Text(list[i].Name.c_str());

                
                ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - 100);
                ImGui::Text("Global: ");
                ImGui::SameLine();
                ImGui::Checkbox("##global", &list[i].Global);


                if (pushed)
                {
                    ImGui::PopItemFlag();
                    ImGui::PopStyleVar();
                }

                //ImGui::PopStyleColor();

                //if (is_selected)
                //    ImGui::PopStyleColor();
                
                if (hovered[id] == i && any_hovered)
                    ImGui::PopStyleColor();


                ImGui::PopID();


            }

        }


        if (open)
        {
            if (active[id] != -1 && hovered[id] != -1 && hovered[id] != active[id])
            {
                std::string item = list[active[id]].Name;
                int n_next = active[id] + (ImGui::GetMouseDragDelta(0).y < 0.1f ? -1 : 1);
                if (n_next >= 0 && n_next < list.size())
                {
                    list[active[id]] = list[n_next];
                    list[n_next] = item;
                    ImGui::ResetMouseDragDelta();
                    active[id] = n_next;
                    clicked[id] = n_next;
                }
            }

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                active[id] = -1;
            }
        }



        ImGui::EndCollapsibleGroupPanel();

        if (open)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0,0 });
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
            ImVec4 c = ImGui::GetStyleColorVec4(ImGuiCol_Border);
            ImGui::PushStyleColor(ImGuiCol_Button, { c.x, c.y, c.z, c.w / 2.5f });

            ImGui::Dummy({ 1.0f, 0.0f });
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 50.0f);


            if (ImGui::Button(ICON_FK_PLUS))
            {
                static int num = list.size();
                list.push_back(Chroma::Layer("New Layer " + std::to_string(num++), list.size()));
            }
            ImGui::SameLine();

            if (clicked[id] != -1 && list[clicked[id]].Name == "Default")
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }

            bool erased = false;

            if (ImGui::Button(ICON_FK_MINUS))
            {
                if (clicked[id] != -1)
                {
                    erased = true;
                    std::erase_if(list, [&](const Chroma::Layer& layer) { return layer == list[clicked[id]]; });
                }
                   
                if (clicked[id] > static_cast<int>(list.size()) - 1)
                    clicked[id] = static_cast<int>(list.size()) - 1;
            }

            if (clicked[id] != -1 && !erased && list[clicked[id]].Name == "Default")
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }


            ImGui::PopStyleColor();
            ImGui::PopStyleVar(2);
        }

        ImGui::PopID();

        for (int i = 0; i < list.size(); i++)
        {
            list[i].Order = i;
        }

        return open;
    }
	
	void GameSettings::Draw()
	{
		if (!Open)
			return;

		if (ImGui::Begin("Game Settings##game_settins", &Open))
		{
            std::vector<Chroma::Layer> layers;
            for (auto& layer : Chroma::World::Layers)
            {
                layers.push_back(layer);
            }
            for (auto& layer : EditorApp::CurrentScene->Layers)
            {
                layers.push_back(layer);
            }



            std::sort(layers.begin(), layers.end());
            
            if (EditableList(layers, layerListOpen))
            {
                
            }
            Chroma::World::Layers.clear();
            EditorApp::CurrentScene->Layers.clear();
            for (auto& layer : layers)
            {
                if (layer.Global)
                    Chroma::World::Layers.push_back(layer);
                else
                    EditorApp::CurrentScene->Layers.push_back(layer);
            }
		}
		ImGui::End();
		
	}
}