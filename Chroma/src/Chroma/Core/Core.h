#pragma once

#include <memory>
#include <glm/glm.hpp>

#ifdef CHROMA_PLATFORM_WINDOWS
#if CHROMA_DYNAMIC_LINK
	#ifdef CHROMA_BUILD_DLL
		#define CHROMA_API __declspec(dllexport)
	#else
		#define	CHROMA_API __declspec(dllimport)	
	#endif
#else
#define CHROMA_API
#endif
#else
	#error Chroma only builds on windows at the moment!
#endif

#ifdef CHROMA_DEBUG
	#if defined(CHROMA_PLATFORM_WINDOWS)
		#define CHROMA_DEBUGBREAK() __debugbreak()
	#elif defined(CHROMA_PLATFORM_LINUX)
		#include <signal.h>
		#define CHROMA_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
#else
	#define CHROMA_DEBUGBREAK()
#endif

#define CHROMA_EXPAND_MACRO(x) x
#define CHROMA_STRINGIFY_MACRO(x) #x

#ifdef CHROMA_ENABLE_ASSERTS
	#define CHROMA_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { CHROMA##type##ERROR(msg, __VA_ARGS__); CHROMA_DEBUGBREAK(); } }
	#define CHROMA_INTERNAL_ASSERT_WITH_MSG(type, check, ...) CHROMA_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define CHROMA_INTERNAL_ASSERT_NO_MSG(type, check) CHROMA_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", CHROMA_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define CHROMA_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define CHROMA_INTERNAL_ASSERT_GET_MACRO(...) CHROMA_EXPAND_MACRO( CHROMA_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, CHROMA_INTERNAL_ASSERT_WITH_MSG, CHROMA_INTERNAL_ASSERT_NO_MSG) )

	#define CHROMA_ASSERT(...) CHROMA_EXPAND_MACRO( CHROMA_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define CHROMA_CORE_ASSERT(...) CHROMA_EXPAND_MACRO( CHROMA_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define CHROMA_ASSERT(...)
	#define CHROMA_CORE_ASSERT(...)
#endif

#define CHROMA_BIND_EVENT_FN(event_function) std::bind(&event_function, this, std::placeholders::_1)

#define BIT(x) (1 << x)

namespace Chroma
{

	// TODO: Replace unique_ptr and shared_ptr with new reference counter class
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>; // Possible overhead from atomic increment/decrement, but likely negligible
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}