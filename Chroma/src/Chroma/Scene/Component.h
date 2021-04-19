#pragma once


#include "Chroma/Utilities/Json.h"
#include "Chroma/Scene/ECS.h"
#include "Chroma/Core/Log.h"
#include <string>

namespace Polychrome
{
	class Inspector;
}

namespace Chroma
{

	template <class T>
	class ComponentPool;

	struct Component
	{
		virtual void Serialize() {};
		virtual void Deserialize() {};

		bool operator == (const Component& other)
		{
			return this->GetComparisonID() == other.GetComparisonID();
		}

		const virtual std::string Name() const = 0;

		virtual void DrawImGui() {};

	private:

		const virtual bool IsTag() const { return false; }
		const virtual bool IsTransform() const { return false; }

		unsigned int comparison_id = 0;


		void SetComparisonID()
		{
			comparison_id = component_counter;
			component_counter++;
		}

		const unsigned int GetComparisonID() const
		{
			return comparison_id;
		}



		static unsigned int component_counter;

		template <class T>
		friend class ComponentPool;
		friend class Polychrome::Inspector;
		friend class Tag;
		friend class Transform;
	};
}

