#pragma once

#include <entt/entt.hpp>
#include <string>
#include <type_traits>
#include <concepts>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <Chroma/Utilities/StringHash.h>
#include <Chroma/IO/File.h>
#include <Chroma/Core/Log.h>
#include <iostream>
#include <utility>
#include <range/v3/view/join.hpp>
#include "Chroma/Scene/ComponentRegistry.h"

namespace Chroma
{
	struct Component;

	/// @brief This is the main class that controlls the runtime reflection system.
	/// In order to register a type, use the CHROMA_REFLECT() macro, and implement the static
	/// RegisterType() function.
	/// 
	/// All of the classes specified here initialially had their own implementations, but due to various concerns,
	/// they were instead tweaked to wrap the entt runtime reflection libaray.
	struct Reflection
	{

	struct Type;
	struct Data;
	struct Func;
	template <typename T> struct TypeFactory;
	struct Any;

	struct Handle;


	public:

		struct SequenceContainer
		{
			friend struct Reflection;
			friend struct Reflection::Type;
			friend struct Reflection::Any;
			friend struct Reflection::Func;
			template <typename T> friend struct Reflection::TypeFactory;

			struct Iterator
			{
				friend struct SequenceContainer;

				using difference_type = std::ptrdiff_t;
				using value_type = Any;
				using pointer = void;
				using reference = value_type;
				using iterator_category = std::input_iterator_tag;

				Iterator() = default;
				Iterator(entt::meta_sequence_container::iterator iterator) :
						m_Iterator(iterator)
				{
				}

				Iterator &operator++()
				{
					m_Iterator.operator++();
					return *this;
				}

				Iterator operator++(int)
				{
					m_Iterator.operator++();
					return *this;
				}

				reference operator*() const
				{
					auto type = m_Iterator.operator*();
					return Any(type);
				}

				bool operator==(const Iterator &other) const
				{
					return other.m_Iterator == m_Iterator;
				}

				bool operator!=(const Iterator &other) const
				{
					return !(m_Iterator == other.m_Iterator);
				}

			private:
				entt::meta_sequence_container::iterator m_Iterator;
			};

			inline Type ValueType() const { return m_Container.value_type(); }
			inline size_t Size() const { return m_Container.size(); }
			inline bool Clear() { return m_Container.clear(); }
			
			inline Iterator begin() 
			{
				return Iterator(m_Container.begin());
			}

			inline Iterator end()
			{
				return Iterator(m_Container.end());
			}

			inline Iterator Insert(Iterator it, Any val)
			{
				auto its = m_Container.insert(it.m_Iterator, val.m_Value);
				return Iterator(std::move(its));
			}

			inline Iterator Erase(Iterator it)
			{
				auto its = m_Container.erase(it.m_Iterator);
				return Iterator(std::move(its));
			}

			inline Any operator[](size_t size)
			{
				return m_Container[size];
			}

			inline explicit operator bool() const
			{
				return m_Container.operator bool();
			}

		private:
			SequenceContainer(entt::meta_sequence_container container) :
					m_Container(container) {}

			entt::meta_sequence_container m_Container;
		};



		struct AssociativeContainer
		{
			friend struct Reflection;
			friend struct Reflection::Type;
			friend struct Reflection::Any;
			friend struct Reflection::Func;
			template <typename T>
			friend struct Reflection::TypeFactory;

			struct Iterator
			{
				friend struct AssociativeContainer;

				using difference_type = std::ptrdiff_t;
				using value_type = std::pair<Any, Any>;
				using pointer = void;
				using reference = value_type;
				using iterator_category = std::input_iterator_tag;

				Iterator() = default;
				Iterator(entt::meta_associative_container::iterator iterator) :
						m_Iterator(iterator)
				{
				}

				Iterator &operator++()
				{
					m_Iterator.operator++();
					return *this;
				}

				Iterator operator++(int)
				{
					m_Iterator.operator++();
					return *this;
				}

				reference operator*() const
				{
					auto [key, value] = m_Iterator.operator*();
					return std::make_pair(Any(key), Any(value));
				}

				bool operator==(const Iterator &other) const
				{
					return other.m_Iterator == m_Iterator;
				}

				bool operator!=(const Iterator &other) const
				{
					return !(m_Iterator == other.m_Iterator);
				}

			private:
				entt::meta_associative_container::iterator m_Iterator;
			};

			inline bool KeyOnly() const { return m_Container.key_only(); }
			inline Type KeyType() const { return Type(m_Container.key_type()); }
			inline Type MappedType() const { return Type(m_Container.mapped_type()); }
			inline Type ValueType() const { return Type(m_Container.value_type()); }
			inline size_t Size() const { return m_Container.size(); }
			inline bool Clear() { return m_Container.clear(); }

			inline Iterator begin()
			{
				return Iterator(m_Container.begin());
			}

			inline Iterator end()
			{
				return Iterator(m_Container.end());
			}

			inline bool Insert(Any key, Any value = {})
			{
				return m_Container.insert(key, value);
			}

			inline bool Erase(Any key)
			{
				return m_Container.erase(key);
			}

			inline Iterator Find(Any any)
			{
				return Iterator(m_Container.find(any));
			}

