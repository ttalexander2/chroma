#include "chromapch.h"
#include "type.h"

#include "registry.h"
#include "data.h"
#include "function.h"
#include "iterators/data_container.h"
#include "iterators/function_container.h"
#include "iterators/constructor_container.h"

namespace Chroma::Reflection
{

    bool type::valid() const
    {
        return registry::valid(_id);
    }

    type_id type::id() const
    {
        return _id;
    }

    std::string type::name() const
    {
        if (valid())
            return type_data::instance().types[_id].name;
        return {};
    }

    uint32_t type::hash() const
    {
        return basic_hash<uint32_t>::hash(name());
    }

    bool type::is_integral() const
    {
        if (valid())
            return internal::has_flag(type_data::instance().types[_id].flags, type_flags::is_integral);
        return false;
    }

	bool type::is_floating_point() const
    {
    	if (valid())
    		return internal::has_flag(type_data::instance().types[_id].flags, type_flags::is_floating_point);
    	return false;
    }

	bool type::is_arithmetic() const
    {
    	return is_integral() || is_floating_point();
    }

    bool type::is_array() const
    {
        if (valid())
            return internal::has_flag(type_data::instance().types[_id].flags, type_flags::is_array);
        return false;
    }

    bool type::is_enum() const
    {
        if (valid())
            return internal::has_flag(type_data::instance().types[_id].flags, type_flags::is_enum);
        return false;
    }

    bool type::is_class() const
    {
        if (valid())
            return internal::has_flag(type_data::instance().types[_id].flags, type_flags::is_class);
        return false;
    }

    bool type::is_pointer() const
    {
        if (valid())
            return internal::has_flag(type_data::instance().types[_id].flags, type_flags::is_pointer);
        return false;
    }

    bool type::is_pointer_like() const
    {
        if (valid())
            return internal::has_flag(type_data::instance().types[_id].flags, type_flags::is_pointer_like);
        return false;
    }

    bool type::is_sequence_container() const
    {
        if (valid())
            return internal::has_flag(type_data::instance().types[_id].flags, type_flags::is_sequence_container);
        return false;
    }

    bool type::is_associative_container() const
    {
        if (valid())
            return internal::has_flag(type_data::instance().types[_id].flags, type_flags::is_associative_container);
        return false;
    }

    bool type::is_template_specialization() const
    {
        if (valid())
            return internal::has_flag(type_data::instance().types[_id].flags, type_flags::is_template_specialization);
        return false;
    }

    bool type::is_abstract() const
    {
        if (valid())
            return internal::has_flag(type_data::instance().types[_id].flags, type_flags::is_abstract);
        return false;
    }

    type::type(uint32_t id)
            : _id(id)
    {
    }

    type_flags type::flags() const
    {
        if (valid())
            return type_data::instance().types[_id].flags;
        return type_flags::none;
    }

    type_info *type::info() const
    {
        return &type_data::instance().types[_id];
    }

    type type::underlying_type() const
    {
        if (!valid())
            return registry::resolve<void>();
        return type(type_data::instance().types[_id].underlying_type_id);
    }

    type::type() : _id(internal::type_hash_v<void>)
    {

    }

    Reflection::data type::data(const std::string &name) const
    {
        return Reflection::data(basic_hash<uint32_t>(name), _id);
    }

    Reflection::data type::data(uint32_t id) const
    {
        return Reflection::data(id, _id);
    }

    Reflection::function type::func(const std::string &name) const
    {
        return Reflection::function(basic_hash<uint32_t>(name), _id);
    }

    Reflection::function type::func(uint32_t id) const
    {
        return Reflection::function(id, _id);
    }

    bool type::is_convertible(uint32_t type_id) const
    {
        if (!valid())
            return false;
        return type_data::instance().types[_id].conversions.find(type_id) !=
               type_data::instance().types[_id].conversions.end();
    }

    Reflection::data_container type::data() const
    {
        return Reflection::data_container(_id);
    }

    Reflection::function_container type::func() const
    {
        return Reflection::function_container(_id);
    }

	any type::user_data(uint32_t hash) const
    {
        if (!valid() || type_data::instance().types[_id].user_data.find(hash) == type_data::instance().types[_id].user_data.end())
            return any{};

        return any{type_data::instance().types[_id].user_data[hash]};
    }

    any type::user_data(const std::string& key) const
    {
        return user_data(basic_hash<uint32_t>(key));
    }

	bool type::has_user_data(uint32_t hash) const
    {
    	return valid() && type_data::instance().types[_id].user_data.find(hash) != type_data::instance().types[_id].user_data.end();
    }

	bool type::has_user_data(const std::string& key) const
    {
    	return has_user_data(basic_hash<uint32_t>(key));
    }

    Reflection::constructor type::constructor(uint32_t id) const
    {
        return {id, _id};
    }

    Reflection::constructor_container type::constructor() const
    {
        return Reflection::constructor_container(_id);
    }



	std::vector<uint32_t> type::bases() const
    {
    	if (valid())
    		return type_data::instance().types[_id].bases;
    	return {};
    }
}