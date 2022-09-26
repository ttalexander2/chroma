#include "chromapch.h"
#include "ScriptEngineRegistry.h"

#include <Chroma/Scene/Entity.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include "Chroma/Components/AudioSource.h"
#include "Chroma/Components/CircleCollider.h"
#include "Chroma/Components/EdgeCollider.h"
#include "Chroma/Components/PolygonCollider.h"
#include "Chroma/Components/RectangleCollider.h"
#include "Chroma/Components/SpriteRenderer.h"
#include "Chroma/Components/RigidBody.h"
#include "Chroma/Components/Transform.h"
#include "Chroma/Components/Tag.h"
#include "Chroma/Components/Camera.h"
#include "Chroma/Components/ParticleEmitter.h"
#include <Chroma/Scripting/ScriptWrappers.h>

#include "MonoScripting.cpp"

namespace Chroma
{
	std::unordered_map<MonoType *, std::function<bool(Entity &)>> hasComponentFuncs;
	std::unordered_map<MonoType *, std::function<void(Entity &)>> createComponentFuncs;
	std::unordered_map<MonoType *, std::function<Component*(Entity &)>> getComponentFuncs;

#define Component_RegisterType(Type) \
	{\
		MonoType* type = mono_reflection_type_from_name((char*)"Chroma." #Type, coreAssemblyImage);\
		if (type) {\
			uint32_t id = mono_type_get_type(type);\
			hasComponentFuncs[type] = [](Entity& entity) { return entity.HasComponent<Type>();  }; \
			createComponentFuncs[type] = [](Entity& entity) { return entity.AddComponent<Type>(); }; \
			getComponentFuncs[type] = [](Entity& entity) { return entity.GetScene().Registry.try_get<Type>(entity.GetID());  };\
		} else {\
			CHROMA_CORE_ERROR("No C# component class found for " #Type "!");\
		}\
	}

	static void InitComponentTypes()
	{
		Component_RegisterType(Transform);
		Component_RegisterType(AudioSource);
		Component_RegisterType(CircleCollider);
		Component_RegisterType(EdgeCollider);
		Component_RegisterType(PolygonCollider);
		Component_RegisterType(RectangleCollider);
		Component_RegisterType(SpriteRenderer);
		Component_RegisterType(RigidBody)
		Component_RegisterType(Tag);
		Component_RegisterType(Camera);
		Component_RegisterType(ParticleEmitter);
	}

	void ScriptEngineRegistry::RegisterAll()
	{
		InitComponentTypes();

		//Entity
		mono_add_internal_call("Chroma.Entity::CreateComponent_Native", Script::Entity_CreateComponent);
		mono_add_internal_call("Chroma.Entity::HasComponent_Native", Script::Entity_HasComponent);
		mono_add_internal_call("Chroma.Entity::FindEntityByName_Native", Script::Entity_FindEntityByName);
		mono_add_internal_call("Chroma.Entity::GetChildren_Native", Script::Entity_GetChildrenNative);
		mono_add_internal_call("Chroma.Entity::GetChildByName_Native", Script::Entity_GetChildByNameNative);
		mono_add_internal_call("Chroma.Entity::GetFirstChild_Native", Script::Entity_GetFirstChildNative);
		mono_add_internal_call("Chroma.Entity::HasChildren_Native", Script::Entity_HasChildrenNative);
		mono_add_internal_call("Chroma.Entity::NumChildren_Native", Script::Entity_NumChildrenNative);
		mono_add_internal_call("Chroma.Entity::GetAbsolutePosition_Native", Script::Entity_GetAbsolutePositionNative);
		mono_add_internal_call("Chroma.Entity::SetAbsolutePosition_Native", Script::Entity_SetAbsolutePositionNative);

		//Component
		mono_add_internal_call("Chroma.Component::GetEnabled_Native", Script::Component_GetEnabled);
		mono_add_internal_call("Chroma.Component::SetEnabled_Native", Script::Component_SetEnabled);

		//Log
		mono_add_internal_call("Chroma.Log::LogMessage_Native", Script::Log_Message);

		//Tag
		mono_add_internal_call("Chroma.Tag::GetTag_Native", Script::TagComponent_GetTag);
		mono_add_internal_call("Chroma.Tag::SetTag_Native", Script::TagComponent_SetTag);

		//Transform
		mono_add_internal_call("Chroma.Transform::GetTransformPosition_Native", Script::TransformComponent_GetPosition);
		mono_add_internal_call("Chroma.Transform::GetTransformRotation_Native", Script::TransformComponent_GetRotation);
		mono_add_internal_call("Chroma.Transform::GetTransformScale_Native", Script::TransformComponent_GetScale);
		mono_add_internal_call("Chroma.Transform::SetTransformPosition_Native", Script::TransformComponent_SetPosition);
		mono_add_internal_call("Chroma.Transform::SetTransformRotation_Native", Script::TransformComponent_SetRotation);
		mono_add_internal_call("Chroma.Transform::SetTransformScale_Native", Script::TransformComponent_SetScale);

		//Input
		mono_add_internal_call("Chroma.Input::IsKeyPressed_Native", Input::IsKeyPressed);
		mono_add_internal_call("Chroma.Input::IsMouseButtonPressed_Native", Input::IsMouseButtonPressed);
		mono_add_internal_call("Chroma.Input::IsJoystickConnected_Native", Input::IsJoystickConnected);
		mono_add_internal_call("Chroma.Input::GetMousePosition_Native", Script::Input_GetMousePosition);
		mono_add_internal_call("Chroma.Input::GetFirstConnectedGamepad_Native", Input::GetFirstConnectedGamepad);
		mono_add_internal_call("Chroma.Input::GetAllConnectedControllers", Script::Input_GetAllConnectedControllers);
		mono_add_internal_call("Chroma.Input::SetPrimaryGamepad_Native", Input::SetPrimaryGamepad);
		mono_add_internal_call("Chroma.Input::GetGamepadAxis_Native", Script::Input_GetGamepadAxis);
		mono_add_internal_call("Chroma.Input::GetGamepadButtonPressed_Native", Script::Input_GetGamepadButtonState);
		mono_add_internal_call("Chroma.Input::GetGamepadConnectionState_Native", Input::GetGamepadConnectionState);

		//Sprite Renderer
		mono_add_internal_call("Chroma.SpriteRenderer::GetColor_Native", Script::SpriteRenderer_GetColor);
		mono_add_internal_call("Chroma.SpriteRenderer::SetColor_Native", Script::SpriteRenderer_SetColor);
		mono_add_internal_call("Chroma.SpriteRenderer::GetOffset_Native", Script::SpriteRenderer_GetOffset);
		mono_add_internal_call("Chroma.SpriteRenderer::SetOffset_Native", Script::SpriteRenderer_SetOffset);
		mono_add_internal_call("Chroma.SpriteRenderer::GetLayer_Native", Script::SpriteRenderer_GetLayer);
		mono_add_internal_call("Chroma.SpriteRenderer::SetLayer_Native", Script::SpriteRenderer_SetLayer);
		mono_add_internal_call("Chroma.SpriteRenderer::GetPlayOnStart_Native", Script::SpriteRenderer_GetPlayOnStart);
		mono_add_internal_call("Chroma.SpriteRenderer::SetPlayOnStart_Native", Script::SpriteRenderer_SetPlayOnStart);
		mono_add_internal_call("Chroma.SpriteRenderer::GetPlaying_Native", Script::SpriteRenderer_GetPlaying);
		mono_add_internal_call("Chroma.SpriteRenderer::SetPlaying_Native", Script::SpriteRenderer_SetPlaying);
		mono_add_internal_call("Chroma.SpriteRenderer::GetLoop_Native", Script::SpriteRenderer_GetLoop);
		mono_add_internal_call("Chroma.SpriteRenderer::SetLoop_Native", Script::SpriteRenderer_SetLoop);
		mono_add_internal_call("Chroma.SpriteRenderer::GetSpeedMultiplier_Native", Script::SpriteRenderer_GetSpeedMultiplier);
		mono_add_internal_call("Chroma.SpriteRenderer::SetSpeedMultiplier_Native", Script::SpriteRenderer_SetSpeedMultiplier);
		mono_add_internal_call("Chroma.SpriteRenderer::GetSprite_Native", Script::SpriteRenderer_GetSprite);
		mono_add_internal_call("Chroma.SpriteRenderer::SetSprite_Native", Script::SpriteRenderer_SetSprite);
		mono_add_internal_call("Chroma.SpriteRenderer::GetAnimation_Native", Script::SpriteRenderer_GetAnimation);
		mono_add_internal_call("Chroma.SpriteRenderer::SetAnimation_Native", Script::SpriteRenderer_SetAnimation);
		mono_add_internal_call("Chroma.SpriteRenderer::GetFrame_Native", Script::SpriteRenderer_GetFrame);
		mono_add_internal_call("Chroma.SpriteRenderer::SetFrame_Native", Script::SpriteRenderer_SetFrame);
		mono_add_internal_call("Chroma.SpriteRenderer::GetAnimationTag_Native", Script::SpriteRenderer_GetAnimationTag);
		mono_add_internal_call("Chroma.SpriteRenderer::SetAnimationByTag_Native", Script::SpriteRenderer_SetAnimationByTag);
		mono_add_internal_call("Chroma.SpriteRenderer::RestartAnimation_Native", Script::SpriteRenderer_RestartAnimation);

		//Audio
		mono_add_internal_call("Chroma.Audio::PlayEvent_Native", Script::Audio_PlayEvent);
		mono_add_internal_call("Chroma.Audio::PlayEventIfStopped_Native", Script::Audio_PlayEventIfStopped);
		mono_add_internal_call("Chroma.Audio::StopEvent_Native", Script::Audio_StopEvent);
		mono_add_internal_call("Chroma.Audio::GetEventParameter_Native", Script::Audio_GetEventParameter);
		mono_add_internal_call("Chroma.Audio::SetEventParameter_Native", Script::Audio_SetEventParameter);
		mono_add_internal_call("Chroma.Audio::IsEventPlaying_Native", Script::Audio_IsEventPlaying);

		//RigidBody
		mono_add_internal_call("Chroma.RigidBody::GetBodyType_Native", Script::RigidBody_GetBodyType);
		mono_add_internal_call("Chroma.RigidBody::SetBodyType_Native", Script::RigidBody_SetBodyType);
		mono_add_internal_call("Chroma.RigidBody::GetLinearVelocity_Native", Script::RigidBody_GetLinearVelocity);
		mono_add_internal_call("Chroma.RigidBody::SetLinearVelocity_Native", Script::RigidBody_SetLinearVelocity);
		mono_add_internal_call("Chroma.RigidBody::GetAngularVelocity_Native", Script::RigidBody_GetAngularVelocity);
		mono_add_internal_call("Chroma.RigidBody::SetAngularVelocity_Native", Script::RigidBody_SetAngularVelocity);
		mono_add_internal_call("Chroma.RigidBody::GetLinearDamping_Native", Script::RigidBody_GetLinearDamping);
		mono_add_internal_call("Chroma.RigidBody::SetLinearDamping_Native", Script::RigidBody_SetLinearDamping);
		mono_add_internal_call("Chroma.RigidBody::GetAngularDamping_Native", Script::RigidBody_GetAngularDamping);
		mono_add_internal_call("Chroma.RigidBody::SetAngularDamping_Native", Script::RigidBody_SetAngularDamping);
		mono_add_internal_call("Chroma.RigidBody::GetSleepingAllowed_Native", Script::RigidBody_GetSleepingAllowed);
		mono_add_internal_call("Chroma.RigidBody::SetSleepingAllowed_Native", Script::RigidBody_SetSleepingAllowed);
		mono_add_internal_call("Chroma.RigidBody::SetAwake_Native", Script::RigidBody_SetAwake);
		mono_add_internal_call("Chroma.RigidBody::GetAwake_Native", Script::RigidBody_GetAwake);
		mono_add_internal_call("Chroma.RigidBody::SetFixedRotation_Native", Script::RigidBody_SetFixedRotation);
		mono_add_internal_call("Chroma.RigidBody::GetFixedRotation_Native", Script::RigidBody_GetFixedRotation);
		mono_add_internal_call("Chroma.RigidBody::SetBullet_Native", Script::RigidBody_SetBullet);
		mono_add_internal_call("Chroma.RigidBody::GetBullet_Native", Script::RigidBody_GetBullet);
		mono_add_internal_call("Chroma.RigidBody::SetGravityScale_Native", Script::RigidBody_SetGravityScale);
		mono_add_internal_call("Chroma.RigidBody::GetGravityScale_Native", Script::RigidBody_GetGravityScale);
		mono_add_internal_call("Chroma.RigidBody::ApplyForce_Native", Script::RigidBody_ApplyForce);
		mono_add_internal_call("Chroma.RigidBody::ApplyForceToCenter_Native", Script::RigidBody_ApplyForceToCenter);
		mono_add_internal_call("Chroma.RigidBody::ApplyTorque_Native", Script::RigidBody_ApplyTorque);
		mono_add_internal_call("Chroma.RigidBody::ApplyLinearImpulse_Native", Script::RigidBody_ApplyLinearImpulse);
		mono_add_internal_call("Chroma.RigidBody::ApplyLinearImpulseToCenter_Native", Script::RigidBody_ApplyLinearImpulseToCenter);
		mono_add_internal_call("Chroma.RigidBody::ApplyAngularImpulse_Native", Script::RigidBody_ApplyAngularImpulse);

		//Collider
		mono_add_internal_call("Chroma.Collider::GetMask_Native", Script::Collider_GetMask);
		mono_add_internal_call("Chroma.Collider::SetMask_Native", Script::Collider_SetMask);
		mono_add_internal_call("Chroma.Collider::GetLayer_Native", Script::Collider_GetLayer);
		mono_add_internal_call("Chroma.Collider::SetLayer_Native", Script::Collider_SetLayer);
		mono_add_internal_call("Chroma.Collider::GetFriction_Native", Script::Collider_GetFriction);
		mono_add_internal_call("Chroma.Collider::SetFriction_Native", Script::Collider_SetFriction);
		mono_add_internal_call("Chroma.Collider::GetRestitution_Native", Script::Collider_GetRestitution);
		mono_add_internal_call("Chroma.Collider::SetRestitution_Native", Script::Collider_SetRestitution);
		mono_add_internal_call("Chroma.Collider::GetRestitutionThreshold_Native", Script::Collider_GetRestitutionThreshold);
		mono_add_internal_call("Chroma.Collider::SetRestitutionThreshold_Native", Script::Collider_SetRestitutionThreshold);
		mono_add_internal_call("Chroma.Collider::GetDensity_Native", Script::Collider_GetDensity);
		mono_add_internal_call("Chroma.Collider::SetDensity_Native", Script::Collider_SetDensity);
		mono_add_internal_call("Chroma.Collider::GetSensor_Native", Script::Collider_GetSensor);
		mono_add_internal_call("Chroma.Collider::SetSensor_Native", Script::Collider_SetSensor);

		//CircleCollider
		mono_add_internal_call("Chroma.CircleCollider::GetRadius_Native", Script::CircleCollider_GetRadius);
		mono_add_internal_call("Chroma.CircleCollider::SetRadius_Native", Script::CircleCollider_SetRadius);

		//EdgeCollider
		mono_add_internal_call("Chroma.EdgeCollider::GetOneSided_Native", Script::EdgeCollider_GetOneSided);
		mono_add_internal_call("Chroma.EdgeCollider::SetOneSided_Native", Script::EdgeCollider_SetOneSided);
		mono_add_internal_call("Chroma.EdgeCollider::GetV0_Native", Script::EdgeCollider_GetV0);
		mono_add_internal_call("Chroma.EdgeCollider::SetV0_Native", Script::EdgeCollider_SetV0);
		mono_add_internal_call("Chroma.EdgeCollider::GetV1_Native", Script::EdgeCollider_GetV1);
		mono_add_internal_call("Chroma.EdgeCollider::SetV1_Native", Script::EdgeCollider_SetV1);
		mono_add_internal_call("Chroma.EdgeCollider::GetV2_Native", Script::EdgeCollider_GetV2);
		mono_add_internal_call("Chroma.EdgeCollider::SetV2_Native", Script::EdgeCollider_SetV2);
		mono_add_internal_call("Chroma.EdgeCollider::GetV3_Native", Script::EdgeCollider_GetV3);
		mono_add_internal_call("Chroma.EdgeCollider::SetV3_Native", Script::EdgeCollider_SetV3);

		//PolygonCollider
		mono_add_internal_call("Chroma.PolygonCollider::GetVertices_Native", Script::PolygonCollider_GetVertices);

		//RectangleCollider
		mono_add_internal_call("Chroma.RectangleCollider::SetSize_Native", Script::RectangleCollider_SetSize);
		mono_add_internal_call("Chroma.RectangleCollider::GetSize_Native", Script::RectangleCollider_GetSize);
		mono_add_internal_call("Chroma.RectangleCollider::GetRotation_Native", Script::RectangleCollider_GetRotation);
		mono_add_internal_call("Chroma.RectnalgeCollider::SetRotation_Native", Script::RectangleCollider_SetRotation);
	}
}
