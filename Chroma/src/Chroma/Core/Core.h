#pragma once

#include <memory>
#include <glm/glm.hpp>

#ifdef CHROMA_PLATFORM_WINDOWS
#if HZ_DYNAMIC_LINK
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

#ifdef CHROMA_ENABLE_ASSERTS
	#define CHROMA_ASSERT(x, ...) { if (!x)) {CHROMA_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
	#define CHROMA_CORE_ASSERT(x, ...) { if (!x)) {CHROMA_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
#else
	#define CHROMA_ASSERT(x, ...)
	#define CHROMA_CORE_ASSERT(x, ...)
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