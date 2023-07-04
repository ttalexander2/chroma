#pragma once

#include "../MetaData.h"

namespace Chroma::Reflection::Properties
{
	class Range final : public MetaData
	{
	public:
		Range(const float min, const float max) : Min(min), Max(max) {}
		Range(Range&) = default;
		Range(Range&&) = default;
		Range& operator=(const Range&) = delete;
		Range& operator=(Range&&) = delete;
		~Range() override = default;

		float transform(float value);

		const float Min;
		const float Max;
	};
}

