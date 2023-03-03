#pragma once

#include <type_traits>
#include <functional>
#include "any.h"


namespace Chroma
{
	struct Transform;
}

namespace Chroma::Reflection::internal
{

    // Implementation of std::invoke (from cppreference)
    namespace detail
    {
        template<class>
        constexpr bool is_reference_wrapper_v = false;
        template<class U>
        constexpr bool is_reference_wrapper_v<std::reference_wrapper<U>> = true;

        template<class C, class Pointed, class T1, class... Args>
        constexpr decltype(auto) invoke_member_pointer(Pointed C::* f, T1 &&t1, Args &&... args)
        {
            if constexpr (std::is_function_v<Pointed>)
            {
                if constexpr (std::is_base_of_v<C, std::decay_t<T1>>)
                    return (std::forward<T1>(t1).*f)(std::forward<Args>(args)...);
                else if constexpr (is_reference_wrapper_v<std::decay_t<T1>>)
                    return (t1.get().*f)(std::forward<Args>(args)...);
                else
                    return ((*std::forward<T1>(t1)).*f)(std::forward<Args>(args)...);
            } else
            {
                static_assert(std::is_object_v<Pointed> && sizeof...(args) == 0);
                if constexpr (std::is_base_of_v<C, std::decay_t<T1>>)
                    return std::forward<T1>(t1).*f;
                else if constexpr (is_reference_wrapper_v<std::decay_t<T1>>)
                    return t1.get().*f;
                else
                    return (*std::forward<T1>(t1)).*f;
            }
        }

        template<class F, class... Args>
        constexpr std::invoke_result_t<F, Args...> invoke(F &&f, Args &&... args)
        noexcept(std::is_nothrow_invocable_v<F, Args...>)
        {
            if constexpr (std::is_member_pointer_v<std::decay_t<F>>)
                return detail::invoke_member_pointer(f, std::forward<Args>(args)...);
            else
                return std::forward<F>(f)(std::forward<Args>(args)...);
        }
    }


    template<typename... Type>
    struct type_list
    {
        using type = type_list;
        static constexpr auto size = sizeof...(Type);
    };

    template<typename, typename>
    struct function_descriptor;

    template<typename Type, typename Ret, typename Class, typename... Args>
    struct function_descriptor<Type, Ret (Class::*)(Args...) const>
    {
        using return_type = Ret;
        using args_type = std::conditional_t<std::is_base_of_v<Class, Type>,
                type_list<Args...>, type_list<const Class &, Args...>>;

        static constexpr bool is_const = true;
        static constexpr bool is_static = !std::is_base_of_v<Class, Type>;
    };

    template<typename Type, typename Ret, typename Class, typename... Args>
    struct function_descriptor<Type, Ret (Class::*)(Args...)>
    {
        using return_type = Ret;
        using args_type = std::conditional_t<std::is_base_of_v<Class, Type>,
                type_list<Args...>, type_list<Class &, Args...>>;

        static constexpr bool is_const = false;
        static constexpr bool is_static = !std::is_base_of_v<Class, Type>;
    };

    template<typename Type, typename Ret, typename Class>
    struct function_descriptor<Type, Ret Class::*>
    {
        using return_type = Ret &;
        using args_type = std::conditional_t<std::is_base_of_v<Class, Type>, type_list<>, type_list<Class &>>;

        static constexpr bool is_const = false;
        static constexpr bool is_static = !std::is_base_of_v<Class, Type>;
    };

    template<typename Type, typename Ret, typename MaybeType, typename... Args>
    struct function_descriptor<Type, Ret (*)(MaybeType, Args...)>
    {
        using return_type = Ret;
        using args_type = std::conditional_t<std::is_base_of_v<std::remove_cv_t<std::remove_reference_t<MaybeType>>,
                Type>, type_list<Args...>, type_list<MaybeType, Args...>>;

        static constexpr bool is_const = std::is_base_of_v<std::remove_cv_t<std::remove_reference_t<MaybeType>>, Type>
                                         && std::is_const_v<std::remove_reference_t<MaybeType>>;
        static constexpr bool is_static = !std::is_base_of_v<std::remove_cv_t<std::remove_reference_t<MaybeType>>, Type>;
    };

