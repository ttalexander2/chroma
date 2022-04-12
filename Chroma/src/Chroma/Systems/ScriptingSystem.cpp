#include "chromapch.h"
#include "ScriptingSystem.h"

#include <Chroma/Components/CSharpScript.h>
#include <Chroma/Scene/Entity.h>
#include <Chroma/Scripting/MonoScripting.h>

namespace Chroma
{
	void ScriptingSystem::Load()
	{
		
		MonoScripting::SetDeltaTime(0.f, 0.f);
		MonoScripting::SetSceneContext(m_Scene);
		auto view = m_Scene->Registry.view<CSharpScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<CSharpScript>(entity);
			auto entityObj = Chroma::Entity(entity, m_Scene);
			//MonoScripting::InitScriptEntity(entityObj);
			MonoScripting::InstantiateEntityClass(entityObj);

			auto& entityInstanceData = Chroma::MonoScripting::GetEntityInstanceData(m_Scene->GetID(), entity);

			//CHROMA_CORE_TRACE("Loaded [{}] fields from '{}'", script.ModuleFieldMap[script.ModuleName].size(), script.ModuleName);

			for (auto& [name, field] : script.ModuleFieldMap[script.ModuleName])
			{
				field.CopyStoredValueToRuntime(entityInstanceData.Instance);
			}

			script.ExecutionOrder = MonoScripting::GetModuleExecutionOrder(script.ModuleName);
		}

		m_Scene->Registry.sort<CSharpScript>([](const CSharpScript &lhs, const CSharpScript &rhs)
			{
				return lhs.ExecutionOrder < rhs.ExecutionOrder;
			});

		CHROMA_CORE_INFO("LOADED SCRIPTING SYSTEM");

	}

	void ScriptingSystem::EarlyInit()
	{
		auto view = m_Scene->Registry.view<CSharpScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<CSharpScript>(entity);
			if (!script.IsEnabled())
				continue;
			auto entityObj = Chroma::Entity(entity, m_Scene);
			MonoScripting::PreInit(entityObj);
		}

	}

	void ScriptingSystem::Init()
	{
		auto view = m_Scene->Registry.view<CSharpScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<CSharpScript>(entity);
			if (!script.IsEnabled())
				continue;
			auto entityObj = Chroma::Entity(entity, m_Scene);
			MonoScripting::Init(entityObj);
		}
	}

	void ScriptingSystem::LateInit()
	{
		auto view = m_Scene->Registry.view<CSharpScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<CSharpScript>(entity);
			if (!script.IsEnabled())
				continue;
			auto entityObj = Chroma::Entity(entity, m_Scene);
			MonoScripting::PostInit(entityObj);
		}
	}

	void ScriptingSystem::EarlyUpdate(Time time)
	{
		MonoScripting::SetDeltaTime(time, time);

		auto view = m_Scene->Registry.view<CSharpScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<CSharpScript>(entity);
			if (!script.IsEnabled())
				continue;
			auto entityObj = Chroma::Entity(entity, m_Scene);
			MonoScripting::EarlyUpdate(entityObj, time);
		}

	}

	void ScriptingSystem::Update(Time time)
	{
		MonoScripting::SetDeltaTime(time, time);
		
		auto view = m_Scene->Registry.view<CSharpScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<CSharpScript>(entity);
			if (!script.IsEnabled())
				continue;
			auto entityObj = Chroma::Entity(entity, m_Scene);
			MonoScripting::Update(entityObj, time);

				
		}
		
	}

	void ScriptingSystem::LateUpdate(Time time)
	{
		auto view = m_Scene->Registry.view<CSharpScript>();
		for (EntityID entity : view)
		{
			auto& script = view.get<CSharpScript>(entity);
			if (!script.IsEnabled())
				continue;
			auto entityObj = Chroma::Entity(entity, m_Scene);
			MonoScripting::LateUpdate(entityObj, time);
		}
	}

}

