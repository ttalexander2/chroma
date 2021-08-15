#include "chromapch.h"
#include "Bindings.h"

#include "Chroma/Math/Math.h"
#include "glm/gtx/string_cast.hpp"
#include <sol/sol.hpp>
#include <fmt/format.h>

#include "Chroma/Scene/Entity.h"
#include <Chroma/Components/Tag.h>
#include <Chroma/Components/Transform.h>
#include <Chroma/Components/AudioSource.h>
#include <Chroma/Components/BoxCollider2D.h>
#include <Chroma/Components/CircleCollider2D.h>
#include <Chroma/Components/SpriteRenderer.h>


namespace Chroma
{

	void Bindings::BindAllTheThings(sol::state* lua)
	{
		BindMath(lua);
		BindEntity(lua);
		BindComponents(lua);
	}

	void Bindings::BindEntity(sol::state* lua)
	{
		lua->new_usertype<Entity>("Entity",

			"Entity", sol::no_constructor,

			//Properties
			"ID", sol::readonly_property(&Entity::GetID),
			"Scene", sol::readonly_property(&Entity::GetScene),
			"Name", sol::property(&Entity::GetName, &Entity::SetName),

			//Methods
			"Valid", [](Entity& e) {return &e.GetScene() != nullptr && e.GetID() != ENTITY_NULL; },

			"HasTag", &Entity::HasComponent<Tag>,
			"HasTransform", & Entity::HasComponent<Transform>,
			"HasAudioSource", & Entity::HasComponent<AudioSource>,
			"HasBoxCollider2D", &Entity::HasComponent<BoxCollider2D>,
			"HasCircleCollider2D", &Entity::HasComponent<CircleCollider2D>,
			"HasSpriteRenderer", &Entity::HasComponent<SpriteRenderer>,

			"AddTag", [](Entity& e) { return e.AddComponent<Tag>(); },
			"AddTransform", [](Entity& e) { return e.AddComponent<Transform>(); },
			"AddAudioSource", [](Entity& e) { return e.AddComponent<AudioSource>(); },
			"AddBoxCollider2D", [](Entity& e) { return e.AddComponent<BoxCollider2D>(); },
			"AddCircleCollider2D", [](Entity& e) { return e.AddComponent<CircleCollider2D>(); },
			"AddSpriteRenderer", [](Entity& e) { return e.AddComponent<SpriteRenderer>(); },

			"GetTag", [](Entity& e) { return e.GetComponent<Tag>(); },
			"GetTransform", [](Entity& e) { return e.GetComponent<Transform>(); },
			"GetAudioSource", [](Entity& e) { return e.GetComponent<AudioSource>(); },
			"GetBoxCollider2D", [](Entity& e) { return e.GetComponent<BoxCollider2D>(); },
			"GetCircleCollider2D", [](Entity& e) { return e.GetComponent<CircleCollider2D>(); },
			"GetSpriteRenderer", [](Entity& e) { return e.GetComponent<SpriteRenderer>(); },


			//Metamethods
			sol::meta_function::to_string, [](Entity& e) { return fmt::format("{} (Entity {})", e.GetName(), e.GetID()); },
			sol::meta_function::equal_to, &Entity::operator==,
			sol::meta_function::less_than, &Entity::operator<

		);
	}

