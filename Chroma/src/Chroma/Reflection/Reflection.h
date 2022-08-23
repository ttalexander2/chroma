#pragma once

#define ENTT_ID_TYPE std::size_t
#include <entt.hpp>
#include <string>
#include <type_traits>
#include <concepts>

#include <Chroma/Utilities/StringHash.h>



#include <Chroma/IO/File.h>
#include <yaml-cpp/yaml.h>

namespace Chroma
{

	class Component;
	class SpriteRenderer;


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

			inline std::pair<Iterator, bool> Insert(Iterator it, Any val)
			{
				auto [its, result] = m_Container.insert(it.m_Iterator, val.m_Value);
				return std::make_pair(Iterator(std::move(its)), result);
			}

			inline std::pair<Iterator, bool> Erase(Iterator it)
			{
				auto [its, result] = m_Container.erase(it.m_Iterator);
				return std::make_pair(Iterator(std::move(its)), result);
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

			template <typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, Any>>>
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
			}

			Any &operator=(Any &&other)
			{
				m_Value = other.m_Value;
			}

			template<typename T>
			std::enable_if_t<!std::is_same_v<std::decay_t<T>, Any>, Any &>
			operator=(T &&value)
			{
				m_Value = value;
			}

			inline Reflection::Type Type() const { return Reflection::Type(m_Value.type()); }

			const inline void *Data() const { return m_Value.data(); }
			inline void *Data() { return m_Value.data(); }

			template<typename... Args>
			inline Any Invoke(const size_t id, Args &&...args) const
			{
				m_Value.invoke(id, std::forward<Args>(args)...);
			}

			template <typename... Args>
			inline Any Invoke(const std::string& name, Args &&...args) const
			{
				size_t hash = StringHash::Hash(name);
				m_Value.invoke(hash, std::forward<Args>(args)...);
			}
			
			template<typename T>
			inline bool Set(const size_t id, T &&value)
			{
				return m_Value.set(id, std::move(value));
			}

			template <typename T>
			inline bool Set(const std::string& name, T &&value)
			{
				size_t hash = StringHash::Hash(name);
				return m_Value.set(hash, std::move(value));
			}

			inline Any Get(const size_t id)
			{
				return Any(m_Value.get(id));
			}

			inline Any Get(const std::string& name)
			{
				size_t hash = StringHash::Hash(name);
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

			Any AsRef()
			{
				return m_Value.as_ref();
			}

			Any AsRef() const
			{
				return m_Value.as_ref();
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
			
		private:
			Any(entt::meta_any val) :
					m_Value(val) {}

			entt::meta_any m_Value;
		};

		template <typename T>
		struct TypeFactory
		{
			TypeFactory(const std::string &name) :
					m_Factory(entt::meta<T>().type(StringHash::Hash(name).m_Hash))
			{
			}


			template <typename Base>
			TypeFactory<T> &Base()
			{
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
				size_t hash = StringHash::Hash(name).m_Hash;
				m_Factory.data<Data>(hash);
				return *this;
			}

			template <auto Setter, auto Getter>
			TypeFactory<T> &Data(const std::string& name, bool serialize = true)
			{
				size_t hash = StringHash::Hash(name).m_Hash;
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
				size_t hash = StringHash::Hash(name).m_Hash;
				m_Factory.func<Fn>(hash);
				return *this;
			}

		private:
			entt::meta_factory<T, T> m_Factory;
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
				RangeIterator(entt::meta_range<InternalType, InternalNodeType>::iterator iterator) :
						m_Iterator(iterator)
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
					return T(type);
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

			};

			TypeRange(entt::meta_range<InternalType, InternalNodeType> type) :
					m_Type(type) {}

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
				return RangeIterator(m_Type.begin());
			}

			RangeIterator end() const
			{
				return RangeIterator{};
			}

