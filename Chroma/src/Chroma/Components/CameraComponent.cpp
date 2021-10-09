#include "chromapch.h"
#include "CameraComponent.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Chroma
{

	CameraComponent* CameraComponent::primaryCamera;

	CameraComponent::CameraComponent(float x, float y, float width, float height)
		: position(x,y), size(width, height),projectionMatrix(glm::ortho(-1 * width/ height * width, width / height * width, -height, height, -1.0f, 1.0f)), viewMatrix(1.0f)
	{
		viewProjectionMatrix = projectionMatrix * viewMatrix;
		SetPosition(x, y);
	}

	void CameraComponent::RecalculateViewMatrix()
	{
		float aspectRatio = (float)size.x / (float)size.y;
		float left = -(float)size.y * aspectRatio * .5f;
		float right = (float)size.y * aspectRatio * .5f;
		float bottom = -(float)size.y * .5f;
		float top = (float)size.y * .5f;
		projectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { position, 0.f }) *
			glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1));

		viewMatrix = glm::inverse(transform);

		//OpenGL order, reverse for DirectX, as it uses row major order instead
		viewProjectionMatrix = projectionMatrix * viewMatrix;
	}

	void CameraComponent::Serialize(YAML::Emitter& out)
	{
		out << YAML::Key << "Position";
		out << YAML::Value << position;

		out << YAML::Key << "Size";
		out << YAML::Value << size;

		if (this == primaryCamera)
		{
			out << YAML::Key << "Primary";
			out << YAML::Value << true;
		}



	}

	void CameraComponent::Deserialize(YAML::Node& node)
	{
		auto val = node["Position"];
		if (val)
		{
			position = val.as<Math::vec2>();
		}

		val = node["Size"];
		if (val)
		{
			size = val.as<Math::vec2>();
		}

		val = node["Primary"];
		if (val)
		{
			SetPrimaryCamera(this);
		}

	}

	void CameraComponent::SetPrimaryCamera(CameraComponent* camera)
	{
		CameraComponent::primaryCamera = camera;
	}

	CameraComponent& CameraComponent::GetPrimaryCamera()
	{
		return *CameraComponent::primaryCamera;
	}

	void CameraComponent::SetPrimary()
	{
		SetPrimaryCamera(this);
	}

	bool CameraComponent::IsPrimary()
	{
		return primaryCamera == this;
	}
}


