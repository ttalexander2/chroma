#pragma once
#include <Chroma/Scene/Scene.h>
#include <Chroma/Scene/Entity.h>

namespace Chroma
{
	struct SpriteLayer
	{
		std::string Name;
		float Z;
	};

	class World
	{
	public:
		static void LoadScene(Chroma::Scene& Scene);

		static std::vector<SpriteLayer> SpriteLayers;
	};
}