#pragma once


#include "Chroma/Core/Log.h"
#include <string>
#include "Chroma/Utilities/Yaml.h"
#include "yaml-cpp/node/convert.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/node/parse.h"

namespace Polychrome
{
	class Inspector;
}

namespace Chroma
{

	/// @brief Game object Component interface.
	///
	/// In order to implement a new component, Name() MUST be implemented.
	/// In order for the component to interface with the editor, DrawImGui() can be overidden.
	struct Component
	{
		/// @brief Function to serialize the scene to YAML.
		/// 
		/// Serialization occurs in a Key/Value map for data members.
		/// Example:
		/// ```{.cpp}
		/// out << YAML::Key << "Data";
		/// out << YAML::Value << Data;
		/// ```
		/// @param out YAML emitter to write to.
		virtual void Serialize(YAML::Emitter& out) {};

		/// @brief Function to deserialize the scene from YAML
		/// 
		/// Serialization occurs in a Key/Value map for data members.
		/// Example:
		/// ```{.cpp}
		/// auto val = node["Data"];
		/// if (val)
		/// {
		/// 	Data = val.as<float>();
		/// }
		/// ```
		/// @param out YAML emitter to read from.
		virtual void Deserialize(YAML::Node& node) {};

		/// @brief Component comparison operator.
		/// @param other Other Component to compare.
		/// @return Returns whether the components are identical.
		bool operator == (const Component& other)
		{
			return this->GetUniqueID() == other.GetUniqueID();
		}

		/// @brief Returns Name of the component.
		/// 
		/// This must be overidden in order for the scene to function correctly.
		/// @return name of the component.
		const virtual std::string Name() const = 0;

		/// @brief Function to draw the ImGui component widget.
		virtual void DrawImGui() {};

		/// @brief Whether multiple instances of the component can exist for a single entity.
		/// 
		/// Override this to only allow one instance of the component for a single entity.
		/// @return Returns true if it allows multiple.
		const virtual bool AllowMultiple() const { return true; };

		/// @brief Gets a unique identifier for a component.
		/// @return Components' ID.
		const unsigned int GetUniqueID() const
		{
			return comparison_id;
		}


		void DoSerialize(YAML::Emitter& out);

	protected:

		void DrawComponentValue(std::string label);
		bool DrawComponentValueCollapsible(std::string label);

	private:
		void BeginSerialize(YAML::Emitter& out);
		void EndSerialize(YAML::Emitter& out);

		/// @brief Checks whether the Component is a Tag.
		const virtual bool IsTag() const { return false; }
		/// @brief Checks whether the Component is a Transform.
		const virtual bool IsTransform() const { return false; }

		/// @brief Component ID (used for comparison)
		unsigned int comparison_id = 0;

		/// @brief Whether the component is open in the editor inspector.
		bool editor_inspector_open = true;
		/// @brief Whether the compoennt is visible in the editor.
		bool editor_visible = true;


		/// @brief Set the component's comparison ID.
		void SetComparisonID()
		{
			comparison_id = component_counter;
			component_counter++;
		}

		/// @brief Counter for component IDs.
		static unsigned int component_counter;

		friend class Polychrome::Inspector;
		friend struct Tag;
		friend struct Transform;
	};
				
}

