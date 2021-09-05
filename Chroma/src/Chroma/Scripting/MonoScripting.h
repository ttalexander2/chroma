#pragma once

#include <Chroma/Core/Application.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include "ScriptModuleField.h"
#include "Chroma/Scene/Entity.h"
#include "Chroma/Core/Time.h"


namespace Chroma
{

	extern "C" {
		typedef struct _MonoObject MonoObject;
		typedef struct _MonoClass MonoClass;
	}

	struct EntityScriptClass;
	struct EntityInstance
	{
		EntityScriptClass* ScriptClass = nullptr;

		uint32_t Handle = 0;
		Scene* SceneInstance = nullptr;

		MonoObject* GetInstance();
		bool IsRuntimeAvailable() const;
	};

	struct EntityInstanceData
	{
		EntityInstance Instance;
		ScriptModuleFieldMap ModuleFieldMap;
	};


	using EntityInstanceMap = std::unordered_map<GUID, std::unordered_map<EntityID, EntityInstanceData>>;


	class MonoScripting
	{
		friend class Application;

	protected:
		static void Init(const std::string& assemblyPath);
		static void Shutdown();
		static void InitMono();
		static void ShutdownMono();

		static bool LoadChromaRuntimeAssembly(const std::string& path);
		static bool LoadAppAssembly(const std::string& path);
		static bool ReloadAssembly(const std::string& path);
		static MonoAssembly* LoadAssemblyFromFile(const std::string& path);
		
		static void OnSceneDestruct(GUID sceneID);

		static void SetSceneContext(const Ref<Scene>& scene);
		static const Ref<Scene>& GetCurrentSceneContext();

		static MonoObject* Construct(const std::string& fullName, bool callConstructor = true, void** parameters = nullptr);
		static MonoClass* GetCoreClass(const std::string& fullName);

		static void InitScriptEntity(Entity entity);
		static void ShutdownScriptEntity(Entity entity, const std::string& moduleName);
		static void InstantiateEntityClass(Entity entity);

		static void PreInit(Entity entity, Time t);
		static void Init(Entity entity, Time t);
		static void PostInit(Entity entity, Time t);

		static void EarlyStart(Entity entity, Time t);
		static void Start(Entity entity, Time t);
		static void LateStart(Entity entity, Time t);

		static void EarlyUpdate(Entity entity, Time t);
		static void Update(Entity entity, Time t);
		static void LateUpdate(Entity entity, Time t);

		static bool IsEntityModuleValid(Entity entity);

		static void OnScriptComponentDestroyed(GUID sceneID, EntityID entityID);

		static bool ModuleExists(const std::string& moduleName);
		static std::string StripNamespace(const std::string& nameSpace, const std::string& moduleName);


		static EntityInstanceMap& GetEntityInstanceMap();
		static EntityInstanceData& GetEntityInstanceData(GUID sceneID, EntityID entityID);


	private:

		static std::unordered_map<std::string, std::string> publicFieldStringValue;

		friend PublicField;



	};
}