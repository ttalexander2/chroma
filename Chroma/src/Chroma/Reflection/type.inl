//
// Created by Thomas on 1/17/2023.
//

#pragma once

#include "any.h"
#include "type.h"

namespace Chroma::Reflection
{

    template<typename KeyType, typename>
    any type::user_data(KeyType &&key) const
    {
        return user_data(static_cast<uint32_t>(key));
    }

}
