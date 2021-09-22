#include "chromapch.h"
#include "MonoScripting.h"

#include "Chroma/Scene/Entity.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/mono-gc.h>
#include <Chroma/Components/CSharpScript.h>


namespace Chroma
{
	static MonoDomain* currentMonoDomain = nullptr;
	static MonoDomain* newMonoDomain = nullptr;
	static Scene* sceneContext;

	static MonoImage* appAssemblyImage = nullptr;
	static MonoImage* coreAssemblyImage = nullptr;

	static MonoAssembly* appAssembly = nullptr;
	static MonoAssembly* coreAssembly = nullptr;
	static std::string appAssemblyPath;
	static std::string coreAssemblyPath;

	static MonoClass* entityClass = nullptr;

	static EntityInstanceMap entityInstanceMap;
	static std::unordered_map<std::string, EntityScriptClass> entityClassMap;

	std::unordered_map<std::string, std::string> MonoScripting::publicFieldStringValue;

	static std::unordered_map<std::string, MonoClass*> classes;

	static MonoMethod* exceptionMethod;

	MonoMethod* GetMethod(MonoImage* image, const std::string& methodDesc);

	struct EntityScriptClass
	{
		std::string FullName;
		std::string ClassName;
		std::string NamespaceName;

		MonoClass* Class = nullptr;
		MonoMethod* Constructor = nullptr;

		MonoMethod* PreInitMethod = nullptr;
		MonoMethod* InitMethod = nullptr;
		MonoMethod* PostInitMethod = nullptr;

		MonoMethod* EarlyStartMethod = nullptr;
		MonoMethod* StartMethod = nullptr;
		MonoMethod* LateStartMethod = nullptr;

		MonoMethod* EarlyUpdateMethod = nullptr;
		MonoMethod* UpdateMethod = nullptr; 
		MonoMethod* LateUpdateMethod = nullptr;

		void InitClassMethods(MonoImage* image)
		{
			Constructor = GetMethod(coreAssemblyImage, "Chroma.Entity:.ctor(ulong)");

			PreInitMethod = GetMethod(image, FullName + ":PreInit()");
			InitMethod = GetMethod(image, FullName + ":Init()");
			PostInitMethod = GetMethod(image, FullName + ":PostInit()");

			EarlyStartMethod = GetMethod(image, FullName + ":EarlyStart()");
			StartMethod = GetMethod(image, FullName + ":Start()");
			LateStartMethod = GetMethod(image, FullName + ":LateStart()");

			EarlyUpdateMethod = GetMethod(image, FullName + ":EarlyUpdate()");
			UpdateMethod = GetMethod(image, FullName + ":Update()");
			LateUpdateMethod = GetMethod(image, FullName + ":LateUpdate()");
		}

	};

	MonoObject* EntityInstance::GetInstance()
	{
		CHROMA_CORE_ASSERT(Handle, "Entity has not been instantiated!");
		return mono_gchandle_get_target(Handle);
	}

	bool EntityInstance::IsRuntimeAvailable() const
	{
		return Handle != 0;
	}



	void MonoScripting::Init(const std::string& assemblyPath)
	{
		InitMono();
		LoadChromaRuntimeAssembly(assemblyPath);
	}

	void MonoScripting::Shutdown()
	{
		ShutdownMono();
		sceneContext = nullptr;
		entityInstanceMap.clear();
	}

	void MonoScripting::InitMono()
	{
		std::string libs = std::filesystem::absolute("mono/lib").string();
		CHROMA_CORE_INFO("Loaded Mono libraries: {}", libs);
		mono_set_assemblies_path(libs.c_str());
		// mono_jit_set_trace_options("--verbose");
		auto domain = mono_jit_init("Chroma");
	}

	void MonoScripting::ShutdownMono()
	{
		//mono_jit_cleanup(domain);
	}


	bool MonoScripting::BuildAssembly(const std::string& path)
	{
		CHROMA_CORE_INFO("Building: {}", path + "\\bin\\Debug\\net4.6\\Aesix.dll");
		system(("dotnet build " + std::string(path)).c_str());

		if (std::filesystem::exists(path + "\\bin\\Debug\\net4.6\\Aesix.dll"))
			return MonoScripting::LoadAppAssembly(path + "\\bin\\Debug\\net4.6\\Aesix.dll");
		return false;
	}

