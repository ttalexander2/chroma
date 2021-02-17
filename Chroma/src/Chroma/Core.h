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