		private:
			entt::meta_range<InternalType, InternalNodeType> m_Type;
		};

		struct Constructor
		{
			friend class Reflection;
			friend class Type;

			const inline bool Valid() { return m_Constructor.operator bool(); }
			operator bool() const { return m_Constructor.operator bool(); }

			inline Type Parent() const { return Type(m_Constructor.parent()); }
			inline size_t Arity() const { return m_Constructor.arity(); }
			inline Type Arg(size_t index) const { return m_Constructor.arg(index); }

			inline Reflection::Any Invoke(Reflection::Any *const args, const size_t count) const { return m_Constructor.invoke(args, count); }
			
			template <typename... Args>
			inline Reflection::Any Invoke(Args &&... args) const
			{
				Reflection::Any arguments[sizeof...(Args) + 1u]{ std::forward<Args>(args)... };
				return m_Constructor.invoke(arguments, sizeof...(Args));
			}

		private:
			entt::meta_ctor m_Constructor;
		};

		struct Handle
		{
			friend class Reflection;
			friend class Data;
			friend class Type;
			friend class Function;

			Handle() = default;
			Handle(const Handle &) = delete;
			Handle(Handle &&) = default;
			Handle &operator=(const Handle &) = delete;
			Handle &operator=(Handle &&) = default;

			template <typename Type, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Type>, Handle>>>
			Handle(Type &value) :
					m_Handle(entt::meta_handle(value))
			{
			}

			explicit operator bool() const
			{
				return m_Handle.operator bool();
			}

			Any operator->()
			{
				return Any(*m_Handle.operator->()).AsRef();
			}

			const Any operator->() const
			{
				return Any(*m_Handle.operator->()).AsRef();
			}

		
			private:
				Handle(entt::meta_handle handle) :
						m_Handle(std::move(handle)) {}

				entt::meta_handle m_Handle;
		};

		struct Data
		{
			friend class Reflection;
			friend class Reflection::Type;

			Data() = delete;

			const inline bool Valid() const { return m_Data.operator bool(); }
			operator bool() const { return m_Data.operator bool(); }
			const inline uint32_t Id() const { return m_Data.id(); }

			const inline std::string GetName() const
			{
				if (s_TypeDataNames.contains(m_Data.parent().id()) && s_TypeDataNames[m_Data.parent().id()].contains(m_Data.id()))
					return s_TypeDataNames[m_Data.parent().id()][m_Data.id()];
				return std::string();
			}

			const inline bool IsConst() const { return m_Data.is_const(); }
			const inline bool IsStatic() const { return m_Data.is_static(); }
			const inline Reflection::Type Type() const { return Reflection::Type(m_Data.type()); }
			const inline Reflection::Type Parent() const { return Reflection::Type(m_Data.parent()); }
			
			template <typename T>
			bool Set(Handle handle, T&& value) const
			{
				return m_Data.set<T>(std::move(handle.m_Handle), std::forward<T>(value));
			}

			Any Get(Handle handle) const
			{
				return m_Data.get(std::move(handle.m_Handle));
			}
		private:

			Data(entt::meta_data data) :
					m_Data(data) {}

			entt::meta_data m_Data;
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
				if (s_TypeFunctionNames.contains(m_Func.parent().id()) && s_TypeFunctionNames[m_Func.parent().id()].contains(m_Func.id()))
					return s_TypeFunctionNames[m_Func.parent().id()][m_Func.id()];
				return std::string();
			}

			const inline bool IsConst() const { return m_Func.is_const(); }
			const inline bool IsStatic() const { return m_Func.is_static(); }
			const inline Reflection::Type Parent() const { return Reflection::Type(m_Func.parent()); }

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
			Function(entt::meta_func func) :
					m_Func(func) {}
			entt::meta_func m_Func;
		};


		struct Type
		{
			friend class Reflection;
			friend class Reflection::Data;
			friend class TypeRange<Type, entt::meta_type, entt::internal::meta_base_node>;

			const inline bool Valid() { return m_Type.operator bool(); }
			operator bool() const { return m_Type.operator bool(); }
			
			const inline uint32_t Id() const { return m_Type.id(); }

			const inline std::string GetName()
			{
				if (s_TypeNames.contains(m_Type.id()))
					return s_TypeNames[m_Type.id()];
				return std::string();
			}

			const inline bool IsVoid() { return m_Type.is_void(); }
			const inline bool IsIntegral() { return m_Type.is_integral(); }
			const inline bool IsFloatingPoint() { return m_Type.is_floating_point(); }
			const inline bool IsArray() { return m_Type.is_array(); }
			const inline bool IsEnum() { return m_Type.is_enum(); }
			const inline bool IsUnion() { return m_Type.is_union(); }
			const inline bool IsClass() { return m_Type.is_class(); }
			const inline bool IsPointer() { return m_Type.is_pointer(); }
			const inline bool IsFunctionPointer() { return m_Type.is_function_pointer(); }
			const inline bool IsMemberObjectPointer() { return m_Type.is_member_object_pointer(); }
			const inline bool IsMemberFunctionPointer() { return m_Type.is_member_function_pointer(); }
			const inline bool IsPointerLike() { return m_Type.is_pointer_like(); }
			const inline bool IsSequenceContainer() { return m_Type.is_sequence_container(); }
			const inline bool IsAssociativeContainer() { return m_Type.is_associative_container(); }
			const inline bool IsTemplateSpecialization() { return m_Type.is_template_specialization(); }


			inline TypeRange<Type, entt::meta_type, entt::internal::meta_base_node> Base() 
			{
				return TypeRange<Type, entt::meta_type, entt::internal::meta_base_node>(m_Type.base());
			}
			inline Type Base(const std::string &name) { return Type(m_Type.base(StringHash::Hash(name))); }

			template <typename... Args>
			inline Reflection::Any Construct(Args &&... args) const
			{
				Reflection::Any arguments[sizeof...(Args) + 1u]{ std::forward<Args>(args)... };
				return m_Type.construct(arguments, sizeof...(Args));
			}

			inline TypeRange<Constructor, entt::meta_ctor> Ctor() const
			{
				return TypeRange<Constructor, entt::meta_ctor>(m_Type.ctor());
			}

			template <typename... Args>
			inline Constructor Ctor() const
			{
				return m_Type.ctor<Args>();
			}

			inline TypeRange<Reflection::Data, entt::meta_data> Data() const
			{
				return TypeRange<Reflection::Data, entt::meta_data>(m_Type.data());
			}

			inline Reflection::Data Data(const std::string& name) const
			{
				size_t hash = StringHash::Hash(name);
				return Reflection::Data(m_Type.data(hash));
			}

			inline Reflection::Data Data(const size_t id) const
			{
				return Reflection::Data(m_Type.data(id));
			}
			
			inline const size_t Extent(size_t dim = {}) const
			{
				return m_Type.extent(dim);
			}

			inline TypeRange<Reflection::Function, entt::meta_func> Function() const
			{
				return TypeRange<Reflection::Function, entt::meta_func>(m_Type.func());
			}

			inline Reflection::Function Function(const std::string& name) const
			{
				size_t hash = StringHash::Hash(name);
				return Reflection::Function(m_Type.func(hash));
			}

			inline Reflection::Function Function(const size_t id) const
			{
				return Reflection::Function(m_Type.func(id));
			}

			inline Any Get(const std::string &name, Handle instance) const
			{
				size_t hash = StringHash::Hash(name);
				return m_Type.get(hash, std::move(instance.m_Handle));
			}

			inline Any Get(const size_t id, Handle instance) const
			{
				return m_Type.get(id, std::move(instance.m_Handle));
			}

			template <typename T>
			inline T Get(const std::string &name, Handle instance) const
			{
				size_t hash = StringHash::Hash(name);
				return m_Type.get(hash, std::move(instance.m_Handle)).cast<T>();
			}

			template <typename T>
			inline T Get(const size_t id, Handle instance) const
			{
				return m_Type.get(id, std::move(instance.m_Handle)).cast<T>();
			}

			template <typename T>
			inline bool Set(const std::string& name, Handle instance, T&& value) const
			{
				size_t hash = StringHash::Hash(name);
				return m_Type.set(hash, std::move(instance.m_Handle), std::forward<T>(value));
			}

			template <typename T>
			inline bool Set(const size_t id, Handle instance, T &&value) const
			{
				return m_Type.set(id, std::move(instance.m_Handle), std::forward<T>(value));
			}

			inline bool Set(const std::string& name, Handle instance, Any value)
			{
				size_t hash = StringHash::Hash(name);
				return m_Type.set(hash, std::move(instance.m_Handle), value);
			}

			inline bool Set(const size_t id, Handle instance, Any value)
			{
				return m_Type.set(id, std::move(instance.m_Handle), value);
			}

			inline Reflection::Any Invoke(const std::string& name, Handle instance, Reflection::Any *const args, const size_t count) const 
			{
				size_t hash = StringHash::Hash(name);
				return m_Type.invoke(hash, std::move(instance.m_Handle), args, count);
			}

			template <typename... Args>
			inline Reflection::Any Invoke(const std::string &name, Handle instance, Args &&...args) const
			{
				size_t hash = StringHash::Hash(name);
				Reflection::Any arguments[sizeof...(Args) + 1u]{ std::forward<Args>(args)... };
				return m_Type.invoke(hash, std::move(instance.m_Handle), arguments, sizeof...(Args));
			}

			inline Reflection::Any Invoke(const size_t id, Handle instance, Reflection::Any *const args, const size_t count) const
			{
				return m_Type.invoke(id, std::move(instance.m_Handle), args, count);
			}

			template <typename... Args>
			inline Reflection::Any Invoke(const size_t id, Handle instance, Args &&...args) const
			{
				Reflection::Any arguments[sizeof...(Args) + 1u]{ std::forward<Args>(args)... };
				return m_Type.invoke(id, std::move(instance.m_Handle), arguments, sizeof...(Args));
			}

			inline size_t Rank() const { return m_Type.rank(); }

			inline Reflection::Type RemoveExtent() const { return Reflection::Type(m_Type.remove_extent()); }

			inline Reflection::Type RemovePointer() { return Reflection::Type(m_Type.remove_pointer()); }

			inline void Reset() { m_Type.reset(); }

			inline size_t Size() const { return m_Type.size_of(); }

			inline Reflection::Type TemplateArg(size_t index) const { return Reflection::Type(m_Type.template_arg(index)); }

			inline size_t TemplateArity() const { return m_Type.template_arity(); }

			inline Reflection::Type TemplateType() const { return Reflection::Type(m_Type.template_type()); }



		private:

			Type(entt::meta_type type) :
					m_Type(type)
			{
				m_Type.base();
			}

			entt::meta_type m_Type;
		};


		template <typename T>
		static inline TypeFactory<T> Register(const std::string& name)
		{
			size_t hash = StringHash::Hash(name);
			s_TypeAbstract[hash] = std::is_abstract<T>::value;
			s_TypeNames[hash] = name;
			return TypeFactory<T>(name);
		}

		template <typename T>
		static inline Type Resolve()
		{
			return Type(entt::resolve<T>());
		}

		static inline TypeRange<Type, entt::meta_type> Resolve()
		{
			return TypeRange<Type, entt::meta_type>(entt::resolve());
		}

		static inline Type Resolve(const std::string& name)
		{
			size_t hash = StringHash::Hash(name).m_Hash;
			return Type(entt::resolve(hash));
		}

		static inline Type Resolve(size_t hash)
		{
			return Type(entt::resolve(hash));
		}

		template <typename T>
		static inline Any ForwardAsMeta(T&& type)
		{
			return Any(entt::forward_as_meta(std::forward<Type>(type)));
		}


		template <class T>
		struct TypeData
		{
			TypeData()
			{
				//static_assert(HasTypeMethod<T>, "Class implementing IReflect must contain a static function with the following signature: \"static Reflection::TypeFactory<T> RegisterType()\"");
				CHROMA_CORE_INFO("Type registered for reflection: {}", typeid(T).name());
				T::RegisterType();
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
			//auto description = Reflection::Resolve<T>();
			//size_t hash = StringHash::Hash(description->GetName());
			//
			//s_SerializeYAMLFunctions[hash] = serialize_function;
			//s_DeserializeYAMLFunctions[hash] = deserialize_function;
		}

		template <typename T>
		static void RegisterBinarySerializeFunctions(std::function<void(File &, Reflection::Any &)> serialize_function, std::function<Reflection::Any(File &)> deserialize_function)
		{
			//auto description = Reflect::Resolve<T>();
			//size_t hash = StringHash::Hash(description->GetName());
			//
			//s_SerializeBinaryFunctions[hash] = serialize_function;
			//s_DeserializeBinaryFunctions[hash] = deserialize_function;
		}

		static void SerializeObjectYAML(YAML::Emitter &out, Reflection::Any object);
		static void SerliazeObjectBinary(File &file, Reflection::Any object);
		static Reflection::Any DeserializeObjectYAML(YAML::Node &node);
		static Reflection::Any DeserializeObjectBinary(File &file);

	private:

		static std::unordered_map<size_t, std::function<void(YAML::Emitter &, Reflection::Any &)>> s_SerializeYAMLFunctions;
		static std::unordered_map<size_t, std::function<Reflection::Any(YAML::Node &)>> s_DeserializeYAMLFunctions;
		static std::unordered_map<size_t, std::function<void(File &, Reflection::Any &)>> s_SerializeBinaryFunctions;
		static std::unordered_map<size_t, std::function<Reflection::Any(File &)>> s_DeserializeBinaryFunctions;

		static std::unordered_map<size_t, std::string> s_TypeNames;
		static std::unordered_map<size_t, std::unordered_map<size_t, std::string>> s_TypeDataNames;
		static std::unordered_map<size_t, std::unordered_map<size_t, std::string>> s_TypeFunctionNames;

		static std::unordered_map<size_t, bool> s_TypeAbstract;
	};




#ifndef CHROMA_REFLECT
	//Registers a class as a reflectable type. Requires the class to implement: Reflection::TypeFactory<T> RegisterType()
	//Note: Should be defined in a private scope
	#define CHROMA_REFLECT(typeName) \
	static inline Reflection::TypeData<typeName> _type_info{}; \
	static Reflection::TypeFactory<typeName> RegisterType();
#endif

} //namespace Chroma