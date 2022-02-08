#include "AssetBrowser.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include <imgui_stdlib.h>
#include <filesystem>
#include <Chroma/Assets/AssetManager.h>


#include "Project.h"
#include "EditorApp.h"
#include "FuzzyFileSearch.h"
#include "Fonts/IconsForkAwesome.h"


#if _WIN32
#include <shellapi.h>
#include <shlobj.h>
#include <combaseapi.h>
#include <commoncontrols.h>
#include <shellapi.h>
#include "../GLFW/include/GLFW/glfw3.h"
#endif

namespace Polychrome
{
	bool AssetBrowser::Open = true;


	std::string search_term;
	bool show_hidden = false;
	std::filesystem::path active_dir;
	std::filesystem::path AssetBrowser::Selected;
	std::filesystem::path context_item;
	std::map<std::filesystem::path, Chroma::Ref<Chroma::Texture2D>> AssetBrowser::Icons;
	bool asset_folder_open = true;
	float icon_size = 80;
	std::string last_search;

	static std::string tree_folder_editing_path;
	static std::filesystem::path tree_folder_editing;

	static std::string file_editing_path;
	static std::filesystem::path file_editing;

	void AssetBrowser::HandleOpen(std::filesystem::path path)
	{
		if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
			return;

		

	}

	void AssetBrowser::Draw()
	{
		if (active_dir.empty() || !std::filesystem::exists(active_dir))
			active_dir = Project::AssetDirectory;

			for (std::filesystem::path path : std::filesystem::recursive_directory_iterator(Project::AssetDirectory))
			{
				if (!Icons.contains(path))
				{
					LoadFileIcon(path);
				}
			}


			ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
			if (ImGui::Begin("Asset Browser##editor", &Open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
			{


				if (ImGui::Button((std::string(ICON_FK_PLUS_SQUARE) + " " + std::string(ICON_FK_ANGLE_DOWN)).c_str()))
				{
					ImGui::OpenPopup("##Asset_Browser_Create");
				}

				if (ImGui::BeginPopup("##Asset_Browser_Create"))
				{
					AssetBrowserCreatePopup();
					ImGui::EndPopup();
				}


				ImGui::SameLine(ImGui::GetWindowWidth() - 290);
				ImGui::Text(ICON_FK_SEARCH);
				ImGui::SameLine();

				ImGui::PushItemWidth(200);
				ImGui::InputText("##asset_browser_search_bar", &search_term);
				bool searching = !search_term.empty();

				static std::vector<std::filesystem::path> search_paths;
				if (searching && last_search != search_term)
				{
					search_paths = FuzzyFileSearch::Search(search_term, Project::AssetDirectory);
					last_search = search_term;
				}
				

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
						if (Project::AssetDirectory == active_dir)
							flags |= ImGuiTreeNodeFlags_Selected;

	
						if (ImGui::TreeNodeEx((icon + " " + std::filesystem::path(Project::AssetDirectory).filename().string()).c_str(), flags))
						{
							asset_folder_open = ImGui::TreeNodeBehaviorIsOpen(ImGui::GetActiveID());
							if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
								active_dir = Project::AssetDirectory;
							//ImGui::Indent();
							bool changed = ParseFolder(Project::AssetDirectory);
							if (changed)
							{
								ImGui::TreePop();
								ImGui::EndChild();
								ImGui::EndTable();
								ImGui::End();
								ImGui::PopStyleColor();
								return;


							}
							//ImGui::Unindent();
							ImGui::TreePop();
						}
						else
						{
							asset_folder_open = ImGui::TreeNodeBehaviorIsOpen(ImGui::GetActiveID());
							if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
								active_dir = Project::AssetDirectory;
						}
					
						ImGui::EndChild();
					}



					ImGui::TableNextColumn();

					std::filesystem::path path_build = std::filesystem::path(Project::AssetDirectory).parent_path();
					for (auto& part : std::filesystem::relative(active_dir, std::filesystem::path(Project::AssetDirectory).parent_path()))
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

										if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
										{
											context_item = dir.path();
											ImGui::OpenPopup("##File_context_menu7");
										}

										if (context_item == dir.path() && ImGui::BeginPopup("##File_context_menu7"))
										{
											FolderClickPopup();
											ImGui::EndPopup();
										}

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
										if (!Icons.contains(dir.path()))
										{
											LoadFileIcon(dir.path());
										}
										ImGui::PushID(dir.path().string().c_str());
										ImGui::SetNextItemWidth(icon_size);
										ImGui::BeginGroup();
										ImGui::PushFont(&f);
										if (is_selected)
											ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
									
										if (Icons.contains(dir.path()))
										{
											auto& texture = Icons.find(dir.path())->second;
										
											if (ImGui::ImageButton(reinterpret_cast<void*>(texture->GetTextureID()), { icon_size, icon_size }))
											{
												Selected = dir.path();
											}
										}
										else
										{
											if (ImGui::Button(ICON_FK_FILE, { icon_size, icon_size }))
											{
												Selected = dir.path();
											}
										}

										if (is_selected)
											ImGui::PopStyleColor();
										ImGui::PopFont();
									

										if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
										{
											HandleOpen(dir.path());
											if (Selected == dir.path())
												Selected.clear();
										}

										if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
										{
											context_item = dir.path();
											ImGui::OpenPopup("##File_context_menu1");
										}

										if (context_item == dir.path() && ImGui::BeginPopup("##File_context_menu1"))
										{
											if (ImGui::Selectable("Rename"))
											{
												file_editing = dir.path();
												file_editing_path = dir.path().filename().replace_extension("").string();
											}
											FileClickPopup();
											ImGui::EndPopup();
										}
										
										bool is_editing = file_editing == dir.path();

										bool valid = file_editing.parent_path().string() + "/" + file_editing_path + file_editing.extension().string() == dir.path() || (!std::filesystem::exists(file_editing.parent_path().string() + "/" + file_editing_path) && !file_editing_path.empty());


										ImVec2 text_size = ImGui::CalcTextSize(dir.path().filename().string().c_str());

										if (is_editing && !file_editing.empty() && file_editing == dir.path())
										{
											//ImGui::Dummy({ 0,0 });
											//ImGui::SameLine((icon_size - text_size.x) / 2.f);
											ImGui::AlignTextToFramePadding();

											if (!valid)
												ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 0.f, 0.f, 1.f });

											ImGui::SetNextItemWidth(icon_size - ImGui::CalcTextSize(dir.path().extension().string().c_str()).x);

											if (valid && ImGui::InputText("##editing_file", &file_editing_path, ImGuiInputTextFlags_EnterReturnsTrue))
											{
												//std::filesystem::rename(file_editing, file_editing.parent_path().string() + "/" + file_editing_path + dir.path().extension().string());
												//file_editing.clear();
												//file_editing_path.clear();
											}

											if (ImGui::IsItemDeactivatedAfterEdit())
											{
												if (valid)
												{
													std::filesystem::rename(file_editing, file_editing.parent_path().string() + "/" + file_editing_path + dir.path().extension().string());
													file_editing.clear();
													file_editing_path.clear();
												}
												file_editing.clear();
												file_editing_path.clear();
											}

											if (!valid)
												ImGui::PopStyleColor();

											ImGui::SameLine();
											ImGui::Text(file_editing.extension().string().c_str());
										}
										else
										{
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

											if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
											{
												file_editing = dir.path();
												file_editing_path = dir.path().filename().string();
											}

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

									if (Icons.contains(dir))
									{
										auto& texture = Icons.find(dir)->second;

										if (ImGui::ImageButton(reinterpret_cast<void*>(texture->GetTextureID()), { icon_size, icon_size }))
										{
											Selected = dir;
										}
									}
									else
									{
										if (ImGui::Button(ICON_FK_FILE, { icon_size, icon_size }))
										{
											Selected = dir;
										}
									}
									if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
									{
										HandleOpen(dir);
										if (Selected == dir)
											Selected.clear();
									}

									if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
									{
										context_item = dir;
										ImGui::OpenPopup("##File_context_menu");
									}

									if (context_item == dir && ImGui::BeginPopup("##File_context_menu"))
									{
										FileClickPopup();
										ImGui::EndPopup();
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

									if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
									{
										context_item = dir.path();
										ImGui::OpenPopup("##File_context_menu5");

									}
									if (context_item == dir.path() && ImGui::BeginPopup("##File_context_menu5"))
									{
										FolderClickPopup();
										ImGui::EndPopup();
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

									if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
									{
										context_item = dir.path();
										ImGui::OpenPopup("##File_context_menu2");

									}

									if (context_item == dir.path() && ImGui::BeginPopup("##File_context_menu2"))
									{
										FileClickPopup();
										ImGui::EndPopup();
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

								if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
								{
									context_item = dir;
									ImGui::OpenPopup("##File_context_menu3");
								}
								if (context_item == dir && ImGui::BeginPopup("##File_context_menu3"))
								{
									FileClickPopup();
									ImGui::EndPopup();
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

	void AssetBrowser::FileClickPopup()
	{
#ifdef _WIN32
		if (ImGui::Selectable("Show in File Explorer.."))
		{
			std::string directory_to_open = std::filesystem::absolute(context_item).string();
			system(("explorer /select," + directory_to_open).c_str());
		}
#endif
		if (ImGui::Selectable("Open in VSCode.."))
		{
			system(("code " + std::filesystem::path(active_dir).parent_path().parent_path().string() + " " + context_item.string()).c_str());
			context_item.clear();
		}
	}

	void AssetBrowser::FolderClickPopup()
	{
#ifdef _WIN32
		if (ImGui::Selectable("Open in File Explorer.."))
		{
			std::string directory_to_open = std::filesystem::absolute(context_item).string();
			system(("explorer " + directory_to_open).c_str());
		}
#endif
	}

	void AssetBrowser::LoadFileIcon(std::filesystem::path path)
	{
		std::wstring w = path.wstring();

		if (path.extension().compare(".png") == 0 || path.extension().compare(".jpg") == 0 || path.extension().compare(".ase") == 0 || path.extension().compare(".aseprite") == 0)
		{
			std::string sprite_path = path.lexically_relative(Project::AssetDirectory).string();

			if (Chroma::AssetManager::Exists(sprite_path))
			{
				auto sprite = Chroma::AssetManager::Get<Chroma::Sprite>(sprite_path);

				if (!sprite->Frames.empty())
				{
					Icons[path] = sprite->Frames[0].Texture;
					return;
				}
			}



		}

#if _WIN32

		//SHGetFileInfoW(ucPath, FILE_ATTRIBUTE_NORMAL, &info, sizeof(info), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_LARGEICON);
		//HICON 
		//WORD index = 0;
		//HICON icon = ExtractAssociatedIconW(GetModuleHandleW(NULL), LPWSTR(w.c_str()), &index);
		// Get the image list index of the icon
		SHFILEINFO sfi;
		memset(&sfi, 0, sizeof(sfi));
		if (!SHGetFileInfo(w.c_str(), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX)) return;

		// Get the jumbo image list
		IImageList* piml;
		if (FAILED(SHGetImageList(SHIL_JUMBO, IID_PPV_ARGS(&piml)))) return;

		// Extract an icon
		HICON icon;
		piml->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &icon);

		// Clean up
		piml->Release();

		if (icon == NULL)
		{
			return;
		}

		
		BITMAP bm;
		memset(&bm, 0, sizeof(BITMAP));
		ICONINFO iconInfo;
		GetIconInfo(icon, &iconInfo);
		GetObjectW(iconInfo.hbmColor, sizeof(BITMAP), &bm);


		int width = bm.bmWidth;
		int height = bm.bmHeight;
		int bytesPerScanLine = (width * 3 + 3) & 0xFFFFFFFC;
		int size = bytesPerScanLine * height;
		BITMAPINFO infoheader;
		memset(&infoheader, 0, sizeof(BITMAPINFO));
		infoheader.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		infoheader.bmiHeader.biWidth = width;
		infoheader.bmiHeader.biHeight = height;
		infoheader.bmiHeader.biPlanes = 1;
		infoheader.bmiHeader.biBitCount = 24;
		infoheader.bmiHeader.biCompression = BI_RGB;
		infoheader.bmiHeader.biSizeImage = size; // allocate Memory for Icon RGB data plus Icon mask plus ARGB buffer for the resulting image 
		
		std::vector<uint8_t> pMemBlock = std::vector<uint8_t>((size * 2 + height * width * 4));
		uint8_t* pixelsIconRGB = (uint8_t*)&pMemBlock[0];
		uint8_t* alphaPixels = (uint8_t*)(pixelsIconRGB + size);
		uint32_t* imagePixels = (uint32_t*)(alphaPixels + size);
		HDC hDC = CreateCompatibleDC(NULL); // Get Icon RGB data
		HBITMAP hBmpOld = (HBITMAP)SelectObject(hDC, (HGDIOBJ)iconInfo.hbmColor);
		if (!GetDIBits(hDC, iconInfo.hbmColor, 0, height, (LPVOID)pixelsIconRGB, &infoheader, DIB_RGB_COLORS)) return;
		SelectObject(hDC, hBmpOld);
		// now get the mask
		if (!GetDIBits(hDC, iconInfo.hbmMask, 0, height, (LPVOID)alphaPixels, &infoheader, DIB_RGB_COLORS)) return;

		int x = 0;
		int currentSrcPos = 0;
		int currentDestPos = 0;
		int linePosSrc = 0;
		int linePosDest = 0;
		int lsSrc = width * 3;
		int vsDest = height - 1;
		for (int y = 0; y < height; y++)
		{
			linePosSrc = (vsDest - y) * lsSrc;
			linePosDest = y * width;
			for (x = 0; x < width; x++)
			{
				//pixels from Icon are stored in BGR vertical and horizontal flipped order 
				currentDestPos = linePosDest + x;
				currentSrcPos = linePosSrc + x * 3;
				// BGR -> ARGB
				uint8_t alpha = 255;
				if (pixelsIconRGB[currentSrcPos] == 0 && pixelsIconRGB[currentSrcPos + 1] == 0 && pixelsIconRGB[currentSrcPos + 2] == 0)
					alpha = 0;
				imagePixels[currentDestPos] = (((uint32_t)((((pixelsIconRGB[currentSrcPos + 2] << 0x10 /*Red*/) | (pixelsIconRGB[currentSrcPos + 1] << 8 /*Green*/)) | pixelsIconRGB[currentSrcPos] /*Blue*/) | (alpha << 0x18))) & 0xffffffffL);
			}
		}

		auto icon_texture = Chroma::Texture2D::Create(width, height);
		icon_texture->SetData((void*)(uint8_t*)imagePixels, height * width * sizeof(uint32_t));
		Icons[path] = icon_texture;

#endif
	}

	void AssetBrowser::AssetBrowserCreatePopup()
	{
		if (ImGui::Selectable("New Folder"))
		{
			if (!std::filesystem::exists(active_dir.string() + "/New Folder"))
			{
				try
				{
					std::filesystem::create_directory(active_dir.string() + "/New Folder");
				}
				catch (const std::filesystem::filesystem_error& e)
				{
					CHROMA_CORE_ERROR("{}", e.what());
				}
			}
			else
			{
				int x = 1;
				while (std::filesystem::exists(active_dir.string() + "/New Folder (" + std::to_string(x) + ")"))
				{
					x++;
				}
				std::filesystem::create_directory(active_dir.string() + "/New Folder (" + std::to_string(x) + ")");
			}
				
		}
		if (ImGui::Selectable("New C# Script"))
		{

		}

	}

	bool AssetBrowser::ParseFolder(std::filesystem::path path)
	{

		for (auto const& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
			{
				bool is_editing = tree_folder_editing  == entry.path();
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


				if (!is_editing && (open = ImGui::TreeNodeEx((icon + " " + entry.path().filename().string()).c_str(), has_children ? flags | ImGuiTreeNodeFlags_None : flags | ImGuiTreeNodeFlags_Leaf)))
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
						active_dir = entry.path();

					if (has_children)
					{
						//ImGui::Indent();
						bool changed = ParseFolder(entry);
						//ImGui::Unindent();
					}
					ImGui::TreePop();
				}
				else if (!is_editing)
				{
					if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
						active_dir = entry.path();
				}
				else
				{
					ImGui::Indent();
					ImGui::Text(icon.c_str());
					ImGui::SameLine();

					bool valid = true;
					if (std::filesystem::exists(entry.path().parent_path().append(tree_folder_editing_path)) && entry.path().parent_path().append(tree_folder_editing_path) != tree_folder_editing)
						valid = false;

					if (!valid)
						ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 0.f, 0.f, 1.f });

					ImGui::SetKeyboardFocusHere();
					if (ImGui::InputText("##editing_asset_folder_tree_view", &tree_folder_editing_path, ImGuiInputTextFlags_EnterReturnsTrue) && valid)
					{
						if (!std::filesystem::exists(entry.path().parent_path().append(tree_folder_editing_path)))
						{
							std::filesystem::rename(entry.path(), entry.path().parent_path().append(tree_folder_editing_path));
							tree_folder_editing_path.clear();
							tree_folder_editing.clear();
							return true;
						}
						
					}

					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						if (valid)
						{
							std::filesystem::rename(entry.path(), entry.path().parent_path().append(tree_folder_editing_path));
							tree_folder_editing_path.clear();
							tree_folder_editing.clear();
							return true;
						}
						tree_folder_editing_path.clear();
						tree_folder_editing.clear();
					}

					if (!valid)
						ImGui::PopStyleColor();

					ImGui::Unindent();
				}

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup(("##ASSET_BROWSER_TREE_VIEW_CONTEXT" + entry.path().string()).c_str());
				}

				if (ImGui::BeginPopup(("##ASSET_BROWSER_TREE_VIEW_CONTEXT" + entry.path().string()).c_str()))
				{
					if (ImGui::MenuItem("Rename"))
					{
						tree_folder_editing = entry.path();
						tree_folder_editing_path = entry.path().filename().string();
					}
					FolderClickPopup();
					ImGui::EndPopup();
				}
				
			}
		}
		return false;
	}
}