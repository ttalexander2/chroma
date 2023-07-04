#pragma once

#include "MetaData.h"
#include "Range.h"
#include "Chroma/Utilities/ReflectMacros.h"

/// So this small mess of macros is simply to enable intellisense suggestions
/// and enforce that the arguments passed into the meta macro are within
/// the Chroma::Reflection::Properties namespace.

#if defined(__REFLECTION_PARSER__)
	// This is used for collecting data by the header parsing tool.
	#define META(...) __attribute__((annotate(#__VA_ARGS__)))
#else
	#if defined(__RESHARPER__)
		// Trick jetbrains into showing intellisense information by creating individual static variables. 
		// This only affects the code analysis engine. 
		#define META_BODY_MACRO_COMBINE_INNER(A, B, C, D, E, ARG) static inline auto A##B##C##D##E = ARG;
		#define META_BODY_MACRO_COMBINE(A, B, C, D, E, ARG) META_BODY_MACRO_COMBINE_INNER(A, B, C, D, E, ARG)
		#define META_EMPTY(ARG, N) META_BODY_MACRO_COMBINE(_META_, __LINE__, _, N, _, ARG)
		#define META_APPLY_NAMESPACE(ARG, N) META_EMPTY(Chroma::Reflection::Properties::ARG, N)
		#define META(...) REFLECT_PP_MAP_INDEX(META_APPLY_NAMESPACE, __VA_ARGS__)
	#elif defined(__INTELLISENSE__)
		#define META(...)
		// Trick visual studio into including reflection property symbols for use in macros.
		using namespace Chroma::Reflection::Properties;
	#else
		// Macros compile to empty statements in the actual build.
		#define META(...)
	#endif

#endif