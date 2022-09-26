#include "chromapch.h"

#include "Sprite.h"
#include "Chroma/IO/FileSystem.h"
#include "Chroma/IO/File.h"

namespace Chroma
{
	/// @brief Load the sprite from file.
	/// 
	/// This function loads the sprite from a file, if it exists.
	/// It supports Aseprite(.ase) sprites/animations as well as static .png, and .jpg files.
	/// 
	/// @return Returns true if the sprite was loaded succesfully.
	bool Sprite::Load()
	{
		if (!FileSystem::Exists(GetPath()))
		{
			CHROMA_CORE_ERROR("Could not load sprite! The following path does not exist: {}", GetPath());
			return false;
		}

		if (FileSystem::HasFileExtension(GetPath(), ".ase") || FileSystem::HasFileExtension(GetPath(), ".aseprite"))
		{
			auto a = Aseprite(GetPath());

			auto data = new Color[a.width * a.height];
			float width = 0.f;
			float height = 0.f;

			for (auto &frame : a.frames)
			{
				Frame fr;
				fr.Durration = frame.duration;
				fr.Texture = Texture2D::Create(a.width, a.height);
				frame.image.FlipVertically();
				frame.image.GetData(data);
				fr.Texture->SetData(data, sizeof(Color) * a.width * a.height);
				Frames.push_back(fr);

				if (a.width > width)
					width = static_cast<float>(a.width);
				if (a.height > height)
					height = static_cast<float>(a.height);
			}

			m_Size = { width, height };
			/*
			if (a.frames.size() > 1 && a.tags.size() == 0)
			{
				Chroma::Sprite::Animation anim;
				anim.Direction = Sprite::LoopDirection::Forward;
				anim.Tag = "Default";
				anim.Start = 1;
				anim.End = a.frames.size();
				Animations.push_back(anim);
			}
			*/

			for (auto &tag : a.tags)
			{
				Animation anim;
				anim.Direction = static_cast<LoopDirection>(tag.loops);
				anim.Tag = tag.name;
				anim.Start = tag.from;
				anim.End = tag.to;
				anim.LabelColor = tag.color;
				Animations.push_back(anim);
			}

			delete[] data;
			a.Dispose();

			return true;
		}
		if (FileSystem::HasFileExtension(GetPath(), ".png"))
		{
			Frame fr;
			fr.Texture = Texture2D::Create(GetPath());
			Frames.push_back(fr);
			m_Size = { fr.Texture->GetWidth(), fr.Texture->GetHeight() };
			return true;
		}
		if (std::filesystem::path(GetPath()).extension() == ".jpg")
		{
			Frame fr;
			fr.Texture = Texture2D::Create(GetPath());
			Frames.push_back(fr);
			m_Size = { fr.Texture->GetWidth(), fr.Texture->GetHeight() };
			return true;
		}
		CHROMA_CORE_ERROR("Sprite type not supported: {}", std::filesystem::path(GetPath()).extension());

		return false;
	}

	/// @brief Unload the sprite.
	/// 
	/// This function unloads the sprite. 
	/// The sprite's textures are deleted and the frame/animation information is deleted.
	/// 
	/// @return Returns true if the sprite was loaded succesfully.
	bool Sprite::Unload()
	{
		for (Frame f : Frames)
		{
			f.Texture.reset();
		}

		Frames.clear();
		Animations.clear();
		return true;
	}

	/// @brief Reloads the sprite from file.
	/// 
	/// This function unloads and reloads the sprite from a file, if it exists.
	/// It supports Aseprite(.ase) sprites/animations as well as static .png, and .jpg files.
	/// 
	/// @return Returns true if the sprite was unloaded and reloaded succesfully.
	bool Sprite::Reload()
	{
		for (Frame f : Frames)
		{
			f.Texture.reset();
		}
		Frames.clear();
		Animations.clear();
		return Load();
	}
}
