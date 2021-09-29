#include "chromapch.h"
#include "ScriptingSystem.h"

#include <Chroma/Scripting/LuaScripting.h>
#include <Chroma/Components/LuaScript.h>
#include <Chroma/Scene/Entity.h>
#include <Chroma/Scripting/MonoScripting.h>
#include <lua.h>
#include <lauxlib.h>

#include <lobject.h>
#include <lstate.h>
#include <lundump.h>

namespace Chroma
{
	void ScriptingSystem::Load()
	{
		//We could probably cache the source of the scripts here with a single pass, 
		//such that it doesn't have to be loaded from disk for every instance.
		{
			auto view = m_Scene->Registry.view<LuaScript>();
			for (EntityID entity : view)
			{
				//CHROMA_CORE_ERROR("Entity: {}", entity);
				auto& script = view.get<LuaScript>(entity);
				if (script.Path.empty())
					continue;
				script.Environment["entity"] = Entity(entity, m_Scene);
				script.Success = LuaScripting::LoadScriptFromFile(script.Path, script.Environment, false);
			}
		}

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
			auto view = m_Scene->Registry.view<LuaScript>();
			for (EntityID entity : view)
			{
				auto& script = view.get<LuaScript>(entity);
				auto func = script.Environment["EarlyInit"];
				if (func.valid())
					func();
			}
		}

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
			auto view = m_Scene->Registry.view<LuaScript>();
			for (EntityID entity : view)
			{
				auto& script = view.get<LuaScript>(entity);
				auto func = script.Environment["Init"];
				if (func.valid())
					func();
			}
		}


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
			auto view = m_Scene->Registry.view<LuaScript>();
			for (EntityID entity : view)
			{
				auto& script = view.get<LuaScript>(entity);
				auto func = script.Environment["LateInit"];
				if (func.valid())
					func();
			}
		}
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
			auto view = m_Scene->Registry.view<LuaScript>();
			for (EntityID entity : view)
			{
				auto& script = view.get<LuaScript>(entity);
				script.Environment.set_function("start_coroutine", [&](const std::string& func) { script.StartCoroutine(func, 0);  });
				auto func = script.Environment["EarlyUpdate"];
				script.Environment["time"] = time;
				if (func.valid())
					func();

				for (auto& [id, coroutine] : script.Coroutines)
				{
					if (coroutine.Step == 0)
					{
						if (coroutine.Time <= 0 && coroutine.Handle.valid())
						{
							sol::protected_function_result fr = coroutine.Handle();
							if (fr.valid() && fr.get_type() == sol::type::number)
							{
								coroutine.Time = fr;
							}
						}
						else
						{
							coroutine.Time -= time.GetSeconds();
						}

					}
				}
			}
		}

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
		{


			auto view = m_Scene->Registry.view<LuaScript>();
			for (EntityID entity : view)
			{
				auto& script = view.get<LuaScript>(entity);
				if (!script.Success)
					continue;

				script.Environment.set_function("start_coroutine", [&](const std::string& func) { sol::coroutine c = script.Environment[func]; script.StartCoroutine(func, 1);  });
				auto func = script.Environment["Update"];
				script.Environment["time"] = time;
				if (func.valid())
					func();

				std::vector<std::string> toRemove;

				for (auto& [id, coroutine] : script.Coroutines)
				{
					if (coroutine.Step != 2 && coroutine.Step != 0)
					{
						if (coroutine.Time <= 0 && coroutine.Handle.valid())
						{
							try
							{
								script.RegisterEntity(entity, m_Scene);
								sol::protected_function_result fr = coroutine.Handle();
								if (fr.status() == sol::call_status::ok)
								{
									toRemove.push_back(id);
									continue;
								}
								if (fr.valid() && fr.get_type() == sol::type::number)
								{
									coroutine.Time = fr;
								}
							}
							catch (sol::error& e)
							{
								CHROMA_CORE_ERROR("{}", e.what());
							}

						}
						else
						{
							coroutine.Time -= time.GetSeconds();
						}
					}
				}

				for (auto& id : toRemove)
				{
					script.StopCoroutine(id);
				}
			}
		}
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
			auto view = m_Scene->Registry.view<LuaScript>();
			for (EntityID entity : view)
			{
				auto& script = view.get<LuaScript>(entity);
				script.Environment.set_function("start_coroutine", [&](const std::string& func) { sol::coroutine c = script.Environment[func]; script.StartCoroutine(func, 2);  });
				auto func = script.Environment["LateUpdate"];
				script.Environment["time"] = time;
				if (func.valid())
					func();

				for (auto& [id, coroutine] : script.Coroutines)
				{
					if (coroutine.Step == 2)
					{
						if (coroutine.Time <= 0 && coroutine.Handle.valid())
						{
							sol::protected_function_result fr = coroutine.Handle();
							if (fr.valid() && fr.get_type() == sol::type::number)
							{
								coroutine.Time = fr;
							}
						}
						else
						{
							coroutine.Time -= time.GetSeconds();
						}
					}
				}
			}
		}
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