	void Bindings::BindComponents(sol::state* lua)
	{
		lua->new_usertype<Component>("Component",
			"Component", sol::no_constructor,

			//Properties
			"ID", sol::readonly_property(&Component::GetUniqueID),
			"AllowsMultiple", sol::readonly_property(&Component::AllowMultiple),
			"Name", sol::readonly_property(&Component::Name)
		);

		lua->new_usertype<AudioSource>("AudioSource",
			"AudioSource", sol::no_constructor,
			sol::base_classes, sol::bases<Component>(),
			//Members
			"Event", &AudioSource::Event,
			"Mute", &AudioSource::Mute,
			"PlayOnInit", &AudioSource::PlayOnInit

		);

		lua->new_usertype<BoxCollider2D>("BoxCollider2D",
			"BoxCollider2D", sol::no_constructor,
			sol::base_classes, sol::bases<Component>(),
			"Bounds", &BoxCollider2D::Bounds,
			"Offset", &BoxCollider2D::Offset

		);

	
		lua->new_usertype<CircleCollider2D>("CircleCollider2D",
			"CircleCollider2D", sol::no_constructor,
			sol::base_classes, sol::bases<Component>(),
			"Radius", &CircleCollider2D::Radius,
			"Offset", &CircleCollider2D::Offset
		);
	

		lua->new_usertype<SpriteRenderer>("SpriteRenderer",
			"SpriteRenderer", sol::no_constructor,
			sol::base_classes, sol::bases<Component>(),

			//Properties
			"RenderColor", &SpriteRenderer::Color,
			"Offset", &SpriteRenderer::Offset,
			"Layer", &SpriteRenderer::Layer,
			"PlayOnStart", &SpriteRenderer::PlayOnStart,
			"Playing", &SpriteRenderer::Playing,
			"Loop", &SpriteRenderer::Loop,
			"SpeedMultipllier", &SpriteRenderer::SpeedMultiplier,

			//Methods
			"GetSpriteID", &SpriteRenderer::GetSpriteID,
			"SetSprite", &SpriteRenderer::SetSprite,
			"SetAnimation", sol::resolve<void(const std::string&)>(&SpriteRenderer::SetAnimation),
			"SetAnimation", sol::resolve<void(unsigned int)>(&SpriteRenderer::SetAnimation),
			"GetAnimation", &SpriteRenderer::GetAnimation,
			"GetAnimationName", &SpriteRenderer::GetAnimationName,
			"GetCurrentFrame", &SpriteRenderer::GetCurrentFrame,
			"SetCurrentFrame", &SpriteRenderer::SetCurrentFrame,
			"RestartAnimation", &SpriteRenderer::RestartAnimation
		);


		lua->new_usertype<Transform>("Transform",
			"Transform", sol::no_constructor,
			sol::base_classes, sol::bases<Component>(),

			"Position", &Transform::Position,
			"Rotation", &Transform::Rotation,
			"Scale", &Transform::Scale

		);
	}

	void Bindings::BindScene(sol::state* lua)
	{
		
	}

	void Bindings::BindMath(sol::state* lua)
	{
		BindVec2(lua);
		BindIVec2(lua);
		BindVec3(lua);
		BindIVec3(lua);
		BindVec4(lua);
		BindIVec4(lua);
	}