			inline explicit operator bool() const
			{
				return m_Container.operator bool();
			}

		private:
			
			AssociativeContainer(entt::meta_associative_container container) :
					m_Container(container) {}


			entt::meta_associative_container m_Container;
		};

		struct Any
		{
			friend struct Reflection;
			friend struct Reflection::Type;
			friend struct Data;
			friend struct Func;
			template <typename T>
			friend struct TypeFactory;

			Any() = default;

			/// @brief Assign by copy
			/// @tparam T type of value
			/// @tparam  
			/// @param value 
			template <typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, Any> && !std::is_same_v<std::decay_t<T>, entt::meta_any>>>
			Any(T &&value) :
					m_Value(std::forward<T>(value))
			{
			}

			Any(const Any &other) = default;
			Any(Any &&other) :
					m_Value(other.m_Value) {}

			Any &operator=(const Any &other) 
			{
				m_Value = other.m_Value;
				return *this;
			}

			Any &operator=(Any &&other)
			{
				m_Value = other.m_Value;
				return *this;
			}

			template<typename T>
			std::enable_if_t<!std::is_same_v<std::decay_t<T>, Any>, Any &>
			operator=(T &&value)
			{
				m_Value = value;
				return *this;
			}

			template <typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, Any> && !std::is_same_v<std::decay_t<T>, entt::meta_any>>>
			static Any CreateRef(T&& value)
			{
				return Any(std::move(entt::forward_as_meta(std::forward<T>(value))));
			}

			inline Reflection::Type Type() const { return Reflection::Type(m_Value.type()); }

			const inline void *Data() const { return m_Value.data(); }
			inline void *Data() { return m_Value.data(); }

			template<typename... Args>
			inline Any Invoke(const uint32_t id, Args &&...args) const
			{
				m_Value.invoke(id, std::forward<Args>(args)...);
			}

			template <typename... Args>
			inline Any Invoke(const std::string& name, Args &&...args) const
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name);
				m_Value.invoke(hash, std::forward<Args>(args)...);
			}
			
			template<typename T>
			inline bool Set(const uint32_t id, T &&value)
			{
				return m_Value.set(id, std::move(value));
			}

