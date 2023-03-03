#include "chromapch.h"
#include "any.h"

#include "type_data.h"

namespace Chroma::Reflection
{
    any any::try_conversion(uint32_t type_id)
    {
        if (type().valid())
            return type_data::instance().types[type_info._id].conversions[type_id].helper_func(*this);
        return any{};
    }
}