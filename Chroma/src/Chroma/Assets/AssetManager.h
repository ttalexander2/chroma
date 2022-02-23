#pragma once

#include "Asset.h"

#include <string>
#include "Chroma/Core/Core.h"
#include "Chroma/Utilities/GUID.h"

#include "Chroma/Assets/Sprite.h"

#include <unordered_map>

namespace Chroma
{



	template <typename T>
	struct AssetIterator
	{
		static_assert(std::is_base_of<Chroma::Asset, T>::value, "Type T is not derived from Asset!");

		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = Ref<T>;
		using pointer = Ref<T>*;
		using reference = Ref<T>;

		AssetIterator(std::unordered_map<GUID, Ref<Asset>>* assets, std::vector<GUID>::iterator iterator) :
			m_Assets(assets), m_Iterator(iterator)
		{
		}

		reference operator*() const { return std::dynamic_pointer_cast<T>(m_Assets->at(*m_Iterator)); }
		pointer operator->() { return &std::dynamic_pointer_cast<T>(m_Assets->at(*m_Iterator)); }

		//Prefix increment
		AssetIterator& operator++() { m_Iterator++; return *this; }

		//PostFix increment
		AssetIterator operator++(int) { AssetIterator tmp = *this; ++(*this); return tmp; }

		friend bool operator== (const AssetIterator& a, const AssetIterator& b) { return a.m_Iterator == b.m_Iterator && a.m_Assets == b.m_Assets; }
		friend bool operator!= (const AssetIterator& a, const AssetIterator& b) { return a.m_Iterator != b.m_Iterator || a.m_Assets != b.m_Assets; }

	private:
		std::unordered_map<GUID, Ref<Asset>>* m_Assets;
		std::vector<GUID>::iterator m_Iterator;
	};

	template <typename T>
	struct AssetView
	{
		AssetView(std::unordered_map<GUID, Ref<Asset>>* assets, std::vector<GUID>* data) : m_Assets(assets), m_Data(data) {}

		AssetIterator<T> begin() { return AssetIterator<T>(m_Assets, m_Data->begin()); }
		AssetIterator<T> end() { return AssetIterator<T>(m_Assets, m_Data->end()); }

	private:
		std::unordered_map<GUID, Ref<Asset>>* m_Assets;
		std::vector<GUID>* m_Data;
	};

	/// @brief Manages loading/unloading assets.
	///
	/// Class for managing assets during runtime. Handles loading & unloading of assets.
	/// Manages references to assets.
	class AssetManager
	{
	private:
		static std::hash<std::string> s_Hash;
	public:

		template<typename T>
		static Ref<T> Get(const GUID& id)
		{

			static_assert(std::is_base_of<Chroma::Asset, T>::value, "Get only works for types derived from Asset!");

			return std::dynamic_pointer_cast<T>(Get(id));
		}

		template<typename T>
		static Ref<T> Create(const GUID& id, const std::string& path)
		{
			static_assert(std::is_base_of<Chroma::Asset, T>::value, "Create only works for types derived from Asset!");

			if (Exists(id))
			{
				return Get<T>(id);
			}

			size_t type_hash = s_Hash(T::GetTypeNameStatic());
			s_CreateFuncs[type_hash](id, path);

			return std::dynamic_pointer_cast<T>(Get(id));
		}

		static Ref<Asset> Create(const GUID& id, const std::string& path, const std::string& type);

		const inline std::unordered_map<GUID, Ref<Asset>>::const_iterator cbegin() const
		{
			return s_Assets.cbegin();
		}

		const inline std::unordered_map<GUID, Ref<Asset>>::const_iterator cend() const
		{
			return s_Assets.cend();
		}

		template<typename T>
		static StringHash GetAssetType()
		{
			static_assert(std::is_base_of<Chroma::Asset, T>::value, "Type T is not derived from Asset!");
			return T::GetTypeStatic();
		}

		static StringHash GetAssetType(const GUID& id)
		{
			if (!Exists(id))
				return StringHash::Hash("");
			return Get(id)->GetType();
		}

		
		template <typename T>
		static AssetView<T> View()
		{
			StringHash type = GetAssetType<T>();
			return AssetView<T>(&s_Assets, &s_AssetTypes[type]);
			
		}

		template <typename T>
		static void Register()
		{
			static_assert(std::is_base_of<Chroma::Asset, T>::value, "Type T is not derived from Asset!");

			size_t hash = s_Hash(T::GetTypeNameStatic());

			s_CreateFuncs[hash] = [&] (const GUID& id, const std::string& path) {
				s_Assets[id] = CreateRef<T>();
				if (!s_AssetTypes.contains(T::GetTypeStatic()))
					s_AssetTypes[T::GetTypeStatic()] = std::vector<GUID>();
				s_AssetTypes[T::GetTypeStatic()].push_back(id);
				s_Assets[id]->m_Path = path;
				s_Assets[id]->m_Loaded = false;
				s_Assets[id]->m_ID = id;
				s_Paths[s_Hash(path)] = id;
			};

		}

		static Ref<Asset> Get(const GUID& id);
		static bool Exists(const GUID& id);
		static bool Load(const GUID& id);
		static bool Unload(const GUID& id);
		static bool Reload(const GUID& id);
		static bool IsLoaded(const GUID& id);

		static GUID GetID(const std::string& path);
		static std::string GetPath(const GUID& id) { if (!Exists(id)) return""; return s_Assets[id]->GetPath(); }

		static void UnloadAll();
		static void UnloadUnused();
		static std::unordered_map<GUID, Ref<Asset>>& GetAssetMap() { return s_Assets; }

		static void LoadManifest(const std::string& yaml);

	private:
		static std::unordered_map<GUID, Ref<Asset>> s_Assets;
		static std::unordered_map<StringHash, std::vector<GUID>> s_AssetTypes;
		static std::unordered_map<size_t, GUID> s_Paths;
		static std::unordered_map<size_t, std::function<void(const GUID&, const std::string&)>> s_CreateFuncs;

	};
}