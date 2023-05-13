//
// Created by Thomas on 1/17/2023.
//

#pragma once

#include "Any.h"
#include "Type.h"

namespace Chroma::Reflection
{

    template<typename KeyType, typename>
    Any Type::user_data(KeyType &&key) const
    {
        return user_data(static_cast<uint32_t>(key));
    }

}
