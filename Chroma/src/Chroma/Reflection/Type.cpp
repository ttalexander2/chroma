#include "chromapch.h"
#include "Type.h"
#include "Type.inl"

#include "Registry.h"
#include "Data.h"
#include "Function.h"
#include "iterators/data_container.h"
#include "iterators/function_container.h"
#include "iterators/constructor_container.h"

namespace Chroma::Reflection
{

    bool Type::valid() const
    {
        return Registry::valid(_id);
    }

    TypeId Type::id() const
    {
        return _id;
    }

    std::string Type::name() const
    {
        if (valid())
            return TypeData::instance().types[_id].name;
        return {};
    }

    uint32_t Type::hash() const
    {
        return BasicHash<uint32_t>::hash(name());
    }

    bool Type::is_integral() const
    {
        if (valid())
            return Internal::has_flag(TypeData::instance().types[_id].flags, TypeFlags::is_integral);
        return false;
    }

	bool Type::is_floating_point() const
    {
    	if (valid())
    		return Internal::has_flag(TypeData::instance().types[_id].flags, TypeFlags::is_floating_point);
    	return false;
    }

	bool Type::is_arithmetic() const
    {
    	return is_integral() || is_floating_point();
    }

    bool Type::is_array() const
    {
        if (valid())
            return Internal::has_flag(TypeData::instance().types[_id].flags, TypeFlags::is_array);
        return false;
    }

    bool Type::is_enum() const
    {
        if (valid())
            return Internal::has_flag(TypeData::instance().types[_id].flags, TypeFlags::is_enum);
        return false;
    }

    bool Type::is_class() const
    {
        if (valid())
            return Internal::has_flag(TypeData::instance().types[_id].flags, TypeFlags::is_class);
        return false;
    }

    bool Type::is_pointer() const
    {
        if (valid())
            return Internal::has_flag(TypeData::instance().types[_id].flags, TypeFlags::is_pointer);
        return false;
    }

    bool Type::is_pointer_like() const
    {
        if (valid())
            return Internal::has_flag(TypeData::instance().types[_id].flags, TypeFlags::is_pointer_like);
        return false;
    }

    bool Type::is_sequence_container() const
    {
        if (valid())
            return Internal::has_flag(TypeData::instance().types[_id].flags, TypeFlags::is_sequence_container);
        return false;
    }

    bool Type::is_associative_container() const
    {
        if (valid())
            return Internal::has_flag(TypeData::instance().types[_id].flags, TypeFlags::is_associative_container);
        return false;
    }

    bool Type::is_template_specialization() const
    {
        if (valid())
            return Internal::has_flag(TypeData::instance().types[_id].flags, TypeFlags::is_template_specialization);
        return false;
    }

    bool Type::is_abstract() const
    {
        if (valid())
            return Internal::has_flag(TypeData::instance().types[_id].flags, TypeFlags::is_abstract);
        return false;
    }

    Type::Type(uint32_t id)
            : _id(id)
    {
    }

    TypeFlags Type::flags() const
    {
        if (valid())
            return TypeData::instance().types[_id].flags;
        return TypeFlags::none;
    }

    TypeInfo *Type::info() const
    {
        return &TypeData::instance().types[_id];
    }

    Type Type::underlying_type() const
    {
        if (!valid())
            return Registry::resolve<void>();
        return Type(TypeData::instance().types[_id].underlying_type_id);
    }

    Type::Type() : _id(Internal::type_hash_v<void>)
    {

    }

    Reflection::Data Type::data(const std::string &name) const
    {
        return Reflection::Data(BasicHash<uint32_t>(name), _id);
    }

    Reflection::Data Type::data(uint32_t id) const
    {
        return Reflection::Data(id, _id);
    }

    Reflection::Function Type::func(const std::string &name) const
    {
        return Reflection::Function(BasicHash<uint32_t>(name), _id);
    }

    Reflection::Function Type::func(uint32_t id) const
    {
        return Reflection::Function(id, _id);
    }

    bool Type::is_convertible(uint32_t type_id) const
    {
        if (!valid())
            return false;
        return TypeData::instance().types[_id].conversions.find(type_id) !=
               TypeData::instance().types[_id].conversions.end();
    }

    Reflection::data_container Type::data() const
    {
        return Reflection::data_container(_id);
    }

    Reflection::function_container Type::func() const
    {
        return Reflection::function_container(_id);
    }

    Reflection::Constructor Type::constructor(uint32_t id) const
    {
        return {id, _id};
    }

    Reflection::constructor_container Type::constructor() const
    {
        return Reflection::constructor_container(_id);
    }



	std::vector<uint32_t> Type::bases() const
    {
    	if (valid())
    		return TypeData::instance().types[_id].bases;
    	return {};
    }

	Any Type::user_data(uint32_t hash) const
    {
    	if (!valid() || TypeData::instance().types[_id].user_data.find(hash) == TypeData::instance().types[_id].user_data.end())
    		return Any{};

    	return Any{TypeData::instance().types[_id].user_data[hash]};
    }

	Any Type::user_data(const std::string& key) const
    {
    	return user_data(BasicHash<uint32_t>(key));
    }

	bool Type::has_user_data(uint32_t hash) const
    {
    	return valid() && TypeData::instance().types[_id].user_data.find(hash) != TypeData::instance().types[_id].user_data.end();
    }

	bool Type::has_user_data(const std::string& key) const
    {
    	return has_user_data(BasicHash<uint32_t>(key));
    }

	bool Type::has_metadata(uint32_t type_id) const
    {
    	return valid() && TypeData::instance().types[_id].metadata.find(type_id) != TypeData::instance().types[_id].metadata.end();
    }

	Any Type::get_metadata(uint32_t type_id) const
	{
    	return TypeData::instance().types[_id].metadata[type_id];
	}
}
