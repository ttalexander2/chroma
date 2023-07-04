#pragma once

#include <type_traits>
#include <cstdint>
#include <memory>
#include <vector>
#include <list>
#include <deque>
#include <unordered_map>
#include <map>
#include <set>
#include <unordered_set>

namespace Chroma::Reflection::Internal
{
    // Pointer-like
    template<typename>
    struct is_pointer_like
            : std::false_type
    {
    };

    template<typename T>
    struct is_pointer_like<T *>
            : std::true_type
    {
    };

    template<typename T, size_t N>
    struct is_pointer_like<T (*)[N]>
            : std::false_type
    {
    };

    template<typename T>
    struct is_pointer_like<std::shared_ptr<T>>
            : std::true_type
    {
    };

    template<typename T, typename... Args>
    struct is_pointer_like<std::unique_ptr<T, Args...>>
            : std::true_type
    {
    };

    template<typename T>
    inline constexpr auto is_pointer_like_v = is_pointer_like<T>::value;


    // Sequence container
    template<typename>
    struct is_sequence_container : std::false_type
    {
    };

    template<typename... Args>
    struct is_sequence_container<std::vector<Args...>>
            : std::true_type
    {
    };

    template<typename... Args>
    struct is_sequence_container<std::array<Args...>>
            : std::true_type
    {
    };

    template<typename... Args>
    struct is_sequence_container<std::list<Args...>>
            : std::true_type
    {
    };

    template<typename... Args>
    struct is_sequence_container<std::deque<Args...>>
            : std::true_type
    {
    };

    template<typename T>
    inline constexpr auto is_sequence_container_v = is_sequence_container<T>::value;


    // Associative container
    template<typename>
    struct is_associative_container : std::false_type
    {
    };

    template<typename... Args>
    struct is_associative_container<std::unordered_map<Args...>>
            : std::true_type
    {
    };

    template<typename... Args>
    struct is_associative_container<std::map<Args...>>
            : std::true_type
    {
    };

    template<typename... Args>
    struct is_associative_container<std::set<Args...>>
            : std::true_type
    {
    };

    template<typename... Args>
    struct is_associative_container<std::unordered_set<Args...>>
            : std::true_type
    {
    };

    template<typename T>
    inline constexpr auto is_associative_container_v = is_associative_container<T>::value;

    template<typename T, typename = void>
    struct is_equality_comparable : std::false_type
    {
    };

    template<typename T>
    struct is_equality_comparable<T,
            typename std::enable_if<true && !is_associative_container_v<T> && !is_sequence_container_v<T>, decltype(std::declval<const T &>() ==
                                                   std::declval<const T &>(), (void) 0)>::type>
            : std::true_type
    {
    };

    template<typename T>
    inline constexpr bool is_equality_comparable_v = is_equality_comparable<T>::value;

    // Remove pointer
    template<typename T>
    struct remove_all_pointers
    {
        typedef T type;
    };

    template<typename T>
    struct remove_all_pointers<T *>
    {
        typedef typename remove_all_pointers<T>::type type;
    };

    template<typename T>
    struct remove_all_pointers<T *const>
    {
        typedef typename remove_all_pointers<T>::type type;
    };

    template<typename T>
    struct remove_all_pointers<T *volatile>
    {
        typedef typename remove_all_pointers<T>::type type;
    };

    template<typename T>
    struct remove_all_pointers<T *const volatile>
    {
        typedef typename remove_all_pointers<T>::type type;
    };

    template<typename T, typename... Args>
    struct remove_all_pointers<std::unique_ptr<T, Args...>>
    {
        typedef typename remove_all_pointers<T>::type type;
    };

    template<typename T>
    struct remove_all_pointers<std::shared_ptr<T>>
    {
        typedef typename remove_all_pointers<T>::type type;
    };

    template<typename T, size_t N>
    struct remove_all_pointers<T (*)[N]>
    {
        typedef typename remove_all_pointers<T>::type type;
    };

    template<typename From, typename To>
    struct is_convertible
    {
    private:
        template<typename U>
        static auto check(U *t)
        -> decltype(t->operator To(), std::true_type{});

        template<typename>
        static std::false_type check(...);

    public:
        static constexpr bool value = std::is_convertible_v<From, To> || decltype(check<From>(nullptr))::value || (std::is_arithmetic_v<From> && std::is_arithmetic_v<To>);
    };

    template<typename T, typename U>
    inline constexpr auto is_convertible_v = is_convertible<T, U>::value;


}