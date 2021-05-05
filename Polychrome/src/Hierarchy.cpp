#include "Hierarchy.h"

#include "imgui.h"

#include "EditorApp.h"
#include <Chroma/Components/Tag.h>
#include <Chroma/Scene/Component.h>
#include <Chroma/Scene/ECS.h>


namespace Polychrome
{
	bool Hierarchy::Open = true;
	Chroma::EntityRef Hierarchy::SelectedEntity = Chroma::EntityRef(Chroma::ENTITY_NULL);

	void Hierarchy::Draw()
	{
		ImGui::Begin("Scene", &Hierarchy::Open);

		Chroma::Scene* scene = Chroma::Application::Get().m_ActiveScene;

		auto view = scene->GetEntities();

		for (Chroma::EntityID e : view)
		{
			Chroma::ComponentRef<Chroma::Tag> t = scene->GetComponent<Chroma::Tag>(e);
			
			if (ImGui::Selectable(t->EntityName.c_str(), Hierarchy::SelectedEntity.GetID() == e))
				Hierarchy::SelectedEntity = Chroma::EntityRef(e, *scene);
		}
		

		ImGui::End();
	}
}


