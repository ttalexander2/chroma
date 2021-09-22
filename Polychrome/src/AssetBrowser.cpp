#include "AssetBrowser.h"
#include <imgui.h>
#include <imgui_internal.h>
#include "Fonts/IconsForkAwesome.h"
#include <string>
#include <imgui_stdlib.h>
#include <filesystem>
#include <Chroma/Assets/AssetManager.h>
#include "EditorApp.h"
#include "FuzzyFileSearch.h"
#include "CodeEditor.h"

namespace Polychrome
{
	bool AssetBrowser::Open = true;


	std::string search_term;
	bool show_hidden = false;
	std::filesystem::path active_dir;
	std::filesystem::path AssetBrowser::Selected;
	bool asset_folder_open = true;
	float icon_size = 40;

	void AssetBrowser::HandleOpen(std::filesystem::path path, TextEditor::Coordinates cursor_pos)
	{
		if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
			return;
		
		TextEditor::LanguageDefinition definition;
		if (path.extension() == std::filesystem::path(".cpp") || path.extension() == std::filesystem::path(".h") || path.extension() == std::filesystem::path(".hpp"))
		{
			definition = TextEditor::LanguageDefinition::CPlusPlus();
		}
		else if (path.extension() == std::filesystem::path(".lua"))
		{
			definition = TextEditor::LanguageDefinition::Lua();
		}
		else if (path.extension() == std::filesystem::path(".glsl"))
		{
			definition = TextEditor::LanguageDefinition::GLSL();
		}
		else if (path.extension() == std::filesystem::path(".hlsl"))
		{
			definition = TextEditor::LanguageDefinition::HLSL();
		}
		else
		{
			return;
		}

		for (auto& a : CodeEditor::Instances)
		{
			if (a.Path == path)
			{
				return;
			}
				
		}

		CodeEditor::Instance instance(path);
		instance.Definition = definition;
		if (instance.Path.extension() == std::filesystem::path(".lua"))
		{
			TextEditor::Identifier id;
			id.mDeclaration = "Current Entity";
			instance.Definition.mPreprocIdentifiers.insert(std::make_pair("entity", id));
		}

		CodeEditor::Current = path;
		CodeEditor::Editor.SetText(instance.Text);
		CodeEditor::Editor.SetLanguageDefinition(instance.Definition);
		CodeEditor::Open = true;
		instance.Coordinates = cursor_pos;
		CodeEditor::Instances.push_back(instance);

	}