    template<typename Type, typename Ret>
    struct function_descriptor<Type, Ret (*)()>
    {
        using return_type = Ret;
        using args_type = type_list<>;
        static constexpr bool is_const = false;
        static constexpr bool is_static = true;
    };

    template<typename Type, typename Candidate>
    class function_helper
    {
        template<typename Ret, typename... Args, typename Class>
        static constexpr function_descriptor<Type, Ret (Class::*)(Args...) const>
        get_rid_of_noexcept(Ret (Class::*)(Args...) const)
        {}

        template<typename Ret, typename... Args, typename Class>
        static constexpr function_descriptor<Type, Ret (Class::*)(Args...)>
        get_rid_of_noexcept(Ret (Class::*)(Args...))
        {}

        template<typename Ret, typename Class>
        static constexpr function_descriptor<Type, Ret Class::*>
        get_rid_of_noexcept(Ret Class::*)
        {}

        template<typename Ret, typename... Args>
        static constexpr function_descriptor<Type, Ret (*)(Args...)>
        get_rid_of_noexcept(Ret (*)(Args...))
        {}

        template<typename Class>
        static constexpr function_descriptor<Class, decltype(&Class::operator())>
        get_rid_of_noexcept(Class)
        {}

    public:
        using type = decltype(get_rid_of_noexcept(std::declval<Candidate>()));
    };

    template<typename Type, typename Candidate>
    using function_helper_t = typename function_helper<Type, Candidate>::type;

    template<std::size_t, typename>
    struct type_list_element;

    template<std::size_t Index, typename First, typename... Other>
    struct type_list_element<Index, type_list<First, Other...>>
            : type_list_element<Index - 1u, type_list<Other...>>
    {
    };

    template<typename First, typename... Other>
    struct type_list_element<0u, type_list<First, Other...>>
    {
        using type = First;
    };

    template<std::size_t Index, typename List>
    using type_list_element_t = typename type_list_element<Index, List>::type;


    template<typename Type, auto Data>
    [[nodiscard]] bool set_function([[maybe_unused]] Reflection::handle& handle, [[maybe_unused]] any value)
    {
        if (handle.type().id() != type_hash_v<Type>)
        {
        	return false;
        }
    	
    	if constexpr(!std::is_same_v<decltype(Data), Type> && !std::is_same_v<decltype(Data), std::nullptr_t>) {
    		if constexpr(std::is_member_function_pointer_v<decltype(Data)> || std::is_function_v<std::remove_reference_t<std::remove_pointer_t<decltype(Data)>>>) {
    			using descriptor = internal::function_helper_t<Type, decltype(Data)>;
    			using data_type = type_list_element_t<descriptor::is_static, typename descriptor::args_type>;

    			if(auto *const clazz = handle.try_cast<Type>(); clazz && value.can_cast_or_convert<data_type>()) {
    				std::invoke(Data, *clazz, value.cast_or_convert<data_type>());
    				return true;
    			}
    		} else if constexpr(std::is_member_object_pointer_v<decltype(Data)>) {
    			
    			using data_type = std::remove_reference_t<typename function_helper_t<Type, decltype(Data)>::return_type>;

    			if constexpr(!std::is_array_v<data_type> && !std::is_const_v<data_type>) {
    				if(auto *const clazz = handle.try_cast<Type>(); clazz != nullptr && value.can_cast_or_convert<data_type>()) {
    					auto dataValue = value.cast_or_convert<data_type>();
    					detail::invoke(Data, *clazz) = dataValue;
    					return true;
    				}
    			}
    		} else {
    			using data_type = std::remove_reference_t<decltype(*Data)>;

    			if constexpr(!std::is_array_v<data_type> && !std::is_const_v<data_type>) {
    				if(value.can_cast_or_convert<data_type>()) {
    					*Data = value.cast_or_convert<data_type>();
    					return true;
    				}
    			}
    		}
    	}

    	return false;
    }

	template<typename Type>
	any dispatch([[maybe_unused]] Type&& value)
    {
	    return any{std::forward<Type>(value)};
    }