	MonoAssembly* MonoScripting::LoadAssemblyFromFile(const std::string& path)
	{
		if (path.empty() || !std::filesystem::exists(std::filesystem::path(path)))
		{
			CHROMA_CORE_WARN("Failed to load assembly [{}]: File not found.", path);
			return nullptr;
		}

		std::ifstream stream;
		stream.open(path, std::ios_base::binary);
		if (!stream.good())
		{
			CHROMA_CORE_WARN("Failed to load assembly [{}]: Failure reading stream.", path);
			return nullptr;
		}

		std::streampos fileSize;
		
		//Get Size
		stream.seekg(0, std::ios::end);
		fileSize = stream.tellg();
		stream.seekg(0, std::ios::beg);

		std::vector<std::byte> data(fileSize);
		stream.read((char*)&data[0], fileSize);
		
		stream.close();

		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(reinterpret_cast<char*>(&data[0]), fileSize, 1, &status, 0);
		if (status != MONO_IMAGE_OK)
			return nullptr;

		MonoAssembly* assembly = mono_assembly_load_from_full(image, path.c_str(), &status, 0);
		mono_image_close(image);

		return assembly;
	}

	MonoObject* MonoScripting::Construct(const std::string& fullName, bool callConstructor, void** parameters)
	{
		std::string namespaceName;
		std::string className;
		std::string parameterList;

		if (fullName.find(".") != std::string::npos)
		{
			namespaceName = fullName.substr(0, fullName.find_first_of('.'));
			className = fullName.substr(fullName.find_first_of('.') + 1, (fullName.find_first_of(':') - fullName.find_first_of('.')) - 1);
		}

		if (fullName.find(":") != std::string::npos)
		{
			parameterList = fullName.substr(fullName.find_first_of(':'));
		}

		MonoClass* clazz = mono_class_from_name(coreAssemblyImage, namespaceName.c_str(), className.c_str());
		CHROMA_CORE_ASSERT(clazz, "Could not find class!");
		MonoObject* obj = mono_object_new(mono_domain_get(), clazz);

		if (callConstructor)
		{
			MonoMethodDesc* desc = mono_method_desc_new(parameterList.c_str(), NULL);
			MonoMethod* constructor = mono_method_desc_search_in_class(desc, clazz);
			MonoObject* exception = nullptr;
			mono_runtime_invoke(constructor, obj, parameters, &exception);
		}

		return obj;
	}

	MonoClass* MonoScripting::GetCoreClass(const std::string& fullName)
	{
		if (classes.find(fullName) != classes.end())
			return classes[fullName];

		std::string namespaceName = "";
		std::string className;

		if (fullName.find('.') != std::string::npos)
		{
			namespaceName = fullName.substr(0, fullName.find_last_of('.'));
			className = fullName.substr(fullName.find_last_of('.') + 1);
		}
		else
		{
			className = fullName;
		}

		MonoClass* monoClass = mono_class_from_name(coreAssemblyImage, namespaceName.c_str(), className.c_str());
		if (!monoClass)
			std::cout << "mono_class_from_name failed" << std::endl;

		classes[fullName] = monoClass;

		return monoClass;
	}



	static uint32_t Instantiate(EntityScriptClass& scriptClass)
	{
		MonoObject* instance = mono_object_new(currentMonoDomain, scriptClass.Class);
		if (!instance)
			std::cout << "mono_object_new failed" << std::endl;

		mono_runtime_object_init(instance);
		uint32_t handle = mono_gchandle_new(instance, false);
		return handle;
	}

	static void Destroy(uint32_t handle)
	{
		mono_gchandle_free(handle);
	}


	static MonoClass* GetClass(MonoImage* image, const EntityScriptClass& scriptClass)
	{
		MonoClass* monoClass = mono_class_from_name(image, scriptClass.NamespaceName.c_str(), scriptClass.ClassName.c_str());
		if (!monoClass)
			CHROMA_CORE_WARN("mono_class_from_name failed");

		return monoClass;
	}

	static std::string GetStringProperty(const char* propertyName, MonoClass* classType, MonoObject* object)
	{
		MonoProperty* property = mono_class_get_property_from_name(classType, propertyName);
		MonoMethod* getterMethod = mono_property_get_get_method(property);
		MonoString* string = (MonoString*)mono_runtime_invoke(getterMethod, object, NULL, NULL);
		return string != nullptr ? std::string(mono_string_to_utf8(string)) : "";
	}


	static MonoObject* CallMethod(MonoObject* object, MonoMethod* method, void** params = nullptr)
	{
		MonoObject* pException = nullptr;
		MonoObject* result = mono_runtime_invoke(method, object, params, &pException);
		if (pException)
		{
			MonoClass* exceptionClass = mono_object_get_class(pException);
			MonoType* exceptionType = mono_class_get_type(exceptionClass);
			const char* typeName = mono_type_get_name(exceptionType);
			std::string message = GetStringProperty("Message", exceptionClass, pException);
			std::string stackTrace = GetStringProperty("StackTrace", exceptionClass, pException);

			CHROMA_CORE_ERROR("{0}: {1}. Stack Trace: {2}", typeName, message, stackTrace);

			void* args[] = { pException };
			MonoObject* result = mono_runtime_invoke(exceptionMethod, nullptr, args, nullptr);
		}
		return result;
	}




