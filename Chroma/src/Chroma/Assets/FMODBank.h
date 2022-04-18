#pragma once

#include "Asset.h"


namespace Chroma
{

	class FMODBank : public Asset
	{
		CHROMA_ASSET(FMODBank, Asset);

	protected:
		bool Load() override;
		bool Unload() override;
		bool Reload() override;
	};
}