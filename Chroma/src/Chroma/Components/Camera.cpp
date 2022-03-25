#include "chromapch.h"
#include "Camera.h"

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
		entt::meta<Camera>()
			.data<&Camera::SetPosition, &Camera::GetPosition>("Position"_hs)
			.data<&Camera::SetSize, &Camera::GetSize>("Size"_hs)
			.data<nullptr, &Camera::GetProjectionMatrix>("ProjectionMatrix"_hs)
			.data<nullptr, &Camera::GetViewMatrix>("ViewMatrix"_hs)
			.data<nullptr, &Camera::GetViewProjectionMatrix>("ViewProjectionMatrix"_hs)
			.type("Camera"_hs);

	}
}