	MonoMethod* GetMethod(MonoImage* image, const std::string& methodDesc)
	{
		if (!image)
		{
			CHROMA_CORE_WARN("Mono image not valid!: {}", methodDesc);
			return nullptr;
		}

		MonoMethodDesc* desc = mono_method_desc_new(methodDesc.c_str(), NULL);
		if (!desc)
			CHROMA_CORE_ERROR("mono_method_desc_new failed ({})", methodDesc);

		MonoMethod* method = mono_method_desc_search_in_image(desc, image);
		if (!method)
		{
			CHROMA_CORE_ERROR("mono_method_desc_search_in_image failed ({})", methodDesc);
			return nullptr;
		}
			

		return method;
	}


	static uint32_t GetFieldSize(FieldType type)
	{
		switch (type)
		{
		case FieldType::Float:       return 4;
		case FieldType::Int:         return 4;
		case FieldType::UnsignedInt: return 4;
		case FieldType::String:      return 8;
		case FieldType::Vec2:        return 4 * 2;
		case FieldType::Vec3:        return 4 * 3;
		case FieldType::Vec4:        return 4 * 4;
		case FieldType::Asset:       return 8;
		case FieldType::Entity:		 return 8;
		}
		CHROMA_CORE_ASSERT(false, "Unknown field type!");
		return 0;
	}




	static bool postLoadCleanup = false;

	bool MonoScripting::LoadChromaRuntimeAssembly(const std::string& path)
	{
		coreAssemblyPath = path;
		if (currentMonoDomain)
		{
			newMonoDomain = mono_domain_create_appdomain((char*)"Chroma Runtime", nullptr);
			mono_domain_set(newMonoDomain, false);
			postLoadCleanup = true;
		}
		else
		{
			currentMonoDomain = mono_domain_create_appdomain((char*)"Chroma Runtime", nullptr);
			mono_domain_set(currentMonoDomain, false);
			postLoadCleanup = false;
		}

		coreAssembly = LoadAssemblyFromFile(path);
		if (!coreAssembly)
			return false;

		auto img = mono_assembly_get_image(coreAssembly);
		if (!img)
			return false;

		coreAssemblyImage = img;
		exceptionMethod = GetMethod(coreAssemblyImage, "Chroma.RuntimeException:OnException(object)");
		entityClass = mono_class_from_name(coreAssemblyImage, "Chroma", "Entity");

		if (!entityClass)
			return false;
		return true;

	}

	bool MonoScripting::LoadAppAssembly(const std::string& path)
	{
		CHROMA_CORE_INFO("LOADING APP ASSEMBLY");
		if (appAssembly)
		{
			appAssembly = nullptr;
			appAssemblyImage = nullptr;
			return ReloadAssembly(path);
		}

		auto app_assembly = LoadAssemblyFromFile(path);
		if (!app_assembly)
		{
			CHROMA_CORE_ERROR("Failed to load App assembly: {}", path);
			return false;
		}
			

		auto app_assembly_image = mono_assembly_get_image(app_assembly);
		if (!app_assembly_image)
		{
			CHROMA_CORE_ERROR("Failed to load App assembly: {}", path);
			return false;
		}
			

		//ScriptEngineRegistry::RegisterAll();

		if (postLoadCleanup)
		{
			mono_domain_unload(currentMonoDomain);
			currentMonoDomain = newMonoDomain;
			newMonoDomain = nullptr;
		}

		appAssembly = app_assembly;
		appAssemblyImage = app_assembly_image;
		return true;
	}

	bool MonoScripting::ReloadAssembly(const std::string& path)
	{
		if (!LoadChromaRuntimeAssembly(coreAssemblyPath))
			return false;

		if (!LoadAppAssembly(path))
			return false;

		if (entityInstanceMap.size())
		{
			Scene* scene = GetCurrentSceneContext();
			CHROMA_CORE_ASSERT(scene, "No active scene!");
			const auto& map = entityInstanceMap.find(scene->GetID());
			if (map != entityInstanceMap.end())
			{
				for (auto& [entityID, data] : map->second)
				{
					CHROMA_CORE_ASSERT(scene->Registry.valid(entityID), "Invalid entity ID");
					InitScriptEntity(Entity(entityID, scene));
				}
			}
		}
	}

	void MonoScripting::SetSceneContext(Scene* scene)
	{
		classes.clear();
		sceneContext = scene;
		entityInstanceMap[scene->GetID()] = std::unordered_map<EntityID, EntityInstanceData>();
	}

	Scene* MonoScripting::GetCurrentSceneContext()
	{
		return sceneContext;
	}

	void MonoScripting::OnSceneDestruct(GUID sceneID)
	{
		if (entityInstanceMap.find(sceneID) != entityInstanceMap.end())
		{
			entityInstanceMap.at(sceneID).clear();
			entityInstanceMap.erase(sceneID);
		}
	}

