#include "chromapch.h"
#include "Camera.h"

#include "Chroma/Reflection/Reflection.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Chroma
{
	void Camera::RecalculateViewMatrix()
	{
		float aspectRatio = static_cast<float>(size.x) / static_cast<float>(size.y);
		float left = -static_cast<float>(size.y) * aspectRatio * .5f;
		float right = static_cast<float>(size.y) * aspectRatio * .5f;
		float bottom = -static_cast<float>(size.y) * .5f;
		float top = static_cast<float>(size.y) * .5f;
		projectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);

		glm::mat4 transform = translate(glm::mat4(1.0f), { position, 0.f });

		viewMatrix = inverse(transform);

		//OpenGL order, reverse for DirectX, as it uses row major order instead
		viewProjectionMatrix = projectionMatrix * viewMatrix;
	}

	Reflection::TypeFactory<Camera> Camera::RegisterType()
	{
		return Reflection::Register<Camera>("Camera")
		       .Base<Component>()
		       .Data<&Camera::SetSize, &Camera::GetSize>("Size")
		       .Data<&Camera::SetPosition, &Camera::GetPosition>("Position", false)
		       .Func<&Camera::GetProjectionMatrix>("GetProjectionMatrix")
		       .Func<&Camera::GetViewMatrix>("ViewMatrix")
		       .Func<&Camera::GetViewProjectionMatrix>("ViewProjectionMatrix");
	}
}
