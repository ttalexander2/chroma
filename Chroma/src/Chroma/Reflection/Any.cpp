#include "chromapch.h"
#include "Any.h"

#include "TypeData.h"

namespace Chroma::Reflection
{
    Any Any::try_conversion(uint32_t type_id)
    {
        if (type().valid())
            return TypeData::instance().types[type_info._id].conversions[type_id].helper_func(*this);
        return Any{};
    }
}