	static FieldType GetChromaFieldType(MonoType* monoType)
	{
		int type = mono_type_get_type(monoType);
		switch (type)
		{
			case MONO_TYPE_R4: return FieldType::Float;
			case MONO_TYPE_I4: return FieldType::Int;
			case MONO_TYPE_U4: return FieldType::UnsignedInt;
			case MONO_TYPE_STRING: return FieldType::String;
			case MONO_TYPE_CLASS:
			{
				char* name = mono_type_get_name(monoType);
				if (strcmp(name, "Chroma.Prefab") == 0) return FieldType::Asset;
				if (strcmp(name, "Chroma.Entity") == 0) return FieldType::Entity;
				return FieldType::ClassReference;
			}
			case MONO_TYPE_VALUETYPE:
			{
				char* name = mono_type_get_name(monoType);
				if (strcmp(name, "Chroma.Vector2") == 0) return FieldType::Vec2;
				if (strcmp(name, "Chroma.Vector3") == 0) return FieldType::Vec3;
				if (strcmp(name, "Chroma.Vector4") == 0) return FieldType::Vec4;
			}
		}
		return FieldType::None;
	}

	bool MonoScripting::IsEntityModuleValid(Entity entity)
	{
		return entity.HasComponent<CSharpScript>() && ModuleExists(entity.GetComponent<CSharpScript>().ModuleName);
	}

	bool MonoScripting::ModuleExists(const std::string& moduleName)
	{
		if (!appAssemblyImage) // No assembly loaded
			return false;

		std::string NamespaceName, ClassName;
		if (moduleName.find('.') != std::string::npos)
		{
			NamespaceName = moduleName.substr(0, moduleName.find_last_of('.'));
			ClassName = moduleName.substr(moduleName.find_last_of('.') + 1);
		}
		else
		{
			ClassName = moduleName;
		}

		MonoClass* monoClass = mono_class_from_name(appAssemblyImage, NamespaceName.c_str(), ClassName.c_str());
		if (!monoClass)
			return false;

		auto isEntitySubclass = mono_class_is_subclass_of(monoClass, entityClass, 0);
		return isEntitySubclass;
	}

	std::string MonoScripting::StripNamespace(const std::string& nameSpace, const std::string& moduleName)
	{
		std::string name = moduleName;
		size_t pos = name.find(nameSpace + ".");
		if (pos == 0)
		{
			{
				name.erase(pos, nameSpace.length() + 1);
			}
		}
		return name;
	}

	void MonoScripting::OnScriptComponentDestroyed(GUID sceneID, EntityID entityID)
	{
		if (entityInstanceMap.find(sceneID) != entityInstanceMap.end())
		{
			auto& entityMap = entityInstanceMap.at(sceneID);
			//HZ_CORE_ASSERT(entityMap.find(entityID) != entityMap.end());
			if (entityMap.find(entityID) != entityMap.end())
				entityMap.erase(entityID);
		}
	}





