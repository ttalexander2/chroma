#include "chromapch.h"
#include "Range.h"

float Chroma::Reflection::Properties::Range::transform(const float value)
{
	if (value >= Max) {
		return Max - 1;
	}
	if (value < Min) {
		return Min;
	}
	return value;
}
