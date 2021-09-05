#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <filesystem>


#ifdef _WIN32
	#ifndef CHROMA_PLATFORM_WINDOWS
	#define CHROMA_PLATFORM_WINDOWS
	#endif
#endif

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
	#define CHROMA_ENABLE_ASSERTS
#else
	#define CHROMA_DEBUGBREAK()
#endif

#define CHROMA_EXPAND_MACRO(x) x
#define CHROMA_STRINGIFY_MACRO(x) #x

#ifdef CHROMA_ENABLE_ASSERTS
	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define CHROMA_ASSERT(check, ...) if(!(check)) { CHROMA_ERROR(__VA_ARGS__); CHROMA_DEBUGBREAK(); }
	#define CHROMA_CORE_ASSERT(check, ...) if(!(check)) { CHROMA_CORE_ERROR(__VA_ARGS__); CHROMA_DEBUGBREAK(); }
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

#include "Log.h"