	void MonoScripting::InitScriptEntity(Entity entity)
	{
		Scene& scene = entity.GetScene();
		EntityID id = entity.GetID();
		auto& moduleName = entity.GetComponent<CSharpScript>().ModuleName;
		if (moduleName.empty())
			return;

		if (!ModuleExists(moduleName))
		{
			CHROMA_CORE_ERROR("Entity [{}] unable to find script module '{}'", id, moduleName);
			return;
		}

		EntityScriptClass& scriptClass = entityClassMap[moduleName];
		scriptClass.FullName = moduleName;
		if (moduleName.find('.') != std::string::npos)
		{
			scriptClass.NamespaceName = moduleName.substr(0, moduleName.find_last_of('.'));
			scriptClass.ClassName = moduleName.substr(moduleName.find_last_of('.') + 1);
		}
		else
		{
			scriptClass.ClassName = moduleName;
		}

		scriptClass.Class = GetClass(appAssemblyImage, scriptClass);
		scriptClass.InitClassMethods(appAssemblyImage);

		EntityInstanceData& entityInstanceData = entityInstanceMap[scene.GetID()][id];
		EntityInstance& entityInstance = entityInstanceData.Instance;
		entityInstance.ScriptClass = &scriptClass;

		CSharpScript& scriptComponent = entity.GetComponent<CSharpScript>();
		ScriptModuleFieldMap& moduleFieldMap = scriptComponent.ModuleFieldMap;
		auto& fieldMap = moduleFieldMap[moduleName];

		//Save old fields
		std::unordered_map<std::string, PublicField> oldFields;
		oldFields.reserve(fieldMap.size());
		for (auto& [fieldName, field] : fieldMap)
			oldFields.emplace(fieldName, std::move(field));

		entityInstance.Handle = Instantiate(*entityInstance.ScriptClass);

		void* param[] = { &id };
		CallMethod(entityInstance.GetInstance(), entityInstance.ScriptClass->Constructor, param);

		fieldMap.clear();
		{
			MonoClassField* iter;
			void* ptr = 0;
			while ((iter = mono_class_get_fields(scriptClass.Class, &ptr)) != NULL)
			{
				const char* name = mono_field_get_name(iter);
				uint32_t flags = mono_field_get_flags(iter);
				if ((flags & MONO_FIELD_ATTR_PUBLIC) == 0)
					continue;

				MonoType* fieldType = mono_field_get_type(iter);
				FieldType hazelFieldType = GetChromaFieldType(fieldType);

				char* typeName = mono_type_get_name(fieldType);

				auto old = oldFields.find(name);
				if ((old != oldFields.end()) && (old->second.TypeName == typeName))
				{
					fieldMap.emplace(name, std::move(oldFields.at(name)));
					PublicField& field = fieldMap.at(name);
					field.m_MonoClassField = iter;
					continue;
				}

				if (hazelFieldType == FieldType::ClassReference)
					continue;

				// TODO: Attributes
				MonoCustomAttrInfo* attr = mono_custom_attrs_from_field(scriptClass.Class, iter);

				PublicField field = { name, typeName, hazelFieldType };
				field.m_MonoClassField = iter;
				field.CopyStoredValueFromRuntime(entityInstance);
				fieldMap.emplace(name, std::move(field));
			}
		}

		{
			MonoProperty* iter;
			void* ptr = 0;
			while ((iter = mono_class_get_properties(scriptClass.Class, &ptr)) != NULL)
			{
				const char* propertyName = mono_property_get_name(iter);

				if (oldFields.find(propertyName) != oldFields.end())
				{
					fieldMap.emplace(propertyName, std::move(oldFields.at(propertyName)));
					PublicField& field = fieldMap.at(propertyName);
					field.m_MonoProperty = iter;
					continue;
				}

				MonoMethod* propertySetter = mono_property_get_set_method(iter);
				MonoMethod* propertyGetter = mono_property_get_get_method(iter);

				uint32_t setterFlags = 0;
				uint32_t getterFlags = 0;

				bool isReadOnly = false;
				MonoType* monoType = nullptr;

				if (propertySetter)
				{
					void* i = nullptr;
					MonoMethodSignature* sig = mono_method_signature(propertySetter);
					setterFlags = mono_method_get_flags(propertySetter, nullptr);
					isReadOnly = (setterFlags & MONO_METHOD_ATTR_PRIVATE) != 0;
					monoType = mono_signature_get_params(sig, &i);
				}

				if (propertyGetter)
				{
					MonoMethodSignature* sig = mono_method_signature(propertyGetter);
					getterFlags = mono_method_get_flags(propertyGetter, nullptr);

					if (monoType != nullptr)
						monoType = mono_signature_get_return_type(sig);

					if ((getterFlags & MONO_METHOD_ATTR_PRIVATE) != 0)
						continue;
				}

				if ((setterFlags & MONO_METHOD_ATTR_STATIC) != 0)
					continue;

				FieldType type = GetChromaFieldType(monoType);
				if (type == FieldType::ClassReference)
					continue;

				char* typeName = mono_type_get_name(monoType);

				PublicField field = { propertyName, typeName, type, isReadOnly };
				field.m_MonoProperty = iter;
				field.CopyStoredValueFromRuntime(entityInstance);
				fieldMap.emplace(propertyName, std::move(field));
			}
		}

		Destroy(entityInstance.Handle);


	}


	void MonoScripting::ShutdownScriptEntity(Entity entity, const std::string& moduleName)
	{
		{
			EntityInstanceData& entityInstanceData = GetEntityInstanceData(entity.GetScene().GetID(), entity.GetID());
			ScriptModuleFieldMap& moduleFieldMap = entityInstanceData.ModuleFieldMap;
			if (moduleFieldMap.find(moduleName) != moduleFieldMap.end())
				moduleFieldMap.erase(moduleName);
		}

		{
			CSharpScript& scriptComponent = entity.GetComponent<CSharpScript>();
			ScriptModuleFieldMap& moduleFieldMap = scriptComponent.ModuleFieldMap;
			if (moduleFieldMap.find(moduleName) != moduleFieldMap.end())
				moduleFieldMap.erase(moduleName);
		}
	}


	void MonoScripting::InstantiateEntityClass(Entity entity)
	{
		Scene& scene = entity.GetScene();
		EntityID id = entity.GetID();

		CSharpScript& scriptComponent = entity.GetComponent<CSharpScript>();
		auto& moduleName = scriptComponent.ModuleName;

		EntityInstanceData& entityInstanceData = GetEntityInstanceData(scene.GetID(), id);
		EntityInstance& entityInstance = entityInstanceData.Instance;
		CHROMA_CORE_ASSERT(entityInstance.ScriptClass, "");
		entityInstance.Handle = Instantiate(*entityInstance.ScriptClass);

		void* param[] = { &id };
		CallMethod(entityInstance.GetInstance(), entityInstance.ScriptClass->Constructor, param);

		//Set public fields
		ScriptModuleFieldMap& moduleFieldMap = scriptComponent.ModuleFieldMap;
		if (moduleFieldMap.find(moduleName) != moduleFieldMap.end())
		{
			auto& publicFields = moduleFieldMap.at(moduleName);
			for (auto& [name, field] : publicFields)
				field.CopyStoredValueFromRuntime(entityInstance);
		}
	}