	void AssetBrowser::Draw()
	{
		if (active_dir.empty() || !std::filesystem::exists(active_dir))
			active_dir = Chroma::AssetManager::AssetDirectory;

		ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
		if (ImGui::Begin("Asset Browser##editor", &Open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImGui::Button((std::string(ICON_FK_PLUS_SQUARE) + " " + std::string(ICON_FK_ANGLE_DOWN)).c_str());
			ImGui::SameLine(ImGui::GetWindowWidth() - 290);
			ImGui::Text(ICON_FK_SEARCH);
			ImGui::SameLine();

			ImGui::PushItemWidth(200);
			ImGui::InputText("##asset_browser_search_bar", &search_term);
			bool searching = !search_term.empty();

			std::vector<std::filesystem::path> search_paths;
			if (searching)
				search_paths = FuzzyFileSearch::Search(search_term, Chroma::AssetManager::AssetDirectory);

			ImGui::SameLine();
			if (ImGui::Button(ICON_FK_TIMES_CIRCLE))
				search_term.clear();

			ImGui::PopItemWidth();
			ImGui::SameLine();

			if (show_hidden)
			{
				if (ImGui::Button(ICON_FK_EYE)) show_hidden = false;
			}
			else
			{
				if (ImGui::Button(ICON_FK_EYE_SLASH)) show_hidden = true;
			}

			ImGui::Separator();

			if (ImGui::BeginTable("##asset_browser_columns", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp, ImGui::GetContentRegionAvail()))
			{
				ImGui::TableNextColumn();

				//TREE VIEW
				if (ImGui::BeginChild("##asset_browser_tree", ImVec2(0,0), false, ImGuiWindowFlags_NoBackground))
				{

					std::string icon = asset_folder_open ? ICON_FK_FOLDER_OPEN : ICON_FK_FOLDER;

					auto flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
					if (Chroma::AssetManager::AssetDirectory == active_dir)
						flags |= ImGuiTreeNodeFlags_Selected;

	
					if (ImGui::TreeNodeEx((icon + " " + std::filesystem::path(Chroma::AssetManager::AssetDirectory).filename().string()).c_str(), flags))
					{
						asset_folder_open = ImGui::TreeNodeBehaviorIsOpen(ImGui::GetActiveID());
						if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
							active_dir = Chroma::AssetManager::AssetDirectory;
						//ImGui::Indent();
						ParseFolder(Chroma::AssetManager::AssetDirectory);
						//ImGui::Unindent();
						ImGui::TreePop();
					}
					else
					{
						asset_folder_open = ImGui::TreeNodeBehaviorIsOpen(ImGui::GetActiveID());
						if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
							active_dir = Chroma::AssetManager::AssetDirectory;
					}
					
					ImGui::EndChild();
				}



				ImGui::TableNextColumn();

				std::filesystem::path path_build = std::filesystem::path(Chroma::AssetManager::AssetDirectory).parent_path();
				for (auto& part : std::filesystem::relative(active_dir, std::filesystem::path(Chroma::AssetManager::AssetDirectory).parent_path()))
				{
					if (part == ".")
						continue;
					path_build = path_build / part;
					if (ImGui::Button(part.string().c_str()))
					{
						active_dir = path_build;
						break;
					}	
					ImGui::SameLine();
					if (ImGui::Button((std::string(ICON_FK_ANGLE_RIGHT) + "##" + path_build.string()).c_str()))
					{
						ImGui::OpenPopup(("##asset_browser_select_from_arrow" + path_build.string()).c_str());
					}
					bool changed = false;
					if (ImGui::BeginPopup(("##asset_browser_select_from_arrow" + path_build.string()).c_str()))
					{
						for (auto& child : std::filesystem::directory_iterator(path_build))
						{
							if (child.is_directory() && child.exists())
							{
								if (ImGui::MenuItem(child.path().filename().string().c_str()))
								{
									active_dir = child.path();
									changed = true;
								}
							}
						}
						ImGui::EndPopup();
					}
					ImGui::SameLine();
					if (changed)
						break;
				}
				ImGui::NewLine();
				if (ImGui::BeginChild("##asset_browser_view", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - ImGui::CalcTextSize("Oh").y - ImGui::GetStyle().FramePadding.y * 2)))
				{
					ImFont f = *EditorApp::LargeIcons;
					f.Scale = icon_size / f.FontSize;
					//f->Scale = icon_size;


					//ICON VIEW
					if (icon_size > 40)
					{
						auto& style = ImGui::GetStyle();
						float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

						
						if (!searching)
						{

							//RENDER_FOLDERS
							for (auto& dir : std::filesystem::directory_iterator(active_dir))
							{
								bool is_selected = (Selected == dir.path());
								if (dir.is_directory())
								{
									ImGui::PushID(dir.path().string().c_str());
									ImGui::SetNextItemWidth(icon_size);
									ImGui::BeginGroup();
									ImGui::PushFont(&f);
									if (is_selected)
										ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
									if (ImGui::Button(ICON_FK_FOLDER_O, { icon_size, icon_size }))
									{
										Selected = dir.path();
									}

									if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
									{
										active_dir = dir.path();
									}

									if (is_selected)
										ImGui::PopStyleColor();
									ImGui::PopFont();
									ImVec2 text_size = ImGui::CalcTextSize(dir.path().filename().string().c_str());
									if (text_size.x < icon_size && icon_size - text_size.x > 0)
									{
										ImGui::Dummy({ 0,0 });
										ImGui::SameLine((icon_size - text_size.x) / 2.f);
										ImGui::AlignTextToFramePadding();
										ImGui::Text(dir.path().filename().string().c_str());
									}
									else
									{
										std::string a = dir.path().filename().string();
										int chars = (int)icon_size/(text_size.x / a.length()) - 2;
										ImGui::AlignTextToFramePadding();
										ImGui::Text((" " + a.substr(0, chars) + "...").c_str());
									}

									ImGui::EndGroup();
									float last_button_x2 = ImGui::GetItemRectMax().x;
									float next_button_x2 = last_button_x2 + style.ItemSpacing.x + icon_size; // Expected position if next button was on same line
									if (next_button_x2 < window_visible_x2)
										ImGui::SameLine();

									ImGui::PopID();
								}
							
							}

							//RENDER FILES
							for (auto& dir : std::filesystem::directory_iterator(active_dir))
							{
								bool is_selected = (Selected == dir.path());
								if (dir.is_regular_file())
								{
									ImGui::PushID(dir.path().string().c_str());
									ImGui::SetNextItemWidth(icon_size);
									ImGui::BeginGroup();
									ImGui::PushFont(&f);
									if (is_selected)
										ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
									if (ImGui::Button(ICON_FK_FILE, { icon_size, icon_size }))
									{
										Selected = dir.path();
									}
									if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
									{
										HandleOpen(dir.path());
										if (Selected == dir.path())
											Selected.clear();
									}

									if (is_selected)
										ImGui::PopStyleColor();
									ImGui::PopFont();
									ImVec2 text_size = ImGui::CalcTextSize(dir.path().filename().string().c_str());
									if (text_size.x < icon_size && icon_size - text_size.x > 0)
									{
										ImGui::Dummy({ 0,0 });
										ImGui::SameLine((icon_size - text_size.x) / 2.f);
										ImGui::AlignTextToFramePadding();
										ImGui::Text(dir.path().filename().string().c_str());
									}
									else
									{
										std::string a = dir.path().filename().string();
										int chars = (int)icon_size / (text_size.x / a.length()) - 2;
										ImGui::AlignTextToFramePadding();
										ImGui::Text((" " + a.substr(0, chars) + "...").c_str());
									}

									ImGui::EndGroup();
									float last_button_x2 = ImGui::GetItemRectMax().x;
									float next_button_x2 = last_button_x2 + style.ItemSpacing.x + icon_size; // Expected position if next button was on same line
									if (next_button_x2 < window_visible_x2)
										ImGui::SameLine();

									ImGui::PopID();
								}

							}

						}
						else
						{
							//Display Search results
							for (auto& dir : search_paths)
							{
								bool is_selected = (Selected == dir);

								ImGui::PushID(dir.string().c_str());
								ImGui::SetNextItemWidth(icon_size);
								ImGui::BeginGroup();
								ImGui::PushFont(&f);
								if (is_selected)
									ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
								if (ImGui::Button(ICON_FK_FILE, { icon_size, icon_size }))
								{
									Selected = dir;
								}
								if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
								{
									HandleOpen(dir);
									if (Selected == dir)
										Selected.clear();
								}

								if (is_selected)
									ImGui::PopStyleColor();
								ImGui::PopFont();
								ImVec2 text_size = ImGui::CalcTextSize(dir.filename().string().c_str());
								if (text_size.x < icon_size && icon_size - text_size.x > 0)
								{
									ImGui::Dummy({ 0,0 });
									ImGui::SameLine((icon_size - text_size.x) / 2.f);
									ImGui::AlignTextToFramePadding();
									ImGui::Text(dir.filename().string().c_str());
								}
								else
								{
									std::string a = dir.filename().string();
									int chars = (int)icon_size / (text_size.x / a.length()) - 2;
									ImGui::AlignTextToFramePadding();
									ImGui::Text((" " + a.substr(0, chars) + "...").c_str());
								}

								ImGui::EndGroup();
								float last_button_x2 = ImGui::GetItemRectMax().x;
								float next_button_x2 = last_button_x2 + style.ItemSpacing.x + icon_size; // Expected position if next button was on same line
								if (next_button_x2 < window_visible_x2)
									ImGui::SameLine();

								ImGui::PopID();


							}
						}

						if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							Selected = "";
						}
					}
					else 
					{
					auto& style = ImGui::GetStyle();
					float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;


					if (!searching)
					{

						//RENDER_FOLDERS
						for (auto& dir : std::filesystem::directory_iterator(active_dir))
						{
							bool is_selected = (Selected == dir.path());
							if (dir.is_directory())
							{
								ImGui::Dummy({ 3, 1 });
								ImGui::SameLine();
								if (ImGui::Selectable((ICON_FK_FOLDER + std::string("  ") + dir.path().filename().string()).c_str(), &is_selected))
								{
									Selected = dir.path();
								}
								if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
								{
									active_dir = dir.path();
								}

							}

						}

						//RENDER FILES
						for (auto& dir : std::filesystem::directory_iterator(active_dir))
						{
							bool is_selected = (Selected == dir.path());
							if (dir.is_regular_file())
							{
								ImGui::Dummy({ 3, 1 });
								ImGui::SameLine();
								if (ImGui::Selectable((ICON_FK_FILE + std::string("  ") + dir.path().filename().string()).c_str(), &is_selected))
								{
									Selected = dir.path();
								}
								if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
								{
									HandleOpen(dir.path());
									if (Selected == dir.path())
										Selected.clear();
								}
							}


						}

					}
					else
					{
						//Display Search results
						for (auto& dir : search_paths)
						{
							bool is_selected = (Selected == dir);
							ImGui::Dummy({ 3, 1 });
							ImGui::SameLine();
							if (ImGui::Selectable((ICON_FK_FILE + std::string("  ") +  dir.filename().string()).c_str(), &is_selected))
							{
								Selected = dir;
							}
							if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
							{
								HandleOpen(dir);
								if (Selected == dir)
									Selected.clear();
							}
						}
					}

					if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						Selected = "";
					}
					}

					ImGui::EndChild();
				}
				std::string path_text = Selected.string();
				ImGui::Text(path_text.c_str());
				ImGui::SameLine(ImGui::GetColumnWidth() - 120);
				ImGui::SetNextItemWidth(100);
				ImGui::SliderFloat("##asset_browser_view_size", &icon_size, 40, 150, "");
				ImGui::EndTable();
			}
			


			

		}
		ImGui::End();

		ImGui::PopStyleColor();

		
	}

	void AssetBrowser::ParseFolder(std::filesystem::path path)
	{
		for (auto const& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
			{
				bool has_children = false;
				for (auto const& child : std::filesystem::directory_iterator(entry))
				{
					if (child.is_directory())
						has_children = true;
				}

				auto flags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
				static bool open = false;
				std::string icon = open ? ICON_FK_FOLDER_OPEN : ICON_FK_FOLDER;
				if (!has_children)
					icon = ICON_FK_FOLDER_O;

				if (entry.path() == active_dir)
					flags |= ImGuiTreeNodeFlags_Selected;
				if (open = ImGui::TreeNodeEx((icon + " " + entry.path().filename().string()).c_str(), has_children ? flags | ImGuiTreeNodeFlags_None : flags | ImGuiTreeNodeFlags_Leaf))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
						active_dir = entry.path();

					if (has_children)
					{
						//ImGui::Indent();
						ParseFolder(entry);
						//ImGui::Unindent();
					}
					ImGui::TreePop();
				}
				else
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
						active_dir = entry.path();
				}
				
			}
		}
	}
}