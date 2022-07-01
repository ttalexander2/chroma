#pragma once

#include "Chroma/Scene/Component.h"

#include <box2d.h>

namespace Chroma
{
	
	struct Collider : public Component
	{
		CHROMA_COMPONENT(Collider, Component);
		friend class PhysicsSystem;

		uint32_t Mask = 0;
		uint32_t Layer = 0;

		enum class ShapeType
		{
			Circle,
			Polygon,
			Edge,
			Chain,
			Rectangle
		};

		struct Shape 
		{
			inline virtual const ShapeType GetType() const = 0;

			virtual Math::vec2 GetPosition() const = 0;
			virtual void SetPosition(const Math::vec2 &value) = 0;
		};

		struct Circle : public Shape
		{
			inline const ShapeType GetType() const { return ShapeType::Circle; }

			float GetRadius() const;
			void SetRadius(float value);

			Math::vec2 GetPosition() const override;
			void SetPosition(const Math::vec2 &value) override;




		private:
			b2CircleShape m_Shape;
		};

		struct Polygon : public Shape
		{
			inline const ShapeType GetType() const { return ShapeType::Polygon; }

			const size_t MaxVertices = b2_maxPolygonVertices;

			/// @brief Validates convexity. This is a very time consuming operation.
			/// @return Returns true if valid
			bool ValidateConvexity();

			/// @brief Checks to make sure the polygon has at least 3 vertices.
			/// @return Returns true if valid.
			bool IsValidPolygon();

			Math::vec2 GetPosition() const override;
			void SetPosition(const Math::vec2 &value) override;

			bool PushVertex(const Math::vec2 &vertex, const Math::vec2& normal);
			bool PopVertex();

			bool SetVertex(int32_t index, const Math::vec2 &vertex, const Math::vec2 &normal);
			bool RemoveVertex(int32_t index);

			struct Vertex
			{
				Math::vec2 vertex;
				Math::vec2 normal;
			};

			std::vector<Vertex> GetVertices();
			int32_t Count();
			bool GetVertex(Vertex* out, int32_t index);


		private:
			b2PolygonShape m_Shape;
		};


		struct Edge : public Shape
		{
			inline const ShapeType GetType() const { return ShapeType::Edge; }

			Math::vec2 GetPosition() const override;
			void SetPosition(const Math::vec2 &value) override;

			void SetOneSided(bool val);
			bool IsOneSided() const;

			void SetV0(const Math::vec2 &val);
			void SetV1(const Math::vec2 &val);
			void SetV2(const Math::vec2 &val);
			void SetV3(const Math::vec2 &val);

			Math::vec2 GetV0() const;
			Math::vec2 GetV1() const;
			Math::vec2 GetV2() const;
			Math::vec2 GetV3() const;

		private:
			b2EdgeShape m_Shape;
		};


		struct Chain : public Shape
		{
			inline const ShapeType GetType() const { return ShapeType::Chain; }

			Math::vec2 GetPosition() const override;
			void SetPosition(const Math::vec2 &value) override;

		private:
			b2ChainShape m_Shape;
		};

		struct Rectangle : public Shape
		{
			inline const ShapeType GetType() const { return ShapeType::Rectangle; }

			Math::vec2 GetPosition() const override;
			void SetPosition(const Math::vec2 &value) override;

		private:
			b2PolygonShape m_Shape;
		};



		void Initialize() override;

		void SetFriction(float value);
		float GetFriction() const;

		void SetRestitution(float value);
		float GetRestitution() const;

		void SetRestitutionThreshold(float value);
		float GetRestitutionThreshold() const;

		void SetDensity(float density);
		float GetDensity() const;

		void SetSensor(bool sensor);
		bool IsSensor() const;

		inline const ShapeType GetShapeType() const { return m_Shape->GetType(); }
		void SetShapeType(ShapeType type);

		template <typename T>
		T &GetShape()
		{
			static_assert(std::is_base_of<Collider::Shape, T>::value, "Type T is not derived from Collider::Shape");
			if (!m_Shape)
				m_Shape = new Rectangle();
			return *reinterpret_cast<T*>(m_Shape);
		}
		

	private:
		b2FixtureDef m_FixtureDef;
		b2Fixture *m_Fixture;
		Shape *m_Shape;
	};


}