	EntityInstanceData& MonoScripting::GetEntityInstanceData(GUID sceneID, EntityID entityID)
	{
		auto& entityIDMap = entityInstanceMap.at(sceneID);

		if (entityIDMap.find(entityID) == entityIDMap.end())
			MonoScripting::InitScriptEntity(Entity(entityID, sceneContext));

		return entityIDMap.at(entityID);
	}

	EntityInstanceMap& MonoScripting::GetEntityInstanceMap()
	{
		return entityInstanceMap;
	}

	void MonoScripting::PreInit(Entity entity)
	{
		EntityInstanceData& entityInstance = GetEntityInstanceData(entity.GetScene().GetID(), entity.GetID());
		if (entityInstance.Instance.ScriptClass->PreInitMethod)
		{
			CallMethod(entityInstance.Instance.GetInstance(), entityInstance.Instance.ScriptClass->PreInitMethod);
		}
	}

	void MonoScripting::Init(Entity entity)
	{
		EntityInstanceData& entityInstance = GetEntityInstanceData(entity.GetScene().GetID(), entity.GetID());
		if (entityInstance.Instance.ScriptClass->InitMethod)
		{
			CallMethod(entityInstance.Instance.GetInstance(), entityInstance.Instance.ScriptClass->InitMethod);
		}
	}

	void MonoScripting::PostInit(Entity entity)
	{
		EntityInstanceData& entityInstance = GetEntityInstanceData(entity.GetScene().GetID(), entity.GetID());
		if (entityInstance.Instance.ScriptClass->PostInitMethod)
		{
			CallMethod(entityInstance.Instance.GetInstance(), entityInstance.Instance.ScriptClass->PostInitMethod);
		}
	}
	void MonoScripting::EarlyStart(Entity entity)
	{
		EntityInstanceData& entityInstance = GetEntityInstanceData(entity.GetScene().GetID(), entity.GetID());
		if (entityInstance.Instance.ScriptClass->EarlyStartMethod)
		{
			CallMethod(entityInstance.Instance.GetInstance(), entityInstance.Instance.ScriptClass->EarlyStartMethod);
		}
	}
	void MonoScripting::Start(Entity entity)
	{
		EntityInstanceData& entityInstance = GetEntityInstanceData(entity.GetScene().GetID(), entity.GetID());
		if (entityInstance.Instance.ScriptClass->StartMethod)
		{
			CallMethod(entityInstance.Instance.GetInstance(), entityInstance.Instance.ScriptClass->StartMethod);
		}
	}
	void MonoScripting::LateStart(Entity entity)
	{
		EntityInstanceData& entityInstance = GetEntityInstanceData(entity.GetScene().GetID(), entity.GetID());
		if (entityInstance.Instance.ScriptClass->LateStartMethod)
		{
			CallMethod(entityInstance.Instance.GetInstance(), entityInstance.Instance.ScriptClass->LateStartMethod);
		}
	}
	void MonoScripting::EarlyUpdate(Entity entity, Time t)
	{
		EntityInstanceData& entityInstance = GetEntityInstanceData(entity.GetScene().GetID(), entity.GetID());
		if (entityInstance.Instance.ScriptClass->EarlyUpdateMethod)
		{
			CallMethod(entityInstance.Instance.GetInstance(), entityInstance.Instance.ScriptClass->EarlyUpdateMethod);
		}
	}
	void MonoScripting::Update(Entity entity, Time t)
	{
		EntityInstanceData& entityInstance = GetEntityInstanceData(entity.GetScene().GetID(), entity.GetID());
		if (entityInstance.Instance.ScriptClass->UpdateMethod)
		{
			CallMethod(entityInstance.Instance.GetInstance(), entityInstance.Instance.ScriptClass->UpdateMethod);
		}
	}
	void MonoScripting::LateUpdate(Entity entity, Time t)
	{
		EntityInstanceData& entityInstance = GetEntityInstanceData(entity.GetScene().GetID(), entity.GetID());
		if (entityInstance.Instance.ScriptClass->LateUpdateMethod)
		{
			CallMethod(entityInstance.Instance.GetInstance(), entityInstance.Instance.ScriptClass->LateUpdateMethod);
		}
	}

