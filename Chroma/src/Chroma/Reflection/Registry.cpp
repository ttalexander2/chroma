#include "chromapch.h"
#include "Registry.h"

#include <limits>
#include "iterators/type_container.h"

namespace Chroma::Reflection
{

    Type Registry::resolve(const std::string &name) noexcept
    {
        return Type(id_from_name(name));
    }

    Type Registry::resolve(uint32_t id) noexcept
    {
        return Type(id);
    }

    type_container Registry::resolve() noexcept
    {
        return {};
    }

    bool Registry::valid(uint32_t id)
    {
        return TypeData::instance().types.find(id) != TypeData::instance().types.end();
    }

    bool Registry::valid(const std::string &name)
    {
        auto id = id_from_name(name);
        return valid(id);
    }

    uint32_t Registry::id_from_name(const std::string &name)
    {
        auto val = TypeData::instance().type_aliases.find(id_hash::hash(name));
        if (val != TypeData::instance().type_aliases.end())
        {
            return val->second;
        }
        return std::numeric_limits<uint32_t>::max();
    }

} // mirr