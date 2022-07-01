#pragma once

#include <type_traits>
#include <algorithm>

namespace Chroma
{
	namespace Util
	{
		template <class Default, class AlwaysVoid, template <class...> class Op, class... Args>
		struct detector
		{
			using value_t = std::false_type;
			using type = Default;
		};

		template <class Default, template <class...> class Op, class... Args>
		struct detector<Default, std::void_t<Op<Args...>>, Op, Args...>
		{
			using value_t = std::true_type;
			using type = Op<Args...>;
		};

		struct nonesuch
		{
		};

		template <template <class...> class Op, class... Args>
		using is_detected = typename detector<nonesuch, void, Op, Args...>::value_t;

		template <template <class...> class Op, class... Args>
		using detected_t = typename detector<nonesuch, void, Op, Args...>::type;

		template <class Default, template <class...> class Op, class... Args>
		using detected_or = detector<Default, void, Op, Args...>;

		template <typename Lhs, typename Rhs>
		using addition_t = decltype(std::declval<Lhs>() + std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool addition_detected = is_detected<addition_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using subtraction_t = decltype(std::declval<Lhs>() - std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool subtraction_detected = is_detected<subtraction_t, Lhs, Rhs>::value;

		template <typename T>
		using unary_plus_t = decltype(+std::declval<T>());
		template <typename T>
		constexpr bool unary_plus_detected = is_detected<unary_plus_t, T>::value;

		template <typename T>
		using unary_minus_t = decltype(-std::declval<T>());
		template <typename T>
		constexpr bool unary_minus_detected = is_detected<unary_minus_t, T>::value;

		template <typename Lhs, typename Rhs>
		using multiplication_t = decltype(std::declval<Lhs>() * std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool multiplication_detected = is_detected<multiplication_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using division_t = decltype(std::declval<Lhs>() / std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool division_detected = is_detected<division_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using modulo_t = decltype(std::declval<Lhs>() % std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool modulo_detected = is_detected<modulo_t, Lhs, Rhs>::value;

		template <typename T>
		using preincrement_t = decltype(++std::declval<T>());
		template <typename T>
		constexpr bool preincrement_detected = is_detected<preincrement_t, T>::value;

		template <typename T>
		using postincrement_t = decltype(std::declval<T>()++);
		template <typename T>
		constexpr bool postincrement_detected = is_detected<postincrement_t, T>::value;

		template <typename T>
		using predeincrement_t = decltype(--std::declval<T>());
		template <typename T>
		constexpr bool predeincrement_detected = is_detected<predeincrement_t, T>::value;

		template <typename T>
		using postdeincrement_t = decltype(std::declval<T>()--);
		template <typename T>
		constexpr bool postdeincrement_detected = is_detected<postdeincrement_t, T>::value;

		template <typename Lhs, typename Rhs>
		using equal_to_t = decltype(std::declval<Lhs>() == std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool equal_to_detected = is_detected<equal_to_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using not_equal_to_t = decltype(std::declval<Lhs>() != std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool not_equal_to_detected = is_detected<not_equal_to_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using greater_than_t = decltype(std::declval<Lhs>() > std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool greater_than_detected = is_detected<greater_than_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using less_than_t = decltype(std::declval<Lhs>() < std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool less_than_detected = is_detected<less_than_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using greater_than_or_equal_to_t = decltype(std::declval<Lhs>() >= std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool greater_than_or_equal_to_detected = is_detected<greater_than_or_equal_to_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using less_than_or_equal_to_t = decltype(std::declval<Lhs>() <= std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool less_than_or_equal_to_detected = is_detected<less_than_or_equal_to_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using three_way_comparison_t = decltype(std::declval<Lhs>() <=> std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool three_way_comparison_detected = is_detected<three_way_comparison_t, Lhs, Rhs>::value;

		template <typename T>
		using logical_negation_t = decltype(!std::declval<T>());
		template <typename T>
		constexpr bool logical_negation_detected = is_detected<logical_negation_t, T>::value;

		template <typename Lhs, typename Rhs>
		using logical_and_t = decltype(std::declval<Lhs>() && std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool logical_and_detected = is_detected<logical_and_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using logical_or_t = decltype(std::declval<Lhs>() || std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool logical_or_detected = is_detected<logical_or_t, Lhs, Rhs>::value;

		template <typename T>
		using bitwise_not_t = decltype(~std::declval<T>());
		template <typename T>
		constexpr bool bitwise_not_detected = is_detected<bitwise_not_t, T>::value;

		template <typename Lhs, typename Rhs>
		using bitwise_and_t = decltype(std::declval<Lhs>() & std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool bitwise_and_detected = is_detected<bitwise_and_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using bitwise_or_t = decltype(std::declval<Lhs>() | std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool bitwise_or_detected = is_detected<bitwise_or_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using bitwise_xor_t = decltype(std::declval<Lhs>() ^ std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool bitwise_xor_detected = is_detected<bitwise_xor_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using bitwise_left_shift_t = decltype(std::declval<Lhs>() << std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool bitwise_left_shift_detected = is_detected<bitwise_left_shift_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using bitwise_right_shift_t = decltype(std::declval<Lhs>() >> std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool bitwise_right_shift_detected = is_detected<bitwise_right_shift_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using assignment_t = decltype(std::declval<Lhs>() = std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool assignment_detected = is_detected<assignment_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using addition_assignment_t = decltype(std::declval<Lhs>() += std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool addition_assignment_detected = is_detected<addition_assignment_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using subtraction_assignment_t = decltype(std::declval<Lhs>() -= std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool subtraction_assignment_detected = is_detected<subtraction_assignment_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using multipication_assignment_t = decltype(std::declval<Lhs>() *= std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool multipication_assignment_detected = is_detected<multipication_assignment_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using division_assignment_t = decltype(std::declval<Lhs>() /= std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool division_assignment_detected = is_detected<division_assignment_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using modulo_assignment_t = decltype(std::declval<Lhs>() %= std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool modulo_assignment_detected = is_detected<modulo_assignment_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using bitwise_and_assignment_t = decltype(std::declval<Lhs>() &= std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool bitwise_and_assignment_detected = is_detected<bitwise_and_assignment_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using bitwise_or_assignment_t = decltype(std::declval<Lhs>() |= std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool bitwise_or_assignment_detected = is_detected<bitwise_or_assignment_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using bitwise_xor_assignment_t = decltype(std::declval<Lhs>() ^= std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool bitwise_xor_assignment_detected = is_detected<bitwise_xor_assignment_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using bitwise_left_shift_assignment_t = decltype(std::declval<Lhs>() <<= std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool bitwise_left_shift_assignment_detected = is_detected<bitwise_left_shift_assignment_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using bitwise_right_shift_assignment_t = decltype(std::declval<Lhs>() >>= std::declval<Rhs>());
		template <typename Lhs, typename Rhs>
		constexpr bool bitwise_right_shift_assignment_detected = is_detected<bitwise_right_shift_assignment_t, Lhs, Rhs>::value;

		template <typename Lhs, typename Rhs>
		using subscript_t = decltype(std::declval<Lhs>()[std::declval<Rhs>()]);
		template <typename Lhs, typename Rhs>
		constexpr bool subscript_detected = is_detected<subscript_t, Lhs, Rhs>::value;
	} //namespace Util

	template <size_t N>
	struct StringLiteral
	{
		constexpr StringLiteral(const char (&str)[N])
		{
			std::copy_n(str, N, value);
		}

		char value[N];
	};

	template <typename ComponentType, typename T, StringLiteral S>
	struct ComponentData
	{
		const std::string name = S.value;
		T data;

		ComponentData(const ComponentData &) = default;
		ComponentData &operator=(const ComponentData &) = default;


		template <typename... Args>
		ComponentData(Args &&...args) :
				data(std::forward<Args>(args)...)
		{
		}

		const T &operator*() const
		{
			return data;
		}

		const T* operator->() const
		{
			return &data;
		}

		template <typename... Args>
		ComponentData& operator=(Args &&...args)
		{
			data = T{ std::forward<Args>(args)... };
			return *this;
		}

		operator T() { return data; }

		template <typename U = T, std::enable_if_t<Util::addition_detected<U, U>, bool> = true>
		T operator+(const T &v)
		{
			return data + v;
		}

		template <typename U = T, std::enable_if_t<Util::addition_detected<U, U>, bool> = true>
		T operator+(const ComponentData &v)
		{
			return data + v.data;
		}

		template <typename U = T, std::enable_if_t<Util::subtraction_detected<U, U>, bool> = true>
		T operator-(const T &v)
		{
			return data - v;
		}

		template <typename U = T, std::enable_if_t<Util::subtraction_detected<U, U>, bool> = true>
		T operator-(const ComponentData &v)
		{
			return data - v.data;
		}

		template <typename U = T, std::enable_if_t<Util::unary_plus_detected<U>, bool> = true>
		T operator+()
		{
			return +data;
		}

		template <typename U = T, std::enable_if_t<Util::unary_minus_detected<U>, bool> = true>
		T operator-()
		{
			return -data;
		}

		template <typename U = T, std::enable_if_t<Util::multiplication_detected<U, U>, bool> = true>
		T operator*(const T &v)
		{
			return data * v;
		}
		template <typename U = T, std::enable_if_t<Util::multiplication_detected<U, U>, bool> = true>
		T operator*(const ComponentData &v)
		{
			return data * v.data;
		}

		template <typename U = T, std::enable_if_t<Util::division_detected<U, U>, bool> = true>
		T operator/(const T &v)
		{
			return data / v;
		}

		template <typename U = T, std::enable_if_t<Util::division_detected<U, U>, bool> = true>
		T operator/(const ComponentData &v)
		{
			return data / v.data;
		}

		template <typename U = T, std::enable_if_t<Util::modulo_detected<U, U>, bool> = true>
		T operator%(const T &v)
		{
			return data % v;
		}

		template <typename U = T, std::enable_if_t<Util::modulo_detected<U, U>, bool> = true>
		T operator%(const ComponentData &v)
		{
			return data % v.data;
		}

		template <typename U = T, std::enable_if_t<Util::preincrement_detected<U>, bool> = true>
		T &operator++()
		{
			return ++data;
		}

		template <typename U = T, std::enable_if_t<Util::postincrement_detected<U>, bool> = true>
		T operator++(int)
		{
			return data++;
		}

		template <typename U = T, std::enable_if_t<Util::predeincrement_detected<U>, bool> = true>
		T &operator--()
		{
			return --data;
		}

		template <typename U = T, std::enable_if_t<Util::postdeincrement_detected<U>, bool> = true>
		T operator--(int)
		{
			return data--;
		}

		template <typename U = T, std::enable_if_t<Util::equal_to_detected<U, U>, bool> = true>
		bool operator==(const T &v) const
		{
			return data == v;
		}

		template <typename U = T, std::enable_if_t<Util::equal_to_detected<U, U>, bool> = true>
		bool operator==(const ComponentData &v) const
		{
			return data == v.data;
		}

		template <typename U = T, std::enable_if_t<Util::not_equal_to_detected<U, U>, bool> = true>
		bool operator!=(const T &v) const
		{
			return data != v;
		}

		template <typename U = T, std::enable_if_t<Util::not_equal_to_detected<U, U>, bool> = true>
		bool operator!=(const ComponentData &v) const
		{
			return data != v.data;
		}

		template <typename U = T, std::enable_if_t<Util::greater_than_detected<U, U>, bool> = true>
		bool operator>(const T &v) const
		{
			return data > v;
		}

		template <typename U = T, std::enable_if_t<Util::greater_than_detected<U, U>, bool> = true>
		bool operator>(const ComponentData &v) const
		{
			return data > v.data;
		}

		template <typename U = T, std::enable_if_t<Util::less_than_detected<U, U>, bool> = true>
		bool operator<(const T &v) const
		{
			return data < v;
		}

		template <typename U = T, std::enable_if_t<Util::less_than_detected<U, U>, bool> = true>
		bool operator<(const ComponentData &v) const
		{
			return data < v.data;
		}

		template <typename U = T, std::enable_if_t<Util::greater_than_or_equal_to_detected<U, U>, bool> = true>
		bool operator>=(const T &v) const
		{
			return data >= v;
		}

		template <typename U = T, std::enable_if_t<Util::greater_than_or_equal_to_detected<U, U>, bool> = true>
		bool operator>=(const ComponentData &v) const
		{
			return data >= v.data;
		}

		template <typename U = T, std::enable_if_t<Util::less_than_or_equal_to_detected<U, U>, bool> = true>
		bool operator<=(const T &v) const
		{
			return data <= v;
		}

		template <typename U = T, std::enable_if_t<Util::less_than_or_equal_to_detected<U, U>, bool> = true>
		bool operator<=(const ComponentData &v) const
		{
			return data <= v.data;
		}

		template <typename U = T, std::enable_if_t<Util::three_way_comparison_detected<U, U>, bool> = true>
		bool operator<=>(const T &v) const
		{
			return data <=> v;
		}

		template <typename U = T, std::enable_if_t<Util::three_way_comparison_detected<U, U>, bool> = true>
		bool operator<=>(const ComponentData &v)
		{
			return data <=> v.data;
		}

		template <typename U = T, std::enable_if_t<Util::logical_negation_detected<U>, bool> = true>
		bool operator!()
		{
			return !data;
		}

		template <typename U = T, std::enable_if_t<Util::logical_and_detected<U, U>, bool> = true>
		bool operator&&(const T &v)
		{
			return data && v;
		}

		template <typename U = T, std::enable_if_t<Util::logical_and_detected<U, U>, bool> = true>
		bool operator&&(const ComponentData &v)
		{
			return data && v.data;
		}

		template <typename U = T, std::enable_if_t<Util::logical_or_detected<U, U>, bool> = true>
		bool operator||(const T &v)
		{
			return data || v;
		}

		template <typename U = T, std::enable_if_t<Util::logical_or_detected<U, U>, bool> = true>
		bool operator||(const ComponentData &v)
		{
			return data || v.data;
		}

		template <typename U = T, std::enable_if_t<Util::bitwise_not_detected<U>, bool> = true>
		bool operator~()
		{
			return ~data;
		}

		template <typename U = T, std::enable_if_t<Util::bitwise_and_detected<U, U>, bool> = true>
		bool operator&(const T &v)
		{
			return data & v;
		}

		template <typename U = T, std::enable_if_t<Util::bitwise_and_detected<U, U>, bool> = true>
		bool operator&(const ComponentData &v)
		{
			return data & v.data;
		}

		template <typename U = T, std::enable_if_t<Util::bitwise_or_detected<U, U>, bool> = true>
		bool operator|(const T &v)
		{
			return data | v;
		}

		template <typename U = T, std::enable_if_t<Util::bitwise_or_detected<U, U>, bool> = true>
		bool operator|(const ComponentData &v)
		{
			return data | v.data;
		}

		template <typename U = T, std::enable_if_t<Util::bitwise_xor_detected<U, U>, bool> = true>
		bool operator^(const T &v)
		{
			return data ^ v;
		}

		template <typename U = T, std::enable_if_t<Util::bitwise_xor_detected<U, U>, bool> = true>
		bool operator^(const ComponentData &v)
		{
			return data ^ v.data;
		}

		template <typename U = T, std::enable_if_t<Util::bitwise_left_shift_detected<U, U>, bool> = true>
		bool operator<<(const T &v)
		{
			return data << v;
		}

		template <typename U = T, std::enable_if_t<Util::bitwise_left_shift_detected<U, U>, bool> = true>
		bool operator<<(const ComponentData &v)
		{
			return data << v.data;
		}

		template <typename U = T, std::enable_if_t<Util::bitwise_right_shift_detected<U, U>, bool> = true>
		bool operator>>(const T &v)
		{
			return data >> v;
		}

		template <typename U = T, std::enable_if_t<Util::bitwise_right_shift_detected<U, U>, bool> = true>
		bool operator>>(const ComponentData &v)
		{
			return data >> v.data;
		}

	};
} //namespace Chroma