	PublicField::PublicField(const std::string& name, const std::string& typeName, FieldType type, bool isReadOnly)
		: Name(name), TypeName(typeName), Type(type), IsReadOnly(isReadOnly)
	{
		if (Type != FieldType::String)
			m_StoredValueBuffer = AllocateBuffer(Type);
	}

	PublicField::PublicField(const PublicField& other)
		: Name(other.Name), TypeName(other.TypeName), Type(other.Type), IsReadOnly(other.IsReadOnly)
	{
		if (Type != FieldType::String)
		{
			m_StoredValueBuffer = AllocateBuffer(Type);
			memcpy(m_StoredValueBuffer, other.m_StoredValueBuffer, GetFieldSize(Type));
		}
		else
		{
			m_StoredValueBuffer = other.m_StoredValueBuffer;
		}

		m_MonoClassField = other.m_MonoClassField;
		m_MonoProperty = other.m_MonoProperty;
	}

	PublicField& PublicField::operator=(const PublicField& other)
	{
		if (&other != this)
		{
			Name = other.Name;
			TypeName = other.TypeName;
			Type = other.Type;
			IsReadOnly = other.IsReadOnly;
			m_MonoClassField = other.m_MonoClassField;
			m_MonoProperty = other.m_MonoProperty;
			if (Type != FieldType::String)
			{
				m_StoredValueBuffer = AllocateBuffer(Type);
				memcpy(m_StoredValueBuffer, other.m_StoredValueBuffer, GetFieldSize(Type));
			}
			else
			{
				m_StoredValueBuffer = other.m_StoredValueBuffer;
			}
		}

		return *this;
	}

	PublicField::PublicField(PublicField&& other)
	{
		Name = std::move(other.Name);
		TypeName = std::move(other.TypeName);
		Type = other.Type;
		IsReadOnly = other.IsReadOnly;
		m_MonoClassField = other.m_MonoClassField;
		m_MonoProperty = other.m_MonoProperty;
		m_StoredValueBuffer = other.m_StoredValueBuffer;

		other.m_MonoClassField = nullptr;
		other.m_MonoProperty = nullptr;
		if (Type != FieldType::String)
			other.m_StoredValueBuffer = nullptr;
	}

	PublicField::~PublicField()
	{
		if (Type != FieldType::String)
			delete[] m_StoredValueBuffer;
	}

	void PublicField::CopyStoredValueToRuntime(EntityInstance& entityInstance)
	{
		CHROMA_CORE_ASSERT(entityInstance.GetInstance(), "");

		if (IsReadOnly)
			return;

		if (Type == FieldType::ClassReference)
		{

			// Create Managed Object
			void* params[] = {
				&m_StoredValueBuffer
			};
			MonoObject* obj = MonoScripting::Construct(TypeName + ":.ctor(intptr)", true, params);
			mono_field_set_value(entityInstance.GetInstance(), m_MonoClassField, obj);
		}
		else if (Type == FieldType::Asset || Type == FieldType::Entity)
		{
			// Create Managed Object
			void* params[] = { m_StoredValueBuffer };
			MonoObject* obj = MonoScripting::Construct(TypeName + ":.ctor(ulong)", true, params);
			if (m_MonoProperty)
			{
				void* data[] = { obj };
				mono_property_set_value(m_MonoProperty, entityInstance.GetInstance(), data, nullptr);
			}
			else
			{
				mono_field_set_value(entityInstance.GetInstance(), m_MonoClassField, obj);
			}
		}
		else if (Type == FieldType::String)
		{
			SetRuntimeValue_Internal(entityInstance, *(std::string*)m_StoredValueBuffer);
		}
		else
		{
			SetRuntimeValue_Internal(entityInstance, m_StoredValueBuffer);
		}
	}

	void PublicField::CopyStoredValueFromRuntime(EntityInstance& entityInstance)
	{
		CHROMA_CORE_ASSERT(entityInstance.GetInstance(), "");

		if (Type == FieldType::String)
		{
			if (m_MonoProperty)
			{
				MonoString* str = (MonoString*)mono_property_get_value(m_MonoProperty, entityInstance.GetInstance(), nullptr, nullptr);
				MonoScripting::publicFieldStringValue[Name] = mono_string_to_utf8(str);
				m_StoredValueBuffer = (uint8_t*)&MonoScripting::publicFieldStringValue[Name];
			}
			else
			{
				MonoString* str;
				mono_field_get_value(entityInstance.GetInstance(), m_MonoClassField, &str);
				MonoScripting::publicFieldStringValue[Name] = mono_string_to_utf8(str);
				m_StoredValueBuffer = (uint8_t*)&MonoScripting::publicFieldStringValue[Name];
			}
		}
		else
		{
			if (m_MonoProperty)
			{
				MonoObject* result = mono_property_get_value(m_MonoProperty, entityInstance.GetInstance(), nullptr, nullptr);
				memcpy(m_StoredValueBuffer, mono_object_unbox(result), GetFieldSize(Type));
			}
			else
			{
				mono_field_get_value(entityInstance.GetInstance(), m_MonoClassField, m_StoredValueBuffer);
			}
		}
	}

