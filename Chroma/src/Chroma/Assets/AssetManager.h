#pragma once

#include "Asset.h"

#include <string>
#include "Chroma/Core/Core.h"
#include "Chroma/Utilities/GUID.h"

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

		AssetIterator(std::unordered_map<size_t, Ref<Asset>>* assets, std::vector<size_t>::iterator iterator) :
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
		std::unordered_map<size_t, Ref<Asset>>* m_Assets;
		std::vector<size_t>::iterator m_Iterator;
	};

	template <typename T>
	struct AssetView
	{
		AssetView(std::unordered_map<size_t, Ref<Asset>>* assets, std::vector<size_t>* data) : m_Assets(assets), m_Data(data) {}

		AssetIterator<T> begin() { return AssetIterator<T>(m_Assets, m_Data->begin()); }
		AssetIterator<T> end() { return AssetIterator<T>(m_Assets, m_Data->end()); }

	private:
		std::unordered_map<size_t, Ref<Asset>>* m_Assets;
		std::vector<size_t>* m_Data;
	};

	/// @brief Manages loading/unloading assets.
	///
	/// Class for managing assets during runtime. Handles loading & unloading of assets.
	/// Manages references to assets.
	class AssetManager
	{
	public:

		template<typename T>
		static Ref<T> Get(const std::string& assetPath)
		{

			static_assert(std::is_base_of<Chroma::Asset, T>::value, "Get only works for types derived from Asset!");

			return std::dynamic_pointer_cast<T>(Get(assetPath));
		}

		template<typename T>
		static Ref<T> Create(const std::string& assetPath)
		{

			static_assert(std::is_base_of<Chroma::Asset, T>::value, "Create only works for types derived from Asset!");

			if (Exists(assetPath))
			{
				return Get<T>(assetPath);
			}

			size_t hash = s_Hash(assetPath);

			s_Assets[hash] = CreateRef<T>();
			if (!s_AssetTypes.contains(T::GetTypeStatic()))
				s_AssetTypes[T::GetTypeStatic()] = std::vector<size_t>();
			s_AssetTypes[T::GetTypeStatic()].push_back(hash);
			s_Assets[hash]->m_Path = assetPath;
			s_Assets[hash]->m_Loaded = false;
			s_Assets[hash]->m_Hash = hash;

			return std::dynamic_pointer_cast<T>(Get(assetPath));
		}

		const inline std::unordered_map<size_t, Ref<Asset>>::const_iterator cbegin() const
		{
			return s_Assets.cbegin();
		}

		const inline std::unordered_map<size_t, Ref<Asset>>::const_iterator cend() const
		{
			return s_Assets.cend();
		}

		template<typename T>
		static StringHash GetAssetType()
		{
			static_assert(std::is_base_of<Chroma::Asset, T>::value, "Type T is not derived from Asset!");
			return T::GetTypeStatic();
		}

		
		template <typename T>
		static AssetView<T> View()
		{
			StringHash type = GetAssetType<T>();
			return AssetView<T>(&s_Assets, &s_AssetTypes[type]);
			
		}

		static Ref<Asset> Get(const std::string& assetPath);

		static bool Exists(const std::string& assetPath);

		static bool Load(const std::string& assetPath);

		static bool Unload(const std::string& assetPath);

		static bool Reload(const std::string& assetPath);

		static bool IsLoaded(const std::string& assetPath);

		static void UnloadAll();
		static void UnloadUnused();

	private:
		static std::unordered_map<size_t, Ref<Asset>> s_Assets;
		static std::unordered_map<StringHash, std::vector<size_t>> s_AssetTypes;
		static std::hash<std::string> s_Hash;
	};
}