#include <type_traits>

#define __REFLECTION_PARSER__

#ifdef __REFLECTION_PARSER__
    #define META_CLASS __attribute__((annotate("class")))
    #define META(...) __attribute__((annotate(#__VA_ARGS__)))
#else
    #define META_CLASS
    #define META(...)
#endif



struct MetaProperty
{

};

struct Range : MetaProperty
{
    template <typename ComparableType, typename = std::enable_if_t<std::is_arithmetic_v<ComparableType>>>
    Range(ComparableType min, ComparableType max)
    {
    }
};




META_CLASS
class Vector
{
public:

    META(Range(4.f, 12.f))
    float x;

    META()
    float y;

    META()
    float z;
};