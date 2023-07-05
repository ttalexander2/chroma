#pragma once


#include "TypeFactory.h"
#include "TypeData.h"
#include "TypeFlags.h"
#include "Type.h"
#include "Data.h"
#include "Function.h"
#include "Registry.h"
#include "TypeInitializer.h"
#include "iterators/data_container.h"
#include "iterators/function_container.h"
#include "iterators/type_container.h"
#include "iterators/argument_container.h"
#include "Chroma/IO/File.h"
#include "Chroma/Scene/ComponentRegistry.h"
#include "Metadata/Properties.h"

#include "yaml-cpp/emitter.h"
#include "yaml-cpp/node/node.h"

namespace Chroma
{
	// Forward Declarations
	struct File;
	class Application;
	
	namespace Reflection
	{
		//const std::vector<uint32_t>& GetTypeRequirements(uint32_t type_id);

			    /**
	     * @brief Registers a type to the reflection system.
	     * @tparam - T Type to register.
	     * @param - name Identifier for the type.
	     * @return Type factory object, used as named parameter idiom, to declare information about the type.
	     */
	    template<typename T>
	    inline TypeFactory<T> register_type(const std::string &name)
	    {
	    	auto factory = Registry::register_type<T>(name);
	    	if constexpr (std::is_base_of_v<Component, T> && !std::is_abstract_v<T>)
	    	{
	    		ComponentRegistry::RegisterComponent<T>(name);
	    	}
	        return factory;
	    }

	    /**
	     * @brief Registers a template specialization to the reflection system.
	     * @tparam -  T Template specialization to register.
	     * @param - name Identifier for the type.
	     * @return Type factory object to, used to declare information about the type.
	     */
	    template<template<typename> typename T>
	    inline auto register_type(const std::string &name)
	    {
	        return Registry::register_type<T>(name);
	    }

	    /**
	     * @brief Retrieves type information for a given identifier.
	     * @param - name The type's existing identifier.
	     * @return Object containing information about the type.
	     */
	    inline Type resolve(const std::string &name) noexcept
	    {
	        return Registry::resolve(name);
	    }

	    /**
	     * @brief Retrieves type information for a given id.
	     * @param - name The type's id.
	     * @return Object containing information about the type.
	     */
	    inline Type resolve(uint32_t id) noexcept
	    {
	        return Registry::resolve(id);
	    }

	    /**
	     * @brief Retrieves a list of all the types registered in the reflection system.
	     * @return Iterable container with all the registered types.
	     */
	    inline type_container resolve() noexcept
	    {
	        return Registry::resolve();
	    }

	    /**
	     * @brief Retrieves type information for the given type.
	     * @tparam - T Type to resolve.
	     * @return Object containing information about the type.
	     */
	    template<typename T>
	    inline Type resolve() noexcept
	    {
	        return Registry::resolve<T>();
	    }

	    /**
	     * @brief Checks whether a given identifier has been registered for a type.
	     * @param - name Identifier to validate.
	     * @return Whether the identifier is valid (there exists a type registered with the given name).
	     */
	    inline bool valid(const std::string &name)
	    {
	        return Registry::valid(name);
	    }

	    /**
	     * @brief Checks whether a type with the given ID exists in the registry.
	     * @param - id ID to validate.
	     * @return Whether the ID is valid (there exists a type registered with the given ID).
	     */
	    inline bool valid(uint32_t id)
	    {
	        return Registry::valid(id);
	    }

	    /**
	     * @brief Gets the name of a type from the given id.
	     * @param - id Assigned id to for the type.
	     * @return Assigned name to the type.
	     */
	    inline std::string type_name_from_id(uint32_t id)
	    {
	        return Registry::resolve(id).name();
	    }

	    /**
	 * @brief Gets the name of a type from the given id.
	 * @param - id Assigned id to for the type.
	 * @return Assigned name to the type.
	 */
	    inline uint32_t type_id_from_name(const std::string &name)
	    {
	        return Registry::resolve(name).id();
	    }
		



		class Initializer
		{
			template <typename T> friend class ComponentInitializer;
		public:
			static void Initialize();
			
			template <typename Type, typename = std::enable_if_t<std::is_base_of_v<Component, Type> || std::is_same_v<Component, Type>>>
			static void AddComponentInitializer()
			{
				std::function func = []()
				{
					Type::register_type();
				};
				component_initializers.push_back(func);
			}

		private:
			static inline std::vector<std::function<void()>> component_initializers{};
		};
		
	}
	
	#undef typeof
	// Gets the type data for a provided type. Equivalent to Chroma::Reflection::resolve<T>()
	// ReSharper disable once CppInconsistentNaming
	#define typeof(TYPE) Chroma::Reflection::resolve<TYPE>()

}


