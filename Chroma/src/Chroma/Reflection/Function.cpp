#include "chromapch.h"
#include "Function.h"

#include "TypeData.h"
#include "Registry.h"
#include "iterators/argument_container.h"

namespace Chroma::Reflection
{

    Function::Function() : _id(0), _type_id(Registry::resolve<void>().id())
    {

    }


    Function::Function(uint32_t id, uint32_t type_id) : _id(id), _type_id(type_id)
    {

    }

    bool Function::valid() const
    {
        return Registry::valid(_type_id)
               && TypeData::instance().types[_type_id].functions.find(_id)
                  != TypeData::instance().types[_type_id].functions.end();
    }

    bool Function::operator==(const Function &rhs) const
    {
        return _id == rhs._id && _type_id == rhs._type_id;
    }

    uint32_t Function::id() const
    {
        return _id;
    }

    std::string Function::name() const
    {
        if (valid())
            return TypeData::instance().types[_type_id].functions[_id].name;
        return std::string{};
    }

    Reflection::Type Function::type() const
    {
        return Registry::resolve(_type_id);
    }

    size_t Function::arity() const
    {
        if (valid())
            return TypeData::instance().types[_type_id].functions[_id].arity;
        return 0;
    }

    Reflection::Type Function::return_type() const
    {
        if (valid())
            return Reflection::Type(TypeData::instance().types[_type_id].functions[_id].return_type);
        return Registry::resolve<void>();
    }

    Reflection::Type Function::args(size_t index) const
    {
        if (valid() && index < arity())
            return Reflection::Type(TypeData::instance().types[_type_id].functions[_id].arg(index));
        return Registry::resolve<void>();
    }

    Any Function::invoke_internal(Reflection::Handle handle, Any *args, size_t count) const
    {
        if (valid())
            return TypeData::instance().types[_type_id].functions[_id].invoke(std::move(handle), args);
        return Any{};
    }

    argument_container Function::args() const
    {
        return argument_container(_type_id, _id);
    }

	Any Function::user_data(const std::string& key) const
    {
    	uint32_t hash = BasicHash<uint32_t>::hash(key);
    	if (valid())
    	{
    		return TypeData::instance().types[_type_id].functions[_id].user_data[hash];
    	}
    	return Any{};
    }
	
}