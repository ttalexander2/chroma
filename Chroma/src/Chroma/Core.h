#pragma once

#ifdef CHROMA_PLATFORM_WINDOWS
	#ifdef CHROMA_BUILD_DLL
		#define CHROMA_API __declspec(dllexport)
	#else
		#define	CHROMA_API __declspec(dllimport)	
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

#define BIT(x) (1 << x)