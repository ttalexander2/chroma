#include "chromapch.h"
#include "Camera.h"

#include "Chroma/Reflection/Reflection.h"
#include "Chroma/Renderer/RendererAPI.h"

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

	Reflection::TypeFactory<Camera> Camera::register_type()
	{
		auto factory =  Reflection::register_type<Camera>("Camera")
		       .base<Component>();
		
		       return factory.data<&Camera::GetSize, &Camera::SetSize>("Size")
		       .data<&Camera::GetPosition, &Camera::SetPosition>("Position")
		       .function<&Camera::GetProjectionMatrix>("GetProjectionMatrix")
		       .function<&Camera::GetViewMatrix>("GetViewMatrix")
		       .function<&Camera::GetViewProjectionMatrix>("GetViewProjectionMatrix");
	}
}
