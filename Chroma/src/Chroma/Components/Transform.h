#pragma once

#include "Chroma/Scene/Component.h"
#include "Chroma/Math/Mat4x4.h"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui.h"


namespace Chroma
{
	struct Transform : public Component
	{
		Math::vec3 Position = { 0.0f, 0.0f, 0.0f };
		Math::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		Math::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		Transform() = default;
		Transform(const Transform&) = default;
		Transform(const Math::mat4& transform)
		{
		}

		Math::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Position.y, { 1, 0, 0 })
				* glm::rotate(glm::mat4(1.0f), Rotation.y, { 0, 1, 0 })
				* glm::rotate(glm::mat4(1.0f), Rotation.z, { 0, 0, 1 });

			return Math::translate(glm::mat4(1.0f), Position)
				* rotation * glm::scale(glm::mat4(1.0f), Scale);
		}

		const std::string Name() const override
		{
			return "Transform";
		}

		void DrawImGui() override
		{

			if (ImGui::BeginTable("transform_split1", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX))
			{
				ImGui::TableNextColumn();

				ImGui::AlignTextToFramePadding(); ImGui::Text("Position");
				ImGui::AlignTextToFramePadding(); ImGui::Text("Rotation");
				ImGui::AlignTextToFramePadding(); ImGui::Text("Scale");

				ImGui::TableNextColumn();

				float pos[3] = { Position.x, Position.y, Position.z };
				if (ImGui::InputFloat3("##transform_position", pos))
				{
					Position = { pos[0], pos[1], pos[2] };
				}
				float rot[3] = { Rotation.x, Rotation.y, Rotation.z };
				if (ImGui::InputFloat3("##transform_rotation", rot))
				{
					Rotation = { rot[0], rot[1], rot[2] };
				}
				float scl[3] = { Scale.x, Scale.y, Scale.z };
				if (ImGui::InputFloat3("##transform_scale", scl))
				{
					Scale = { scl[0], scl[1], scl[2] };
				}
				ImGui::EndTable();
			}
		}

	private:
		const bool IsTransform() const override
		{
			return true;
		}
		

	};
}