	void Bindings::BindVec2(sol::state* lua)
	{
		lua->new_usertype<Math::vec2>("Vec2",

			//Members
			"X", &Math::vec2::x,
			"Y", &Math::vec2::y,

			//Methods
			"Length", &Math::vec2::length,
			"Normalize", [](Math::vec2& v) { v = glm::normalize(v); return v; },
			"Distance", [](Math::vec2& v1, Math::vec2& v2) { return glm::distance(v1, v2); },

			//Metamethods
			sol::meta_function::to_string, [](Math::vec2& v) {return fmt::format("Vec2({}, {})", v.x, v.y); },
			sol::meta_function::index, [](Math::vec2& v, const int index) { if (index < 0 or index > 2) return 0.0f; return v[index]; },
			sol::meta_function::new_index, [](Math::vec2& v, const int index, double x) { if (index < 0 or index > 2) return; v[index] = x; },
			sol::meta_function::equal_to, [](Math::vec2& v1, Math::vec2& v2) { return glm::operator==<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::subtraction, [](Math::vec2& v1, Math::vec2& v2) { return glm::operator-<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::addition, [](Math::vec2& v1, Math::vec2& v2) { return glm::operator+<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::division, [](Math::vec2& v1, Math::vec2& v2) { return glm::operator/<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::multiplication, [](Math::vec2& v1, Math::vec2& v2) { return glm::operator*<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::unary_minus, [](Math::vec2& v1) { return glm::operator-<float, glm::packed_highp>(v1);  }

		);

		//Constructors
		lua->set_function("Vec2", []() {return Math::ivec2(); });
		lua->set_function("Vec2", sol::overload([](float x, float y) {return Math::vec2(x, y); }));

		//Static functions
		lua->set_function("Vec2_Distance", [](Math::vec2& v1, Math::vec2& v2) { return glm::distance(v1, v2); });

	}

	void Bindings::BindIVec2(sol::state* lua)
	{
		lua->new_usertype<Math::ivec2>("IVec2",

			//Members
			"X", &Math::ivec2::x,
			"Y", &Math::ivec2::y,

			//Methods
			//"Length", &Math::ivec2::length,
			//"Distance", [](Math::ivec2& v1, Math::ivec2& v2) { return glm::distance(v1, v2); },

			//Metamethods
			sol::meta_function::to_string, [](Math::ivec2& v) {return fmt::format("IVec2({}, {})", v.x, v.y); },
			sol::meta_function::index, [](Math::ivec2& v, const int index) { if (index < 0 or index > 2) return 0; return v[index]; },
			sol::meta_function::new_index, [](Math::ivec2& v, const int index, double x) { if (index < 0 or index > 2) return; v[index] = x; },
			sol::meta_function::equal_to, [](Math::ivec2& v1, Math::ivec2& v2) { return glm::operator==<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::subtraction, [](Math::ivec2& v1, Math::ivec2& v2) { return glm::operator-<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::addition, [](Math::ivec2& v1, Math::ivec2& v2) { return glm::operator+<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::division, [](Math::ivec2& v1, Math::ivec2& v2) { return glm::operator/<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::multiplication, [](Math::ivec2& v1, Math::ivec2& v2) { return glm::operator*<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::unary_minus, [](Math::ivec2& v1) { return glm::operator-<int, glm::packed_highp>(v1);  }

		);

		//Constructors
		lua->set_function("IVec2", []() {return Math::ivec2(); });
		lua->set_function("IVec2", sol::overload([](int x, int y, int z) {return Math::ivec2(x, y); }));

		//Static functions
		//lua->set_function("IVec2_Distance", [](Math::ivec2& v1, Math::ivec2& v2) { return glm::distance(v1, v2); });
	}

	void Bindings::BindVec3(sol::state* lua)
	{
		lua->new_usertype<Math::vec3>("Vec3",

			//Members
			"X", &Math::vec3::x,
			"Y", &Math::vec3::y,
			"Z", &Math::vec3::z,

			//Methods
			"Length", &Math::vec3::length,
			"Cross", [](Math::vec3& v1, Math::vec3& v2) { return glm::cross(v1, v2); },
			"Normalize", [](Math::vec3& v) { v = glm::normalize(v); return v; },
			"Distance", [](Math::vec3& v1, Math::vec3& v2) { return glm::distance(v1, v2); },

			//Metamethods
			sol::meta_function::to_string, [](Math::vec3& v) {return fmt::format("Vec3({}, {}, {})", v.x, v.y, v.z); },
			sol::meta_function::index, [](Math::vec3& v, const int index) { if (index < 0 or index > 2) return 0.0f; return v[index]; },
			sol::meta_function::new_index, [](Math::vec3& v, const int index, double x) { if (index < 0 or index > 2) return; v[index] = x; },
			sol::meta_function::equal_to, [](Math::vec3& v1, Math::vec3& v2) { return glm::operator==<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::subtraction, [](Math::vec3& v1, Math::vec3& v2) { return glm::operator-<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::addition, [](Math::vec3& v1, Math::vec3& v2) { return glm::operator+<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::division, [](Math::vec3& v1, Math::vec3& v2) { return glm::operator/<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::multiplication, [](Math::vec3& v1, Math::vec3& v2) { return glm::operator*<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::unary_minus, [](Math::vec3& v1) { return glm::operator-<float, glm::packed_highp>(v1);  }

		);

		//Constructors
		lua->set_function("Vec3", []() {return Math::ivec3(); });
		lua->set_function("Vec3", sol::overload([](float x, float y, float z) {return Math::vec3(x, y, z); }));

		//Static functions
		lua->set_function("Vec3_Distance", [](Math::vec3& v1, Math::vec3& v2) { return glm::distance(v1, v2); });
		lua->set_function("Vec3_Cross", [](Math::vec3& v1, Math::vec3& v2) { return glm::cross(v1, v2); });
	}

	void Bindings::BindIVec3(sol::state* lua)
	{

		lua->new_usertype<Math::ivec3>("IVec3",

			//Members
			"X", &Math::ivec3::x,
			"Y", &Math::ivec3::y,
			"Z", &Math::ivec3::z,

			//Methods
			//"Length", &Math::ivec3::length,
			//"Distance", [](Math::ivec3& v1, Math::ivec3& v2) { return glm::distance(v1, v2); },

			//Metamethods
			sol::meta_function::to_string, [](Math::ivec3& v) {return fmt::format("IVec3({}, {}, {})", v.x, v.y, v.z); },
			sol::meta_function::index, [](Math::ivec3& v, const int index) { if (index < 0 or index > 2) return 0; return v[index]; },
			sol::meta_function::new_index, [](Math::ivec3& v, const int index, double x) { if (index < 0 or index > 2) return; v[index] = x; },
			sol::meta_function::equal_to, [](Math::ivec3& v1, Math::ivec3& v2) { return glm::operator==<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::subtraction, [](Math::ivec3& v1, Math::ivec3& v2) { return glm::operator-<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::addition, [](Math::ivec3& v1, Math::ivec3& v2) { return glm::operator+<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::division, [](Math::ivec3& v1, Math::ivec3& v2) { return glm::operator/<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::multiplication, [](Math::ivec3& v1, Math::ivec3& v2) { return glm::operator*<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::unary_minus, [](Math::ivec3& v1) { return glm::operator-<int, glm::packed_highp>(v1);  }

		);

		//Constructors
		lua->set_function("IVec3", []() {return Math::ivec3(); });
		lua->set_function("IVec3", sol::overload([](int x, int y, int z) {return Math::ivec3(x, y, z); }));

		//Static functions
		//lua->set_function("IVec3_Distance", [](Math::ivec3& v1, Math::ivec3& v2) { return glm::distance(v1, v2); });
	}

	void Bindings::BindVec4(sol::state* lua)
	{
		lua->new_usertype<Math::vec4>("Vec4",

			//Members
			"X", &Math::vec4::x,
			"Y", &Math::vec4::y,
			"Z", &Math::vec4::z,
			"W", &Math::vec4::w,


			//Methods
			"Length", &Math::vec4::length,
			"Normalize", [](Math::vec4& v) { v = glm::normalize(v); return v; },
			"Distance", [](Math::vec4& v1, Math::vec4& v2) { return glm::distance(v1, v2); },

			//Metamethods
			sol::meta_function::to_string, [](Math::vec4& v) {return fmt::format("Vec4({}, {}, {}, {})", v.x, v.y, v.z, v.w); },
			sol::meta_function::index, [](Math::vec4& v, const int index) { if (index < 0 or index > 2) return 0.0f; return v[index]; },
			sol::meta_function::new_index, [](Math::vec4& v, const int index, double x) { if (index < 0 or index > 2) return; v[index] = x; },
			sol::meta_function::equal_to, [](Math::vec4& v1, Math::vec4& v2) { return glm::operator==<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::subtraction, [](Math::vec4& v1, Math::vec4& v2) { return glm::operator-<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::addition, [](Math::vec4& v1, Math::vec4& v2) { return glm::operator+<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::division, [](Math::vec4& v1, Math::vec4& v2) { return glm::operator/<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::multiplication, [](Math::vec4& v1, Math::vec4& v2) { return glm::operator*<float, glm::packed_highp>(v1, v2);  },
			sol::meta_function::unary_minus, [](Math::vec4& v1) { return glm::operator-<float, glm::packed_highp>(v1);  }

		);

		//Constructors
		lua->set_function("Vec4", []() {return Math::ivec4(); });
		lua->set_function("Vec4", sol::overload([](float x, float y, float z, float w) {return Math::vec4(x, y, z, w); }));

		//Static functions
		lua->set_function("Vec4_Distance", [](Math::vec4& v1, Math::vec4& v2) { return glm::distance(v1, v2); });
	}

	void Bindings::BindIVec4(sol::state* lua)
	{

		lua->new_usertype<Math::ivec4>("IVec4",

			//Members
			"X", &Math::ivec4::x,
			"Y", &Math::ivec4::y,
			"Z", &Math::ivec4::z,
			"W", &Math::ivec4::w,

			//Methods
			//"Length", &Math::ivec4::length,
			//"Distance", [](Math::ivec4& v1, Math::ivec4& v2) { return glm::distance(v1, v2); },

			//Metamethods
			sol::meta_function::to_string, [](Math::ivec4& v) {return fmt::format("IVec4({}, {}, {}, {})", v.x, v.y, v.z, v.w); },
			sol::meta_function::index, [](Math::ivec4& v, const int index) { if (index < 0 or index > 2) return 0; return v[index]; },
			sol::meta_function::new_index, [](Math::ivec4& v, const int index, double x) { if (index < 0 or index > 2) return; v[index] = x; },
			sol::meta_function::equal_to, [](Math::ivec4& v1, Math::ivec4& v2) { return glm::operator==<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::subtraction, [](Math::ivec4& v1, Math::ivec4& v2) { return glm::operator-<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::addition, [](Math::ivec4& v1, Math::ivec4& v2) { return glm::operator+<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::division, [](Math::ivec4& v1, Math::ivec4& v2) { return glm::operator/<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::multiplication, [](Math::ivec4& v1, Math::ivec4& v2) { return glm::operator*<int, glm::packed_highp>(v1, v2);  },
			sol::meta_function::unary_minus, [](Math::ivec4& v1) { return glm::operator-<int, glm::packed_highp>(v1);  }

		);

		//Constructors
		lua->set_function("IVec4", []() {return Math::ivec4(); });
		lua->set_function("IVec4", sol::overload([](int x, int y, int z, int w) {return Math::ivec4(x, y, z, w); }));

		//Static functions
		//lua->set_function("IVec4_Distance", [](Math::ivec4& v1, Math::ivec4& v2) { return glm::distance(v1, v2); });
	}


}