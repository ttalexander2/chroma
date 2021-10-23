#include "chromapch.h"
#include "ScriptingSystem.h"

#include <Chroma/Scene/Entity.h>
#include <Chroma/Scripting/MonoScripting.h>

namespace Chroma
{
	void ScriptingSystem::Load()
	{
		//We could probably cache the source of the scripts here with a single pass, 
		//such that it doesn't have to be loaded from disk for every instance.
		{
			MonoScripting::SetDeltaTime(0.f, 0.f);
			MonoScripting::SetSceneContext(m_Scene);
			auto view = m_Scene->Registry.view<CSharpScript>();
			for (EntityID entity : view)
			{
				auto& script = view.get<CSharpScript>(entity);
				auto entityObj = Chroma::Entity(entity, m_Scene);
				MonoScripting::InitScriptEntity(entityObj);
				MonoScripting::InstantiateEntityClass(entityObj);
			}

		}

	}

	void ScriptingSystem::EarlyInit()
	{
		{
			auto view = m_Scene->Registry.view<CSharpScript>();
			for (EntityID entity : view)
			{
				auto& script = view.get<CSharpScript>(entity);
				auto entityObj = Chroma::Entity(entity, m_Scene);
				MonoScripting::PreInit(entityObj);
			}
		}

	}

	void ScriptingSystem::Init()
	{
		{
			auto view = m_Scene->Registry.view<CSharpScript>();
			for (EntityID entity : view)
			{
				auto& script = view.get<CSharpScript>(entity);
				auto entityObj = Chroma::Entity(entity, m_Scene);
				MonoScripting::Init(entityObj);
			}
		}
	}

	void ScriptingSystem::LateInit()
	{
		{
			auto view = m_Scene->Registry.view<CSharpScript>();
			for (EntityID entity : view)
			{
				auto& script = view.get<CSharpScript>(entity);
				auto entityObj = Chroma::Entity(entity, m_Scene);
				MonoScripting::PostInit(entityObj);
			}
		}
	}

	void ScriptingSystem::EarlyUpdate(Time time)
	{
		MonoScripting::SetDeltaTime(time, time);
		{
			auto view = m_Scene->Registry.view<CSharpScript>();
			for (EntityID entity : view)
			{
				auto& script = view.get<CSharpScript>(entity);
				auto entityObj = Chroma::Entity(entity, m_Scene);
				MonoScripting::EarlyUpdate(entityObj, time);
			}
		}
	}

	void ScriptingSystem::Update(Time time)
	{
		MonoScripting::SetDeltaTime(time, time);
		{
			auto view = m_Scene->Registry.view<CSharpScript>();
			for (EntityID entity : view)
			{
				auto& script = view.get<CSharpScript>(entity);
				auto entityObj = Chroma::Entity(entity, m_Scene);
				MonoScripting::Update(entityObj, time);

				
			}
		}
	}

	void ScriptingSystem::LateUpdate(Time time)
	{
		{
			auto view = m_Scene->Registry.view<CSharpScript>();
			for (EntityID entity : view)
			{
				auto& script = view.get<CSharpScript>(entity);
				auto entityObj = Chroma::Entity(entity, m_Scene);
				MonoScripting::LateUpdate(entityObj, time);
			}
		}
	}

}