    template<typename Type, auto Data>
    [[nodiscard]] any get_function(Reflection::handle handle)
    {
        if (handle.type().id() != type_hash_v<Type>)
            return false;

        if constexpr (std::is_member_pointer_v<decltype(Data)>
                      || std::is_function_v<std::remove_reference_t<std::remove_pointer_t<decltype(Data)>>>)
        {
            if constexpr (!std::is_array_v<std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<decltype(Data), Type &>>>>)
            {
                if constexpr (std::is_invocable_v<decltype(Data), Type &>)
                {
                    if (auto *clazz = handle.try_cast<Type>(); clazz)
                    {
                        return dispatch(std::invoke(Data, *clazz));
                    }
                }
                if constexpr (std::is_invocable_v<decltype(Data), const Type &>)
                {
                    if (auto *clazz = handle.try_cast<const Type>(); clazz)
                    {
                    	return dispatch(std::invoke(Data, *clazz));
                    }
                }
            } else if constexpr (std::is_pointer_v<decltype(Data)>)
            {
                if constexpr (std::is_array_v<std::remove_pointer_t<decltype(Data)>>)
                {
                    return any{};
                } else
                {
                	return dispatch(*Data);
                }
            } else
            {
            	return dispatch(Data);
            }
        }
        return any{};
    }

    template<typename... Args>
    [[nodiscard]] uint32_t unwrap_func_args(type_list<Args...>, const std::size_t index) noexcept
    {
        uint32_t args[sizeof...(Args) + 1u]{type_hash_v<Args>...};
        return args[index];
    }

    template<typename Types>
    [[nodiscard]] static uint32_t func_arg(const size_t index) noexcept  // NOLINT(clang-diagnostic-unused-template)
    {
        return unwrap_func_args(Types{}, index);
    }

    template<typename... Args>
    [[nodiscard]] uint32_t ctor_arg(const size_t index) noexcept
    {
        uint32_t args[sizeof...(Args) + 1u]{type_hash_v<Args>...};
        return args[index];
    }

    template<typename Func, typename... Args>
    [[nodiscard]] any invoke_with_args(Func &&func, Args &&...args)
    {
        if constexpr (std::is_same_v<decltype(std::invoke(std::forward<Func>(func), args...)), void>)
        {
            std::invoke(std::forward<Func>(func), args...);
            return any{std::in_place_type<void>};
        } else
        {
            return any(std::invoke(std::forward<Func>(func), args...));
        }
    }

    template<typename Type, typename Func, std::size_t... Index>
    [[nodiscard]] any func_invoke([[maybe_unused]] Reflection::handle handle, Func &&func,
                                  [[maybe_unused]] any *args, std::index_sequence<Index...>)
    {
        using descriptor = function_helper_t<Type, std::remove_reference_t<Func>>;

        if constexpr (std::is_invocable_v<std::remove_reference_t<Func>, const Type &,
                type_list_element_t<Index, typename descriptor::args_type>...>)
        {
            if (const auto *clazz = handle.template try_cast<const Type>(); clazz &&
                                                                            ((args +
                                                                              Index)->template can_cast_or_convert<type_list_element_t<Index, typename descriptor::args_type>>() && ...))
            {
                return invoke_with_args(std::forward<Func>(func), *clazz,
                                        (args +
                                         Index)->template cast_or_convert<type_list_element_t<Index, typename descriptor::args_type>>()...);
            }
        } else if constexpr (std::is_invocable_v<std::remove_reference_t<Func>, Type &,
                type_list_element_t<Index, typename descriptor::args_type>...>)
        {
            if (auto *clazz = handle.template try_cast<Type>(); clazz &&
                                                                ((args +
                                                                  Index)->template can_cast_or_convert<type_list_element_t<Index, typename descriptor::args_type>>() && ...))
            {
                return invoke_with_args(std::forward<Func>(func), *clazz,
                                        (args +
                                         Index)->template cast_or_convert<type_list_element_t<Index, typename descriptor::args_type>>()...);
            }
        } else
        {
            if (((args +
                  Index)->template can_cast_or_convert<type_list_element_t<Index, typename descriptor::args_type>>() && ...))
            {
                return invoke_with_args(std::forward<Func>(func),
                                        (args +
                                         Index)->cast_or_convert<type_list_element_t<Index, typename descriptor::args_type>>()...);
            }
        }

        return any{};
    }

    template<typename Type, typename Func>
    [[nodiscard]] any
    func_invoke([[maybe_unused]] Reflection::handle handle, Func &&candidate, [[maybe_unused]] any *const args)
    {
        return internal::func_invoke<Type>(std::move(handle), std::forward<Func>(candidate), args,
                                           std::make_index_sequence<function_helper_t<Type, std::remove_reference_t<Func>>::args_type::size>{});
    }

