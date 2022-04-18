#pragma once

#include "Chroma/Utilities/GUID.h"
#include "Chroma/Core/TypeInfo.h"
#include "Chroma/Utilities/StringHash.h"

namespace Chroma
{


	class AssetManager;

	/// @brief Generic game Asset
	///
	/// Interface for game assets. Each asset controlls its name, id, and path.
	/// Any implementing class is required to provide load/unload/reload functionality.
	struct Asset
	{

		friend class AssetManager;

#define CHROMA_ASSET(typeName, baseTypeName) \
		public: \
			using ClassName = typeName; \
			using BaseClass = baseTypeName; \
			typeName() = default; \
			virtual StringHash GetType() const override { return GetTypeInfoStatic()->GetType(); } \
			virtual const std::string& GetTypeName() const override { return GetTypeInfoStatic()->GetTypeName(); } \
			virtual const TypeInfo* GetTypeInfo() const override { return GetTypeInfoStatic(); } \
			virtual const size_t GetTypeSize() const override { return GetTypeSizeStatic(); } \
			static StringHash GetTypeStatic() { return GetTypeInfoStatic()->GetType(); } \
			static const std::string& GetTypeNameStatic() { return GetTypeInfoStatic()->GetTypeName(); } \
			static const TypeInfo* GetTypeInfoStatic () { static const TypeInfo typeInfoStatic(#typeName, baseTypeName::GetTypeInfoStatic()); return &typeInfoStatic; } \
			static const size_t GetTypeSizeStatic() { return sizeof(#typeName); } \
		private: \
			virtual void NOTYPEINFO() override {} \
		public: \

		virtual ~Asset();

		virtual StringHash GetType() const { return GetTypeInfoStatic()->GetType(); }
		virtual const std::string& GetTypeName() const { return GetTypeInfoStatic()->GetTypeName(); }
		virtual const TypeInfo* GetTypeInfo() const { return GetTypeInfoStatic(); }
		virtual const size_t GetTypeSize() const { return GetTypeSizeStatic(); }
		static StringHash GetTypeStatic() { return GetTypeInfoStatic()->GetType(); }
		static const std::string& GetTypeNameStatic() { return GetTypeInfoStatic()->GetTypeName(); }
		static const TypeInfo* GetTypeInfoStatic() { static const TypeInfo typeInfoStatic("Asset", nullptr); return &typeInfoStatic; }
		static const size_t GetTypeSizeStatic() { return sizeof(Asset); }

		bool IsTypeOf(const TypeInfo * pTypeInfo) const;
		bool IsTypeOf(StringHash type) const;
		template<typename T>
		inline bool IsTypeOf() const { return IsTypeOf(T::GetTypeInfoStatic()); }

		template<typename T>
		static bool IsOfTypeStatic() { return GetTypeStatic() == T::GetTypeStatic(); }

		const std::string GetPath() const { return m_Path; }
		const bool IsLoaded() const { return m_Loaded; }
		GUID GetID() const { return m_ID; }

	protected:
		virtual bool Load() { return true; };
		virtual bool Unload() { return true; };
		virtual bool Reload() { return true; };

		virtual void NOTYPEINFO() = 0;

	private:
		GUID m_ID;
		std::string m_Path;
		bool m_Loaded = true;


	};
}