			template <typename T>
			inline bool Set(const std::string& name, T &&value)
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name);
				return m_Value.set(hash, std::move(value));
			}

			inline Any Get(const uint32_t id)
			{
				return Any(m_Value.get(id));
			}

			inline Any Get(const std::string& name)
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name);
				return Any(m_Value.get(hash));
			}

			template<typename T>
			inline const T *TryCast() const
			{
				return m_Value.try_cast<T>();
			}

			template <typename T>
			inline T *TryCast()
			{
				return m_Value.try_cast<T>();
			}

			template<typename T>
			inline T Cast() const
			{
				return m_Value.cast<T>();
			}

			template<typename T>
			inline T Cast()
			{
				return m_Value.cast<T>();
			}

			template<typename T>
			inline Any AllowCast() const
			{
				return Any(m_Value.allow_cast());
			}

			template<typename T>
			inline bool AllowCast()
			{
				return m_Value.allow_cast();
			}

			template<typename T, typename... Args>
			inline void Emplace(Args &&... args)
			{
				return m_Value.emplace(std::forward<Args>(args)...);
			}

			inline void Reset()
			{
				m_Value.reset();
			}

			inline SequenceContainer AsSequenceContainer()
			{
				return SequenceContainer(m_Value.as_sequence_container());
			}

			inline SequenceContainer AsSequenceContainer() const
			{
				return SequenceContainer(m_Value.as_sequence_container());
			}

			inline AssociativeContainer AsAssociativeContainer()
			{
				return AssociativeContainer(m_Value.as_associative_container());
			}

			inline AssociativeContainer AsAssociativeContainer() const
			{
				return AssociativeContainer(m_Value.as_associative_container());
			}

			Any operator*() const { return m_Value.operator*(); }

			explicit operator bool() const { return m_Value.operator bool(); }

			bool operator==(const Any& other) const
			{
				return m_Value == other.m_Value;
			}

			template <typename T>
			inline bool IsType() const
			{
				return Reflection::Resolve<T>().Id() == m_Value.type().id();
			}

			inline bool IsType(const Reflection::Type& type) const
			{
				return type.Id() == m_Value.type().id();
			}

			operator entt::meta_any() { return m_Value; }
			operator Reflection::Handle() { return Reflection::Handle(m_Value); }

			Reflection::Handle Handle() { return Reflection::Handle(m_Value); }

			const bool Valid() const { return m_Value.operator bool(); }
			

			Any(entt::meta_any val) :
					m_Value(val) {}

		private:
			entt::meta_any m_Value;
		};

		struct AnyRef
		{
			friend struct Reflection;
			friend struct Reflection::Type;
			friend struct Data;
			friend struct Func;
			template <typename T>
			friend struct TypeFactory;

			AnyRef() = delete;

			AnyRef(const Any &) = delete;
			AnyRef(AnyRef &&) = delete;

			template <typename T>
			std::enable_if_t<!std::is_same_v<std::decay_t<T>, Any>, Any &>
			operator=(T &&value)
			{
				m_Value = value;
				return *this;
			}

			template <typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, Any> && !std::is_same_v<std::decay_t<T>, entt::meta_any>>>
			static AnyRef Create(T &&value)
			{
				return AnyRef(std::move(entt::forward_as_meta(std::forward<T>(value))));
			}

			inline Reflection::Type Type() const { return Reflection::Type(m_Value.type()); }

			const inline void *Data() const { return m_Value.data(); }
			inline void *Data() { return m_Value.data(); }

			template <typename... Args>
			inline Any Invoke(const uint32_t id, Args &&...args) const
			{
				m_Value.invoke(id, std::forward<Args>(args)...);
			}

			template <typename... Args>
			inline Any Invoke(const std::string &name, Args &&...args) const
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name);
				m_Value.invoke(hash, std::forward<Args>(args)...);
			}

			template <typename T>
			inline bool Set(const uint32_t id, T &&value)
			{
				return m_Value.set(id, std::move(value));
			}

			template <typename T>
			inline bool Set(const std::string &name, T &&value)
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name);
				return m_Value.set(hash, std::move(value));
			}

			inline Any Get(const uint32_t id)
			{
				return Any(m_Value.get(id));
			}

			inline Any Get(const std::string &name)
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name);
				return Any(m_Value.get(hash));
			}

			template <typename T>
			inline const T *TryCast() const
			{
				return m_Value.try_cast<T>();
			}

			template <typename T>
			inline T *TryCast()
			{
				return m_Value.try_cast<T>();
			}

			template <typename T>
			inline T Cast() const
			{
				return m_Value.cast<T>();
			}

			template <typename T>
			inline T Cast()
			{
				return m_Value.cast<T>();
			}

			template <typename T>
			inline bool AllowCast()
			{
				return m_Value.allow_cast();
			}

			template <typename T, typename... Args>
			inline void Emplace(Args &&...args)
			{
				return m_Value.emplace(std::forward<Args>(args)...);
			}

			inline void Reset()
			{
				m_Value.reset();
			}

			inline SequenceContainer AsSequenceContainer()
			{
				return SequenceContainer(m_Value.as_sequence_container());
			}

			inline SequenceContainer AsSequenceContainer() const
			{
				return SequenceContainer(m_Value.as_sequence_container());
			}

			inline AssociativeContainer AsAssociativeContainer()
			{
				return AssociativeContainer(m_Value.as_associative_container());
			}

			inline AssociativeContainer AsAssociativeContainer() const
			{
				return AssociativeContainer(m_Value.as_associative_container());
			}

			Any operator*() const { return m_Value.operator*(); }

			explicit operator bool() const { return m_Value.operator bool(); }

			bool operator==(const AnyRef &other) const
			{
				return m_Value == other.m_Value;
			}

			template <typename T>
			inline bool IsType() const
			{
				return Reflection::Resolve<T>().Id() == m_Value.type().id();
			}

			inline bool IsType(const Reflection::Type &type) const
			{
				return type.Id() == m_Value.type().id();
			}

			operator entt::meta_any() { return m_Value; }
			operator Reflection::Handle() { return Reflection::Handle(m_Value); }

			Reflection::Handle Handle() { return Reflection::Handle(m_Value); }

			const bool Valid() const { return m_Value.operator bool(); }

		private:
			AnyRef(entt::meta_any val) :
					m_Value(std::move(val)) {}
			entt::meta_any m_Value;
		};



		template <typename T>
		struct TypeFactory
		{
			friend struct Reflection;

			template <typename Base>
			TypeFactory<T> &Base()
			{
				static_assert(!std::is_same<Base, T>(), "Warning, class cannot be base class of itself!");
				m_Factory.base<Base>();
				return *this;
			}

			template <typename To>
			TypeFactory<T> &Conv()
			{
				m_Factory.conv<To>();
				return *this;
			}

			template <auto Ctor>
			TypeFactory<T> &Ctor()
			{
				m_Factory.ctor<Ctor>();
				return *this;
			}

			template <auto Data>
			TypeFactory<T> &Data(const std::string& name, bool serialize = true)
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name).m_Hash;
				TypeData::GetTypeDataNames()[m_Hash][hash] = name;
				TypeData::GetTypeDataSerialize()[m_Hash][hash] = serialize;
				m_Factory.data<Data>(hash);
				return *this;
			}

			template <auto Setter, auto Getter>
			TypeFactory<T> &Data(const std::string& name, bool serialize = true)
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name).m_Hash;
				TypeData::GetTypeDataNames()[m_Hash][hash] = name;
				TypeData::GetTypeDataSerialize()[m_Hash][hash] = serialize;
				m_Factory.data<Setter, Getter>(hash);
				return *this;
			}

			template <auto Data>
			TypeFactory<T> &Data(const std::string &name, std::function<bool(Reflection::Any*)> callback)
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name).m_Hash;
				TypeData::GetTypeDataNames()[m_Hash][hash] = name;
				TypeData::GetTypeDataSerializeCallbacks()[m_Hash][hash] = callback;
				m_Factory.data<Data>(hash);
				return *this;
			}

			template <auto Setter, auto Getter>
			TypeFactory<T> &Data(const std::string &name, std::function<bool(Reflection::Any*)> callback)
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name).m_Hash;
				TypeData::GetTypeDataNames()[m_Hash][hash] = name;
				TypeData::GetTypeDataSerializeCallbacks()[m_Hash][hash] = callback;
				m_Factory.data<Setter, Getter>(hash);
				return *this;
			}

			template <auto Deconstructor>
			TypeFactory<T> &Dtor()
			{
				m_Factory.dtor<Deconstructor>();
				return *this;
			}

			template <auto Fn>
			TypeFactory<T> &Func(const std::string& name)
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name).m_Hash;
				TypeData::GetTypeFunctionNames()[m_Hash][hash] = name;
				m_Factory.func<Fn>(hash);
				return *this;
			}

			template <typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
			TypeFactory<T> &Require(const std::string& componentName)
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(componentName).m_Hash;
				//std::cout << TypeData::GetTypeNames()[m_Hash] << " requires " << componentName << " [" << hash << "]\n";
				TypeData::GetTypeRequirements()[m_Hash].push_back(hash);
				return *this;
			}

		private:

			TypeFactory(const std::string &name) :
					m_Hash(BasicHash<uint32_t>::Hash(name).m_Hash), m_Factory(entt::meta<T>().type(BasicHash<uint32_t>::Hash(name).m_Hash))
			{
			}

			entt::meta_factory<T, T> m_Factory;
			uint32_t m_Hash;
		};

		template <typename T, typename InternalType, typename InternalNodeType = InternalType::node_type>
		struct TypeRange
		{
			friend struct Type;
			friend struct Reflection;

			//This iterator wraps the underlying entt iterator
			struct RangeIterator
			{
				using difference_type = std::ptrdiff_t;
				using value_type = T;
				using pointer = void;
				using reference = value_type;
				using iterator_category = std::input_iterator_tag;


				RangeIterator() = default;
				RangeIterator(entt::meta_range<InternalType, InternalNodeType>::iterator iterator, uint32_t parent) :
						m_Iterator(iterator), m_Parent(parent)
				{
				}

				RangeIterator &operator++()
				{
					m_Iterator.operator++();
					return *this;
				}

				RangeIterator operator++(int)
				{
					m_Iterator.operator++();
					return *this;
				}

				reference operator*() const
				{
					auto type = m_Iterator.operator*();
					return T(type, m_Parent);
				}

				bool operator==(const RangeIterator &other) const
				{
					return other.m_Iterator == m_Iterator;
				}

				bool operator!=(const RangeIterator &other) const
				{
					return !(m_Iterator == other.m_Iterator);
				}

			private:
				entt::meta_range<InternalType, InternalNodeType>::iterator m_Iterator;
				uint32_t m_Parent;
			};

			TypeRange(entt::meta_range<InternalType, InternalNodeType> type, uint32_t parent) :
					m_Type(type), m_Parent(parent) {}

			const inline std::vector<T> ToVector() 
			{
				std::vector<T> vals;
				for (auto val : *this)
				{
					vals.push_back(val);
				}
				return vals;
			}

			RangeIterator begin() const
			{
				return RangeIterator(m_Type.begin(), m_Parent);
			}

			RangeIterator end() const
			{
				return RangeIterator{};
			}

			size_t Count() const
			{
				//I don't know if there's an easier way to do this, and i'm too lazy to find out.
				size_t count = 0;
				for (auto val : *this)
				{
					count++;
				}
				return count;
			}

		private:
			entt::meta_range<InternalType, InternalNodeType> m_Type;
			uint32_t m_Parent;
		};

		struct Data
		{
			friend struct Reflection;
			friend struct Reflection::Type;

			Data() = delete;

			const inline bool Valid() const { return m_Data.operator bool(); }
			operator bool() const { return m_Data.operator bool(); }
			const inline uint32_t Id() const { return m_Data.id(); }

			const inline std::string GetName() const
			{
				if (TypeData::GetTypeDataNames().contains(m_Parent) && TypeData::GetTypeDataNames()[m_Parent].contains(m_Data.id()))
					return TypeData::GetTypeDataNames()[m_Parent][m_Data.id()];
				return std::string();
			}

			const inline bool Serialized(Reflection::Any* handle = nullptr)
			{
				if (handle && TypeData::GetTypeDataSerializeCallbacks().contains(m_Parent) && TypeData::GetTypeDataSerializeCallbacks()[m_Parent].contains(m_Data.id())) 
					return TypeData::GetTypeDataSerializeCallbacks()[m_Parent][m_Data.id()](handle);
				else if(TypeData::GetTypeDataSerialize().contains(m_Parent) && TypeData::GetTypeDataSerialize()[m_Parent].contains(m_Data.id())) 
					return TypeData::GetTypeDataSerialize()[m_Parent][m_Data.id()];
				return false;
			}

			const inline bool IsConst() const { return m_Data.is_const(); }
			const inline bool IsStatic() const { return m_Data.is_static(); }
			const inline Reflection::Type Type() const { return Reflection::Type(m_Data.type()); }
			const inline Reflection::Type Parent() const { return Reflection::Resolve(m_Parent); }
			
			template <typename T>
			bool Set(Handle& handle, T&& value) const
			{
				return m_Data.set<T>(std::move(handle.m_Handle), std::forward<T>(value));
			}

			Any Get(Handle handle)
			{
				return m_Data.get(std::move(handle.m_Handle));
			}

			bool SetFromAny(Handle handle, Any value)
			{
				return m_Data.set(std::move(handle.m_Handle), value.m_Value);
			}

		private:

			Data(entt::meta_data data, uint32_t parent) :
					m_Data(data), m_Parent(parent) {}

			entt::meta_data m_Data;
			uint32_t m_Parent;
		};

		struct Function
		{
			friend struct Reflection;
			friend struct Reflection::Type;

			Function() = delete;

			const inline bool Valid() const { return m_Func.operator bool(); }
			operator bool() const { return m_Func.operator bool(); }
			const inline uint32_t Id() const { return m_Func.id(); }

			const inline std::string GetName() const
			{
				if (TypeData::GetTypeFunctionNames().contains(m_Parent) && TypeData::GetTypeFunctionNames()[m_Parent].contains(m_Func.id()))
					return TypeData::GetTypeFunctionNames()[m_Parent][m_Func.id()];
				return std::string();
			}

			const inline bool IsConst() const { return m_Func.is_const(); }
			const inline bool IsStatic() const { return m_Func.is_static(); }
			const inline Reflection::Type Parent() const { return Reflection::Resolve(m_Parent); }

			const inline size_t Arity() const { return m_Func.arity(); }
			const inline Reflection::Type ReturnType() const { return Reflection::Type(m_Func.ret()); }
			const inline Reflection::Type ArgType(size_t index) const { return Reflection::Type(m_Func.arg(index)); }

			inline Any Invoke(Handle handle, Any * const args, const size_t size) const
			{
				return m_Func.invoke(std::move(handle.m_Handle), args, size);
			}

			template<typename... Args>
			inline Any Invoke(Handle handle, Args &&... args) const
			{
				Reflection::Any arguments[sizeof...(Args) + 1u]{ std::forward<Args>(args)... };
				return m_Func.invoke(arguments, sizeof...(Args));
			}

		private:
			Function(entt::meta_func func, uint32_t parent) :
					m_Func(func), m_Parent(parent) {}
			entt::meta_func m_Func;
			uint32_t m_Parent;
		};

		struct Type
		{
			friend struct Reflection;
			friend struct Reflection::Data;
			friend struct TypeRange<Type, entt::meta_type, entt::internal::meta_base_node>;

			Type(const Type &) = default;

			const inline bool Valid() const { return m_Type.operator bool(); }
			operator bool() const { return m_Type.operator bool(); }
			
			const inline uint32_t Id() const { return m_Type.id(); }

			const inline std::string GetName() const 
			{
				if (TypeData::GetTypeNames().contains(m_Type.id()))
					return TypeData::GetTypeNames()[m_Type.id()];
				return std::string();
			}

			const inline bool IsIntegral() const { return m_Type.is_integral(); }
			const inline bool IsArray() const { return m_Type.is_array(); }
			const inline bool IsEnum() const { return m_Type.is_enum(); }
			const inline bool IsClass() const { return m_Type.is_class(); }
			const inline bool IsPointer() const { return m_Type.is_pointer(); }
			const inline bool IsPointerLike() const { return m_Type.is_pointer_like(); }
			const inline bool IsSequenceContainer() const { return m_Type.is_sequence_container(); }
			const inline bool IsAssociativeContainer() const { return m_Type.is_associative_container(); }
			const inline bool IsTemplateSpecialization() const { return m_Type.is_template_specialization(); }
			const inline bool IsAbstract() const
			{ 
				if (TypeData::GetTypeAbstract().contains(m_Type.id()))
					return TypeData::GetTypeAbstract()[m_Type.id()];
				return true;
			}

			/// @brief Gets a list of top-level base classes
			/// @return 
			inline TypeRange<Type, entt::meta_type, entt::internal::meta_base_node> TopLevelBases()  const
			{
				return TypeRange<Type, entt::meta_type, entt::internal::meta_base_node>(m_Type.base(), m_Type.id());
			}

			/// @brief Gets a list of all base classes (breadth-first recursive search)
			/// For now, this has a hard limit to attempt to mitigate potential circular references.
			/// TODO: Prevent base classes from being registered if it creates a circular reference.
			/// @return List of all base classes for this type
			inline std::vector<Type> Base() const
			{
				std::vector<Type> bases;
				InternalRecurseBase(&bases, this);
				return bases;
			}

		private:

			const uint8_t BaseSearchMaxDepth = 64;

			void InternalRecurseBase(std::vector<Type> *bases_list, const Type *t) const
			{
				InternalRecurseBase(bases_list, t, 0);
			}
			
			void InternalRecurseBase(std::vector<Type>* bases_list, const Type *t, uint8_t depth) const
			{
				depth++;
				for (auto base : t->TopLevelBases())
				{
					bases_list->push_back(base);
					if (depth <= BaseSearchMaxDepth)
						InternalRecurseBase(bases_list, &base);
				}
				depth--;
			}

		public:
			/// @brief Gets the top-level base class of this type
			/// @param name Name of the base class
			/// @return The type of the base class
			inline Type Base(const std::string &name) { return Type(m_Type.base(BasicHash<uint32_t>::Hash(name))); }
			inline Type Base(uint32_t id) { return Type(m_Type.base(id)); }

			template <typename... Args>
			inline Reflection::Any Construct(Args &&... args) const
			{
				if (sizeof...(Args) == 0)
					return m_Type.construct();
				entt::meta_any arguments[sizeof...(Args) + 1u]{ std::forward<Args>(args)... };
				return m_Type.construct(arguments, sizeof...(Args));
			}

			inline TypeRange<Reflection::Data, entt::meta_data> TopLevelData() const
			{
				return TypeRange<Reflection::Data, entt::meta_data>(m_Type.data(), m_Type.id());
			}

			inline std::vector<Reflection::Data> Data() const
			{
				std::vector<Reflection::Data> all_data;
				for (Reflection::Data data : TopLevelData())
				{
					all_data.push_back(data);
				}
				for (Type base : Base())
				{
					for (Reflection::Data data : base.TopLevelData())
					{
						all_data.push_back(data);
					}
				}
				return all_data;
			}

			inline Reflection::Data Data(const std::string& name) const
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name);
				auto data = Reflection::Data(m_Type.data(hash), m_Type.id());
				if (!data.Valid())
				{
					for (auto& base : Base())
					{
						auto base_data = base.Data(hash);
						if (base_data.Valid())
							return base_data;
					}
				}
				return data;
			}

			inline Reflection::Data Data(const uint32_t id) const
			{
				auto data = Reflection::Data(m_Type.data(id), m_Type.id());
				if (!data.Valid())
				{
					for (auto& base : Base())
					{
						auto base_data = base.Data(id);
						if (base_data.Valid())
							return base_data;
					}
				}
				return data;
			}

			inline TypeRange<Reflection::Function, entt::meta_func> TopLevelFunctions() const
			{
				return TypeRange<Reflection::Function, entt::meta_func>(m_Type.func(), m_Type.id());
			}

			inline std::vector<Reflection::Function> Function() const
			{
				std::vector<Reflection::Function> all_functions;
				for (Reflection::Function func : TopLevelFunctions())
				{
					all_functions.push_back(func);
				}
				for (Type base : Base())
				{
					for (Reflection::Function func : base.TopLevelFunctions())
					{
						all_functions.push_back(func);
					}
				}
				return all_functions;
			}

			inline Reflection::Function Function(const std::string& name) const
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name);
				auto func = Reflection::Function(m_Type.func(hash), m_Type.id());
				if (!func.Valid())
				{
					for (auto &base : Base())
					{
						auto base_func = base.Function(hash);
						if (base_func.Valid())
							return base_func;
					}
				}
				return func;
			}

			inline Reflection::Function Function(const uint32_t id) const
			{
				auto func = Reflection::Function(m_Type.func(id), m_Type.id());
				if (!func.Valid())
				{
					for (auto &base : Base())
					{
						auto base_func = base.Function(id);
						if (base_func.Valid())
							return base_func;
					}
				}
				return func;
			}

			inline Any Get(const std::string &name, Handle instance) const
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name);
				return m_Type.get(hash, std::move(std::move(instance.m_Handle)));
			}

			inline Any Get(const uint32_t id, Handle instance) const
			{
				return m_Type.get(id, std::move(std::move(instance.m_Handle)));
			}

			template <typename T>
			inline T Get(const std::string &name, Handle instance) const
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name);
				return m_Type.get(hash, std::move(std::move(instance.m_Handle))).cast<T>();
			}

			template <typename T>
			inline T Get(const uint32_t id, Handle instance) const
			{
				return m_Type.get(id, std::move(std::move(instance.m_Handle))).cast<T>();
			}

			template <typename T>
			inline bool Set(const std::string& name, Handle instance, T&& value) const
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name);
				return m_Type.set(hash, std::move(std::move(instance.m_Handle)), std::forward<T>(value));
			}

			template <typename T>
			inline bool Set(const uint32_t id, Handle instance, T &&value) const
			{
				return m_Type.set(id, std::move(std::move(instance.m_Handle)), std::forward<T>(value));
			}

			inline bool Set(const std::string& name, Handle instance, Any value)
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name);
				return m_Type.set(hash, std::move(std::move(instance.m_Handle)), value);
			}

			inline bool Set(const uint32_t id, Handle instance, Any value)
			{
				return m_Type.set(id, std::move(std::move(instance.m_Handle)), value);
			}

			inline Reflection::Any Invoke(const std::string& name, Handle instance, Reflection::Any *const args, const size_t count) const 
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name);
				return m_Type.invoke(hash, std::move(std::move(instance.m_Handle)), args, count);
			}

			template <typename... Args>
			inline Reflection::Any Invoke(const std::string &name, Handle instance, Args &&...args) const
			{
				uint32_t hash = BasicHash<uint32_t>::Hash(name);
				Reflection::Any arguments[sizeof...(Args) + 1u]{ std::forward<Args>(args)... };
				return m_Type.invoke(hash, std::move(std::move(instance.m_Handle)), arguments, sizeof...(Args));
			}

			inline Reflection::Any Invoke(const uint32_t id, Handle instance, Reflection::Any *const args, const size_t count) const
			{
				return m_Type.invoke(id, std::move(std::move(instance.m_Handle)), args, count);
			}

			template <typename... Args>
			inline Reflection::Any Invoke(const uint32_t id, Handle instance, Args &&...args) const
			{
				Reflection::Any arguments[sizeof...(Args) + 1u]{ std::forward<Args>(args)... };
				return m_Type.invoke(id, std::move(std::move(instance.m_Handle)), arguments, sizeof...(Args));
			}

			inline Reflection::Type RemovePointer() { return Reflection::Type(m_Type.remove_pointer()); }

			inline size_t Size() const { return m_Type.size_of(); }

			inline Reflection::Type TemplateArg(size_t index) const { return Reflection::Type(m_Type.template_arg(index)); }

			inline size_t TemplateArity() const { return m_Type.template_arity(); }

			inline Reflection::Type TemplateType() const { return Reflection::Type(m_Type.template_type()); }

			template <typename T>
			inline bool Is()
			{
				auto type = Reflection::Resolve<T>();
				if (!type.Valid())
					return false;
				return Id() == type.Id();
			}


		private:

			Type(entt::meta_type type) :
					m_Type(type)
			{
			}

			Type(entt::meta_type type, uint32_t parent) :
					m_Type(type)
			{
			}

			Type() = default;

			entt::meta_type m_Type;
		};


		struct Handle
		{
			friend struct Reflection;
			friend struct Data;
			friend struct Type;
			friend struct Function;

			Handle(const Handle &other) = delete;
			Handle(Handle &&) = default;
			Handle &operator=(const Handle &) = delete;
			Handle &operator=(Handle &&) = default;

			template <typename Type, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Type>, Handle>>>
			Handle(Type& value) :
					m_Type(Reflection::Resolve<Type>()), m_Handle(value)
			{
			}

			explicit operator bool() const
			{
				return m_Handle.operator bool();
			}

			//Any operator->()
			//{
			//	return Any(*m_Handle.operator->()).AsRef();
			//}
			//
			//const Any operator->() const
			//{
			//	return Any(*m_Handle.operator->()).AsRef();
			//}

			inline Reflection::Type Type() { return m_Type; }
			const inline Reflection::Type Type() const { return m_Type; }

		private:
			Handle() = default;
			

			entt::meta_handle m_Handle;
			Reflection::Type m_Type;
		};


	private:
		static inline bool s_InitializeMaps = true;

	public:


		template <typename T>
		static TypeFactory<T> Register(const std::string& name)
		{
			uint32_t hash = BasicHash<uint32_t>::Hash(name);
			bool is_abstract = std::is_abstract_v<T>; 
			TypeData::GetTypeAbstract()[hash] = is_abstract;
			TypeData::GetTypeNames()[hash] = name;
			TypeData::GetTypeDataNames()[hash] = std::unordered_map<uint32_t, std::string>();
			TypeData::GetTypeFunctionNames()[hash] = std::unordered_map<uint32_t, std::string>();
			TypeData::GetTypeDataSerialize()[hash] = std::unordered_map<uint32_t, bool>();
			TypeData::GetTypeDataSerializeCallbacks()[hash] = std::unordered_map<uint32_t, std::function<bool(Reflection::Any *)>>();
			TypeData::GetTypeRequirements()[hash] = std::vector<uint32_t>();
			return TypeFactory<T>(name);
		}

		template <typename T>
		static inline Type Resolve()
		{
			return Type(entt::resolve<T>());
		}

		template <typename T>
		static inline uint32_t ResolveID()
		{
			return entt::resolve<T>().id();
		}

		static inline TypeRange<Type, entt::meta_type> Resolve()
		{
			return TypeRange<Type, entt::meta_type>(entt::resolve(), 0);
		}

		static inline Type Resolve(const std::string& name)
		{
			uint32_t hash = BasicHash<uint32_t>::Hash(name).m_Hash;
			return Type(entt::resolve(hash));
		}

		static inline Type Resolve(uint32_t hash)
		{
			return Type(entt::resolve(hash));
		}

		template <typename T>
		static inline Any ForwardAsMeta(T&& type)
		{
			return Any(entt::forward_as_meta(std::forward<Type>(type)));
		}

		//An instance of this struct is automatically declared statically inlined into the reflected class
		//This class is designed to automatically register the type of the reflected class, and call the RegisterType()
		//function. All of the collection reflection information is stored as local static variables within
		//static functions to ensure they are statically initialized BEFORE the RegisterType function is called.
		//
		//This entire thing is definitely hacky and only tested on the MSVC compiler, however it allows for the reflection
		//registration function to be called automatically.
		struct TypeData
		{
			template <class T>
			static TypeData Create()
			{
				TypeData data;
				T::RegisterType();
				Type t = Reflection::Resolve<T>();
				if constexpr (!std::is_abstract<T>::value)
				{
					if constexpr (!std::is_same<T, Component>::value && std::is_base_of<Component, T>::value)
						Chroma::ComponentRegistry::RegisterComponent<T>(t.Id());
				}
				return data;
			}

			static std::unordered_map<uint32_t, std::string> &GetTypeNames()
			{
				static std::unordered_map<uint32_t, std::string> s_TypeNames;
				return s_TypeNames;
			}

			static std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::string>> &GetTypeDataNames()
			{
				static std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::string>> s_TypeDataNames;
				return s_TypeDataNames;
			}

			static std::unordered_map<uint32_t, std::unordered_map<uint32_t, bool>> &GetTypeDataSerialize()
			{
				static std::unordered_map<uint32_t, std::unordered_map<uint32_t, bool>> s_TypeDataSerialize;
				return s_TypeDataSerialize;
			}

			static std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::function<bool(Reflection::Any*)>>> &GetTypeDataSerializeCallbacks()
			{
				static std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::function<bool(Reflection::Any *)>>> s_TypeDataSerializeCallbacks;
				return s_TypeDataSerializeCallbacks;
			}

			static std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::string>> &GetTypeFunctionNames()
			{
				static std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::string>> s_TypeFunctionNames;
				return s_TypeFunctionNames;
			}

			static std::unordered_map<uint32_t, bool> &GetTypeAbstract()
			{
				static std::unordered_map<uint32_t, bool> s_TypeAbstract;
				return s_TypeAbstract;
			}

			static std::unordered_map<uint32_t, std::vector<uint32_t>> &GetTypeRequirements()
			{
				static std::unordered_map<uint32_t, std::vector<uint32_t>> s_TypeRequirements;
				return s_TypeRequirements;
			}

		private:
			TypeData()
			{
				GetTypeNames();
				GetTypeDataNames();
				GetTypeFunctionNames();
				GetTypeDataSerialize();
				GetTypeDataSerializeCallbacks();
				GetTypeAbstract();
				GetTypeRequirements();
			}
		};


		/// <summary>
		/// This function will initialize all standard serialization types into the reflection system.
		/// </summary>
		static void InitializeDataTypes();

		/// <summary>
		/// Registers a single data type into the reflection system. This function does not decompose the type into
		/// properties/functions.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		template <typename T>
		static void RegisterYAMLSerializeFunctions(std::function<void(YAML::Emitter &, Reflection::Any &)> serialize_function, std::function<Reflection::Any(YAML::Node &)> deserialize_function)
		{
			auto type = Reflection::Resolve<T>();
			uint32_t hash = type.Id();
			
			s_SerializeYAMLFunctions[hash] = serialize_function;
			s_DeserializeYAMLFunctions[hash] = deserialize_function;
		}

		template <typename T>
		static void RegisterBinarySerializeFunctions(std::function<void(File &, Reflection::Any &)> serialize_function, std::function<Reflection::Any(File &)> deserialize_function)
		{
			auto type = Reflection::Resolve<T>();
			uint32_t hash = type.Id();
			
			s_SerializeBinaryFunctions[hash] = serialize_function;
			s_DeserializeBinaryFunctions[hash] = deserialize_function;
		}


		static void SerializeObjectYAML(YAML::Emitter &out, Reflection::Any& object) { SerializeObjectYAML(out, object, 0); }
		static void SerliazeObjectBinary(File &file, Reflection::Any& object) { SerliazeObjectBinary(file, object, 0); }
		static void DeserializeObjectYAML(Any& object, Type type, YAML::Node &node) { DeserializeObjectYAML(object, type, node, 0); }
		static void DeserializeObjectBinary(Any& object, Type type, File &file) { DeserializeObjectBinary(object, type, file, 0); }

	private:

		static void SerializeObjectYAML(YAML::Emitter &out, Reflection::Any& object, int depth);
		static void SerliazeObjectBinary(File &file, Reflection::Any& object, int depth);
		static void DeserializeObjectYAML(Any& object, Type type, YAML::Node &node, int depth);
		static void DeserializeObjectBinary(Any& object, Type type, File &file, int depth);

		static std::unordered_map<uint32_t, std::function<void(YAML::Emitter &, Reflection::Any &)>> s_SerializeYAMLFunctions;
		static std::unordered_map<uint32_t, std::function<Reflection::Any(YAML::Node &)>> s_DeserializeYAMLFunctions;
		static std::unordered_map<uint32_t, std::function<void(File &, Reflection::Any &)>> s_SerializeBinaryFunctions;
		static std::unordered_map<uint32_t, std::function<Reflection::Any(File &)>> s_DeserializeBinaryFunctions;
	};




#ifndef CHROMA_REFLECT
	//Registers a class as a reflectable type. Requires the class to implement: Reflection::TypeFactory<T> RegisterType()
	//Note: If inheritance proves an issue, define in a private scope
	#define CHROMA_REFLECT(typeName) \
	static Reflection::TypeData<typeName> TypeInfo; \
	static Reflection::TypeFactory<typeName> RegisterType();
#endif

} //namespace Chroma