    template<typename Type, auto Func>
    [[nodiscard]] any func_invoke(Reflection::handle instance, any *const args)
    {
        return internal::func_invoke<Type>(std::move(instance), Func, args,
                                           std::make_index_sequence<function_helper_t<Type, std::remove_reference_t<decltype(Func)>>::args_type::size>{});

    }


    template<typename From, typename To, auto Func>
    any convert_type(any from)
    {
        using From_ = std::remove_cv_t<std::remove_reference_t<From>>;
        using To_ = std::remove_cv_t<std::remove_reference_t<To>>;

        if constexpr (!std::is_same_v<decltype(Func), From_> && !std::is_same_v<decltype(Func), std::nullptr_t>)
        {
            if constexpr (std::is_member_function_pointer_v<decltype(Func)>)
            {
                using return_type = std::invoke_result_t<decltype(Func), From>;

                static_assert(std::is_same_v<return_type, To_>,
                              "Conversion func does not return the correct type!");

                From *clazz = from.try_cast<From>();
                if (clazz == nullptr)
                    return nullptr;
                To result = std::invoke(Func, *clazz);
                return result;
            }
        	else if constexpr (std::is_function_v<std::remove_reference_t<std::remove_pointer_t<decltype(Func)>>>)
            {
                using descriptor = internal::function_helper_t<From, decltype(Func)>;

                static_assert(std::is_same_v<typename descriptor::return_type, To_>,
                              "Conversion func does not return the correct type!");
                static_assert(std::is_constructible_v<std::function<To_(From_)>, decltype(Func)>,
                              "Conversion func does not take the correct type!"); // This is very stupid, but it works, and I'm lazy.

                From *clazz = from.try_cast<From>();
                if (clazz == nullptr)
                    return nullptr;

                To result = std::invoke(Func, *clazz);
                return result;
            }
        }
        return {};
    }

    template<typename From, typename To>
    any convert_type(any from)
    {
        using From_ = std::remove_cv_t<std::remove_reference_t<From>>;
        using To_ = std::remove_cv_t<std::remove_reference_t<To>>;

        From *val = from.try_cast<From>();
        if (val == nullptr)
        {
            return any{};
        }

        if constexpr (std::is_constructible_v<To_, From>)
        {
            return Reflection::any(To_(*val));
        } else if constexpr (std::is_convertible_v<From_, To_>)
        {
            return static_cast<To_>(*val);
        } else if constexpr (internal::is_convertible_v<From_, To_>)
        {
            return val->operator To();
        }
        return {};
    }


    template<typename Type, typename... Args, std::size_t... Index>
    [[nodiscard]] any construct_with_args(any *const args, std::index_sequence<Index...>)
    {
        if (((args + Index)->template can_cast_or_convert<Args>() && ...))
        {
            return any{std::in_place_type<Type>, (args + Index)->cast<Args>()...};
        }
        return any{};
    }

    template<typename Type, typename... Args>
    [[nodiscard]] any construct(any *const args)
    {
        return internal::construct_with_args<Type, Args...>(args, std::index_sequence_for<Args...>{});
    }

    template<typename Type, typename Func, std::size_t... Index>
    [[nodiscard]] any construct_with_function(Func &&func, any *const args, std::index_sequence<Index...>)
    {
        using descriptor = function_helper_t<Type, std::remove_reference_t<Func>>;

        if (((args +
              Index)->template can_cast_or_convert<type_list_element_t<Index, typename descriptor::args_type>>() && ...))
        {
            return invoke_with_args(std::forward<Func>(func),
                                    (args +
                                     Index)->template cast_or_convert<type_list_element_t<Index, typename descriptor::args_type>>()...);
        }
        return any{};
    }

    template<typename Type, auto Func>
    [[nodiscard]] any construct(any *const args)
    {
        return internal::construct_with_function<Type>(Func, args,
                                                       std::make_index_sequence<function_helper_t<Type, std::remove_reference_t<decltype(Func)>>::args_type::size>{});
    }

    template<size_t N>
    struct string_literal
    {
        constexpr string_literal() = default;

        constexpr string_literal(const char (&str)[N])
        {
            std::copy_n(str, N, value);
        }

        static constexpr size_t size = N;

        char value[N]{};
    };

}

