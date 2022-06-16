#include "chromapch.h"
#include "Camera.h"

#include "Chroma/Reflection/Reflection.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Chroma
{

	void Camera::RecalculateViewMatrix()
	{
		float aspectRatio = (float)size.x / (float)size.y;
		float left = -(float)size.y * aspectRatio * .5f;
		float right = (float)size.y * aspectRatio * .5f;
		float bottom = -(float)size.y * .5f;
		float top = (float)size.y * .5f;
		projectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { position, 0.f });

		viewMatrix = glm::inverse(transform);

		//OpenGL order, reverse for DirectX, as it uses row major order instead
		viewProjectionMatrix = projectionMatrix * viewMatrix;
	}

	void Camera::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Size";
		out << YAML::Value << size;
	}

	void Camera::Deserialize(YAML::Node& node)
	{
		auto val = node["Size"];
		if (val)
		{
			size = val.as<Math::vec2>();
		}
	}


	void Camera::CreateReflectionModel()
	{
		Reflection::RegisterComponent<Camera>();
		Reflection::RegisterComponentProperty<Camera, &Camera::SetSize, &Camera::GetSize>("Size");
		Reflection::RegisterComponentProperty<Camera, &Camera::SetPosition, &Camera::GetPosition>("Position", false);
		Reflection::RegisterComponentProperty<Camera, nullptr, &Camera::GetProjectionMatrix>("ProjectionMatrix");
		Reflection::RegisterComponentProperty<Camera, nullptr, &Camera::GetViewMatrix>("ViewMatrix");
		Reflection::RegisterComponentProperty<Camera, nullptr, &Camera::GetViewProjectionMatrix>("ViewProjectionMatrix");

	}
}


