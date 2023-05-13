#include "chromapch.h"
#include "Data.h"

#include <utility>

#include "type_data.h"
#include "Registry.h"

namespace Chroma::Reflection
{
    Data::Data() : _id(0), _owner(Registry::resolve<void>().id())
    {

    }

    Data::Data(uint32_t id, uint32_t owner) : _id(id), _owner(owner)
    {

    }

    bool Data::valid() const
    {
        return Registry::valid(_owner)
               && type_data::instance().types[_owner].data.find(_id)
                  != type_data::instance().types[_owner].data.end();
    }

    uint32_t Data::id() const
    {
        return _id;
    }

    std::string Data::name() const
    {
        if (valid())
            return type_data::instance().types[_owner].data[_id].name;
        return std::string{};
    }

    Reflection::Type Data::type() const
    {
    	if (valid())
    		return Reflection::Type(type_data::instance().types[_owner].data[_id].type_id);
    	return Registry::resolve<void>();
    }

    bool Data::is_const() const
    {
        if (valid())
            return Internal::has_flag(type_data::instance().types[_owner].data[_id].flags, data_flags::is_const);
        return false;
    }

    bool Data::is_static() const
    {
        if (valid())
            return Internal::has_flag(type_data::instance().types[_owner].data[_id].flags, data_flags::is_static);
        return false;
    }

    Any Data::get(Reflection::Handle handle) const
    {
        if (!valid())
            return Any{};
        return type_data::instance().types[_owner].data[_id].get(std::move(handle));
    }

    bool Data::set(Reflection::Handle& handle, Any value) const
    {
        if (!valid())
            return false;
        return type_data::instance().types[_owner].data[_id].set(handle, std::move(value));
    }

    bool Data::operator==(const Data &rhs) const
    {
        return _id == rhs._id && _owner == rhs._owner;
    }

    Reflection::Type Data::owner() const
    {
    	return Reflection::Type(_owner);
    }

    Any Data::user_data(uint32_t hash) const
    {
        if (valid())
        {
            return type_data::instance().types[_owner].data[_id].user_data[hash];
        }
        return Any{};
    }
	
	Any Data::user_data(const std::string& key) const
    {
    	return user_data(BasicHash<uint32_t>(key));
    }

	bool Data::has_user_data(uint32_t hash) const
    {
    	return valid() && type_data::instance().types[_owner].data[_id].user_data.find(hash) != type_data::instance().types[_owner].data[_id].user_data.end();
    }

	bool Data::has_user_data(const std::string& key) const
    {
    	return has_user_data(BasicHash<uint32_t>(key));
    }


}