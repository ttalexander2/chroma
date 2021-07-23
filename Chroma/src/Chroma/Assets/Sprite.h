#pragma once
#include <Chroma/Renderer/Texture.h>
#include "Chroma/Assets/Asset.h"
#include <Chroma/Images/Aseprite.h>

namespace Chroma
{
	struct Sprite: public Asset
	{
		Sprite() = default;
		Sprite(const Sprite&) = default;
		Sprite(const std::string& path) : Path(path) 
		{
		}

		~Sprite()
		{
			Unload();
		}



		enum class LoopDirection
		{
			Forward = 0,
			Reverse = 1,
			PingPong = 2
		};

		struct Frame
		{
			unsigned int Durration = 100;
			Ref<Texture2D> Texture;
		};

		struct Animation
		{
			std::string Tag;
			LoopDirection Direction = LoopDirection::Forward;
			int Start;
			int End;
			Chroma::Color LabelColor;
		};

		std::string Path;
		std::vector<Animation> Animations;
		std::vector<Frame> Frames;

		bool Animated() { return Animations.size() > 0; }

		void Load() override
		{
			if (std::filesystem::path(Path).extension() == ".ase")
			{
				Aseprite a = Aseprite(Path);

				Chroma::Color* data = new Chroma::Color[a.width * a.height];

				for (auto& frame : a.frames)
				{
					Frame fr;
					fr.Durration = frame.duration;
					fr.Texture = Chroma::Texture2D::Create(a.width, a.height);
					frame.image.FlipVertically();
					frame.image.GetData(data);
					fr.Texture->SetData(data, sizeof(Chroma::Color) * a.width * a.height);
					Frames.push_back(fr);
				}

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

				for (auto& tag : a.tags)
				{
					Chroma::Sprite::Animation anim;
					anim.Direction = (Sprite::LoopDirection)tag.loops;
					anim.Tag = tag.name;
					anim.Start = tag.from;
					anim.End = tag.to;
					anim.LabelColor = tag.color;
					Animations.push_back(anim);
				}
			}
			else if (std::filesystem::path(Path).extension() == ".png")
			{
				Frame fr;
				fr.Texture = Chroma::Texture2D::Create(Path);
				Frames.push_back(fr);
			}
			else if (std::filesystem::path(Path).extension() == ".jpg")
			{
				Frame fr;
				fr.Texture = Chroma::Texture2D::Create(Path);
				Frames.push_back(fr);
			}


		}

		void Unload() 
		{
			for (Frame f : Frames)
			{
				f.Texture.reset();
			}

			Frames.clear();
			Animations.clear();
		};

		void Reload()
		{
			for (Frame f : Frames)
			{
				f.Texture.reset();
			}
			Frames.clear();
			Animations.clear();
			Load();
		}
	};
}