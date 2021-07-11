#pragma once
#include <vector>
#include "Entity.h"

namespace Chroma
{
	class SceneNode
	{
	public:
		SceneNode(Entity &entity) : Node(entity) {}

		void AddChild(SceneNode* sceneNode)
		{
			Children.push_back(sceneNode);
		}

	protected:
		Entity Node;
		std::vector<SceneNode*> Children;
	};
}