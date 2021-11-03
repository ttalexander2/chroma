#include "LogWindow.h"
#include <imgui.h>
#include <Chroma/Core/Log.h>
#include "LogSink.h"
#include "AssetBrowser.h"
#include <Chroma/Math/Math.h>


namespace Polychrome
{
	bool LogWindow::Open = true;
	std::vector<LogWindow::LogMessage> LogWindow::Messages;

	void LogWindow::Init()
	{
		auto log = new LogSink<spdlog::details::null_mutex>();
		Chroma::Log::GetClientLogger()->sinks().push_back(spdlog::sink_ptr(log));
		spdlog::set_default_logger(Chroma::Log::GetClientLogger());
	}

	void LogWindow::Draw()
	{
		if (Open)
		{
			if (ImGui::Begin("Log Window", &Open))
			{
				ImGui::SetWindowSize({ 800, 400 }, ImGuiCond_FirstUseEver);
				if (ImGui::BeginTable("##error_log_table", 5, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY))
				{
					ImGui::TableSetupScrollFreeze(0, 1);
					ImGui::TableNextColumn();
					ImGui::TableHeader("Level");
					ImGui::TableNextColumn();
					ImGui::TableHeader("Time");
					ImGui::TableNextColumn();
					ImGui::TableHeader("Message");
					ImGui::TableNextColumn();
					ImGui::TableHeader("Source");
					ImGui::TableNextColumn();
					ImGui::TableHeader("Function");

					static int selected = -1;
					int i = 0;
					for (auto& msg : Messages)
					{
						bool is_selected = (selected == i);

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::PushID(&msg);
						ImGui::Selectable("##select", &is_selected, ImGuiSelectableFlags_SpanAllColumns);
						if (is_selected && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
							AssetBrowser::HandleOpen(msg.Filename);
						ImGui::SameLine();
						ImGui::PopID();
						ImGui::Text(ToString(msg.Level).c_str());
						ImGui::TableNextColumn();
						ImGui::Text(msg.Time.c_str());
						ImGui::TableNextColumn();
						ImGui::Text(msg.Message.c_str());
						ImGui::TableNextColumn();
						ImGui::Text(fmt::format("{}:{}", msg.Filename, msg.Line).c_str());
						ImGui::TableNextColumn();
						ImGui::Text(msg.Function.c_str());


						if (is_selected)
						{
							selected = i;
						}

						i++;
					}
					ImGui::EndTable();
				}
				
			}
			ImGui::End();
		}
	}
	const std::string LogWindow::ToString(ErrorLevel level)
	{
		switch (level)
		{
		case ErrorLevel::Trace: return "Trace";
		case ErrorLevel::Info: return "Info";
		case ErrorLevel::Warning: return "Warning";
		case ErrorLevel::Error: return "Error";
		case ErrorLevel::Critical: return "Critical";
		}
		return "Unknown";
	}
}