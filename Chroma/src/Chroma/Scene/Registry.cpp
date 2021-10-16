#include "chromapch.h"
#include "Registry.h"

namespace Chroma
{
	eid Registry::Create()
	{
		eid id = entityCounter++;
		entities.emplace(id);
		this->Emplace<Transform>(id);
		return id;
	}
}