	void PublicField::SetStoredValueRaw(void* src)
	{
		if (IsReadOnly)
			return;

		uint32_t size = GetFieldSize(Type);
		memcpy(m_StoredValueBuffer, src, size);
	}

	void PublicField::SetRuntimeValueRaw(EntityInstance& entityInstance, void* src)
	{
		CHROMA_CORE_ASSERT(entityInstance.GetInstance(), "");

		if (IsReadOnly)
			return;

		if (m_MonoProperty)
		{
			void* data[] = { src };
			mono_property_set_value(m_MonoProperty, entityInstance.GetInstance(), data, nullptr);
		}
		else
		{
			mono_field_set_value(entityInstance.GetInstance(), m_MonoClassField, src);
		}
	}

	void* PublicField::GetRuntimeValueRaw(EntityInstance& entityInstance)
	{
		CHROMA_CORE_ASSERT(entityInstance.GetInstance(), "");

		uint8_t* outValue = nullptr;
		mono_field_get_value(entityInstance.GetInstance(), m_MonoClassField, outValue);
		return outValue;
	}

	uint8_t* PublicField::AllocateBuffer(FieldType type)
	{
		uint32_t size = GetFieldSize(type);
		uint8_t* buffer = new uint8_t[size];
		memset(buffer, 0, size);
		return buffer;
	}

	void PublicField::SetStoredValue_Internal(void* value)
	{
		if (IsReadOnly)
			return;

		uint32_t size = GetFieldSize(Type);
		memcpy(m_StoredValueBuffer, value, size);
	}

	void PublicField::GetStoredValue_Internal(void* outValue) const
	{
		uint32_t size = GetFieldSize(Type);
		memcpy(outValue, m_StoredValueBuffer, size);
	}

	void PublicField::SetRuntimeValue_Internal(EntityInstance& entityInstance, void* value)
	{
		CHROMA_CORE_ASSERT(entityInstance.GetInstance(), "");

		if (IsReadOnly)
			return;

		if (m_MonoProperty)
		{
			void* data[] = { value };
			mono_property_set_value(m_MonoProperty, entityInstance.GetInstance(), data, nullptr);
		}
		else
		{
			mono_field_set_value(entityInstance.GetInstance(), m_MonoClassField, value);
		}
	}

	void PublicField::SetRuntimeValue_Internal(EntityInstance& entityInstance, const std::string& value)
	{
		if (IsReadOnly)
			return;

		CHROMA_CORE_ASSERT(entityInstance.GetInstance(), "");

		MonoString* monoString = mono_string_new(mono_domain_get(), value.c_str());

		if (m_MonoProperty)
		{
			void* data[] = { monoString };
			mono_property_set_value(m_MonoProperty, entityInstance.GetInstance(), data, nullptr);
		}
		else
		{
			mono_field_set_value(entityInstance.GetInstance(), m_MonoClassField, monoString);
		}
	}

	void PublicField::GetRuntimeValue_Internal(EntityInstance& entityInstance, void* outValue) const
	{
		CHROMA_CORE_ASSERT(entityInstance.GetInstance(), "");

		if (Type == FieldType::Entity)
		{
			MonoObject* obj;
			if (m_MonoProperty)
				obj = mono_property_get_value(m_MonoProperty, entityInstance.GetInstance(), nullptr, nullptr);
			else
				mono_field_get_value(entityInstance.GetInstance(), m_MonoClassField, &obj);

			MonoProperty* idProperty = mono_class_get_property_from_name(entityInstance.ScriptClass->Class, "ID");
			MonoObject* idObject = mono_property_get_value(idProperty, obj, nullptr, nullptr);
			memcpy(outValue, mono_object_unbox(idObject), GetFieldSize(Type));
		}
		else
		{
			if (m_MonoProperty)
			{
				MonoObject* result = mono_property_get_value(m_MonoProperty, entityInstance.GetInstance(), nullptr, nullptr);
				memcpy(outValue, mono_object_unbox(result), GetFieldSize(Type));
			}
			else
			{
				mono_field_get_value(entityInstance.GetInstance(), m_MonoClassField, outValue);
			}
		}

	}

	void PublicField::GetRuntimeValue_Internal(EntityInstance& entityInstance, std::string& outValue) const
	{
		CHROMA_CORE_ASSERT(entityInstance.GetInstance(), "");

		MonoString* monoString;
		if (m_MonoProperty)
			monoString = (MonoString*)mono_property_get_value(m_MonoProperty, entityInstance.GetInstance(), nullptr, nullptr);
		else
			mono_field_get_value(entityInstance.GetInstance(), m_MonoClassField, &monoString);

		outValue = mono_string_to_utf8(monoString);
	}


	


}


