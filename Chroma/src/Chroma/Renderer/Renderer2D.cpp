#include "chromapch.h"
#include "Renderer2D.h"

#include "Chroma/Renderer/Pipeline.h"
#include "Chroma/Renderer/Shader.h"
#include "Chroma/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <Chroma/Profiler/Instrumentor.h>
#include <Chroma/Images/Color.h>
#include <Chroma/Scene/Scene.h>

#include <Chroma/Renderer/UniformBuffer.h>
#include <Chroma/Renderer/IndexBuffer.h>

#include <codecvt>

namespace Chroma
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;

		// Editor-only
		int EntityID = -1;
	};

	struct RenderData
	{
		const uint32_t MaxQuads = 2000;
		const uint32_t MaxVertices = MaxQuads * 9;
		const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; //TODO: Query graphics card driver for max #

		Ref<Pipeline> QuadPipeline;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<IndexBuffer> QuadIndexBuffer;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;

		QuadVertex *QuadVertexBufferBase = nullptr;
		QuadVertex *QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;

		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};

		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static RenderData s_Data;
	static Frustum s_CullingFrustum;


	void Renderer2D::Init()
	{
		CHROMA_PROFILE_FUNCTION();

		PipelineSpecification spec;
		spec.Layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TextCoord" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float, "a_TilingFactor" },
			{ ShaderDataType::Int, "a_EntityID" }
		};
		spec.Shader = Shader::Create("assets/shaders/Texture.glsl");

		s_Data.QuadPipeline = Pipeline::Create(spec);

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		auto quadIndices = new uint32_t[s_Data.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		s_Data.QuadIndexBuffer = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		delete[] quadIndices;

		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data.MaxTextureSlots];
		for (int32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f, 0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f, 0.5f, 0.0f, 1.0f };

		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(RenderData::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		delete[] s_Data.QuadVertexBufferBase;
	}

	void Renderer2D::Clear()
	{
		RenderCommand::Clear();
	}

	void Renderer2D::SetClearColor(const Color &color)
	{
		RenderCommand::SetClearColor(color);
	}

	void Renderer2D::SetClearColor(const Math::vec4 &color)
	{
		RenderCommand::SetClearColor(color);
	}

	void Renderer2D::Begin(const OrthographicCamera &camera)
	{
		Begin(camera.GetViewProjectionMatrix());
	}

	void Renderer2D::Begin(const Camera &camera)
	{
		Begin(camera.GetViewProjectionMatrix());
	}


	void Renderer2D::Begin(const Math::mat4 &camera)
	{
		CHROMA_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera;
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(RenderData::CameraData));

		StartBatch();
	}

	void Renderer2D::End()
	{
		CHROMA_PROFILE_FUNCTION();

		Flush();
	}

	void Renderer2D::StartBatch()
	{
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		CHROMA_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount == 0)
			return;

		uint32_t dataSize = static_cast<uint32_t>((uint8_t *)s_Data.QuadVertexBufferPtr - (uint8_t *)s_Data.QuadVertexBufferBase);
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			s_Data.TextureSlots[i]->Bind(i);

		s_Data.QuadPipeline->GetSpecification().Shader->Bind();
		s_Data.QuadVertexBuffer->Bind();
		s_Data.QuadPipeline->Bind();
		s_Data.QuadIndexBuffer->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadIndexCount, PrimitiveType::Triangles, true);
		s_Data.Stats.DrawCalls++;
	}

	void Renderer2D::Flush(const Ref<Shader> &shader)
	{
		CHROMA_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount == 0)
			return;

		uint32_t dataSize = static_cast<uint32_t>((uint8_t *)s_Data.QuadVertexBufferPtr - (uint8_t *)s_Data.QuadVertexBufferBase);
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			s_Data.TextureSlots[i]->Bind(i);

		shader->Bind();
		s_Data.QuadVertexBuffer->Bind();
		s_Data.QuadPipeline->Bind();
		s_Data.QuadIndexBuffer->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadIndexCount, PrimitiveType::Triangles, true);
		s_Data.Stats.DrawCalls++;

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::DrawSprite(int entityID, const Math::vec2 &position, const Math::vec2 &size, const Ref<Texture2D> &texture, const Math::vec4 &color, float rotation)
	{
		CHROMA_PROFILE_FUNCTION();

		glm::vec3 p2 = glm::vec3({ position, 0.f }) + glm::vec3({ size, 0.f });

		glm::mat4 transform;

		if (rotation == 0)
			transform = translate(glm::mat4(1.0f), { position, 0.f })
					* scale(glm::mat4(1.0f), { size, 0.f });
		else
			transform = translate(glm::mat4(1.0f), { position, 0.f })
					* rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
					* scale(glm::mat4(1.0f), { size, 0.f });

		constexpr size_t quadVertexCount = 4;

		if (s_Data.QuadIndexCount + 6 >= s_Data.MaxIndices)
			FlushAndReset();

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = static_cast<float>(i);
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->EntityID = entityID;
		s_Data.QuadVertexBufferPtr->TilingFactor = 0;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->EntityID = entityID;
		s_Data.QuadVertexBufferPtr->TilingFactor = 0;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->EntityID = entityID;
		s_Data.QuadVertexBufferPtr->TilingFactor = 0;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->EntityID = entityID;
		s_Data.QuadVertexBufferPtr->TilingFactor = 0;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::FlushAndReset()
	{
		End();

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::DrawQuad(const Math::vec2 &position, const Math::vec2 &size, const Math::vec4 &color, float rotation)
	{
		DrawQuad({ position.x, position.y, 0.0f }, { size.x, size.y, 0.0f }, color, rotation);
	}

	void Renderer2D::DrawQuad(const Math::vec3 &position, const Math::vec3 &size, const Math::vec4 &color, float rotation)
	{
		CHROMA_PROFILE_FUNCTION();

		glm::mat4 transform;

		if (rotation == 0)
			transform = translate(glm::mat4(1.0f), position)
					* scale(glm::mat4(1.0f), size);
		else
			transform = translate(glm::mat4(1.0f), position)
					* rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
					* scale(glm::mat4(1.0f), size);

		glm::vec3 p2 = position + size;

		if (s_Data.QuadIndexCount + 6 >= s_Data.MaxIndices)
			FlushAndReset();

		const float whiteTextureIndex = 0.0f;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const Math::vec2 &position, const Math::vec2 &size, const Ref<Texture2D> &texture, const Math::vec4 &color, float rotation)
	{
		DrawQuad({ position.x, position.y, 0.0f }, { size.x, size.y, 0.0f }, texture, color, rotation);
	}

	void Renderer2D::DrawQuad(const Math::vec2 &position, const Math::vec2 &size, const Ref<Texture2D> &texture, float rotation)
	{
		DrawQuad({ position.x, position.y, 0.0f }, { size.x, size.y, 0.0f }, texture, glm::vec4(1.0f), rotation);
	}

	void Renderer2D::DrawQuad(const Math::vec3 &position, const Math::vec3 &size, const Ref<Texture2D> &texture, float rotation)
	{
		DrawQuad(position, size, texture, glm::vec4(1.0f), rotation);
	}

	void Renderer2D::DrawQuad(const Math::mat4 &transform, const Math::vec4 &color)
	{
		CHROMA_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount + 6 >= s_Data.MaxIndices)
			FlushAndReset();

		const float whiteTextureIndex = 0.0f;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const Math::mat4 &transform, const Ref<Texture2D> &texture, const Math::vec4 &color)
	{
		CHROMA_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;

		if (s_Data.QuadIndexCount + 6 >= s_Data.MaxIndices)
			FlushAndReset();

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = static_cast<float>(i);
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const Math::vec3 &position, const Math::vec3 &size, const Ref<Texture2D> &texture, const Math::vec4 &color, float rotation)
	{
		CHROMA_PROFILE_FUNCTION();

		if (texture == nullptr)
		{
			CHROMA_CORE_WARN("Attempted to draw null texture!");
			return;
		}

		glm::vec3 p2 = position + size;

		glm::mat4 transform;

		if (rotation == 0)
			transform = translate(glm::mat4(1.0f), position)
					* scale(glm::mat4(1.0f), size);
		else
			transform = translate(glm::mat4(1.0f), position)
					* rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
					* scale(glm::mat4(1.0f), size);

		//if (!s_CullingFrustum.CubeIntersects(glm::min(position, p2), glm::max(position, p2)))
		//return;

		constexpr size_t quadVertexCount = 4;

		if (s_Data.QuadIndexCount + 6 >= s_Data.MaxIndices)
			FlushAndReset();

		float textureIndex = 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = static_cast<float>(i);
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawLine(const Math::vec2 &p1, const Math::vec2 &p2, float line_width, const Math::vec4 &color)
	{
		Math::vec2 midpoint = { (p1.x + p2.x) / 2.f, (p1.y + p2.y) / 2.f };
		float length = static_cast<float>(Math::sqrt(Math::pow((p2.x - p1.x), 2) + Math::pow((p2.y - p1.y), 2)));
		float rotation = Math::atan((p2.y - p1.y) / (p2.x - p1.x));
		//CHROMA_CORE_INFO("Midpoint: ({}, {}), Length: {}, Rotation: {}, LineWidth: {}", midpoint.x, midpoint.y, length, rotation, line_width);
		DrawQuad(midpoint, { length, line_width }, color, rotation);
	}

	void Renderer2D::DrawTriangle(const Math::vec2 &p1, const Math::vec2 &p2, const Math::vec2 &p3, const Math::vec4 &color)
	{
		DrawTriangle({ p1.x, p1.y, 0 }, { p2.x, p2.y, 0 }, { p3.x, p3.y, 0 }, color);
	}

	void Renderer2D::DrawTriangle(const Math::vec3 &p1, const Math::vec3 &p2, const Math::vec3 &p3, const Math::vec4 &color)
	{
		CHROMA_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount + 6 >= s_Data.MaxIndices)
			FlushAndReset();

		const float whiteTextureIndex = 0.0f;

		s_Data.QuadVertexBufferPtr->Position = p1;
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = p2;
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = p3;
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = whiteTextureIndex;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadIndexCount += 3;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRect(const Math::vec2 &position, const Math::vec2 &size, float line_width, const Math::vec4 &color, float rotation)
	{
		DrawQuad({ position.x, position.y - size.y / 2.f }, { size.x + line_width, line_width }, color, rotation); //h
		DrawQuad({ position.x + size.x / 2.f, position.y }, { line_width, size.y - line_width }, color, rotation); //v
		DrawQuad({ position.x, position.y + size.y / 2.f }, { size.x + line_width, line_width }, color, rotation); //h
		DrawQuad({ position.x - size.x / 2.f, position.y }, { line_width, size.y - line_width }, color, rotation); //v

		//DrawQuad({ position.x, position.y }, { 1,1 }, { 0.8f, 0.2f, 0.3f, 1.0f });
		//DrawQuad({ position.x, position.y - size.y / 2.f + line_width }, { 1,1 }, { 0.8f, 0.2f, 0.3f, 1.0f }); //h
		//DrawQuad({ position.x + size.x / 2.f - line_width, position.y }, { 1,1 }, { 0.8f, 0.2f, 0.3f, 1.0f }); //v
		//DrawQuad({ position.x, position.y + size.y / 2.f - line_width }, { 1,1 }, { 0.8f, 0.2f, 0.3f, 1.0f }); //h
		//DrawQuad({ position.x - size.x / 2.f + line_width, position.y }, { 1,1 }, { 0.8f, 0.2f, 0.3f, 1.0f }); //v
	}

	static std::string To_UTF8(const std::u16string &s)
	{
		// ReSharper disable once CppDeprecatedEntity
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
		return conv.to_bytes(s);
	}

	static std::string To_UTF8(const std::u32string &s)
	{
		// ReSharper disable once CppDeprecatedEntity
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
		return conv.to_bytes(s);
	}

	static std::u16string To_UTF16(const std::string &s)
	{
		// ReSharper disable once CppDeprecatedEntity
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
		return conv.from_bytes(s);
	}

	static std::u16string To_UTF16(const std::u32string &s)
	{
		// ReSharper disable once CppDeprecatedEntity
		std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> conv;
		std::string bytes = conv.to_bytes(s);
		return std::u16string(reinterpret_cast<const char16_t *>(bytes.c_str()), bytes.length() / sizeof(char16_t));
	}

	static std::u32string To_UTF32(const std::u16string &s)
	{
		const char16_t *pData = s.c_str();
		// ReSharper disable once CppDeprecatedEntity
		std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> conv;
		return conv.from_bytes(reinterpret_cast<const char *>(pData), reinterpret_cast<const char *>(pData + s.length()));
	}

	static std::u32string To_UTF32(const std::string &s)
	{
		// ReSharper disable once CppDeprecatedEntity
		std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> conv;
		return conv.from_bytes(s);
	}

	static bool NextLine(int index, const std::vector<int> &lines)
	{
		for (int line : lines)
		{
			if (line == index)
				return true;
		}
		return false;
	}

	void Renderer2D::DrawString(const std::string &string, const Ref<Font> &font, const Math::vec3 &position, float maxWidth, const glm::vec4 &color, float scale, float lineHeightOffset = 0.f, float kerningOffset = 0.f)
	{
		DrawString(To_UTF32(string), font, position, maxWidth, color, scale, lineHeightOffset, kerningOffset);
	}

	void Renderer2D::DrawString(const std::u16string &string, const Ref<Font> &font, const Math::vec3 &position, float maxWidth, const glm::vec4 &color, float scale, float lineHeightOffset = 0.f, float kerningOffset = 0.f)
	{
		DrawString(To_UTF32(string), font, position, maxWidth, color, scale, lineHeightOffset, kerningOffset);
	}

	void Renderer2D::DrawString(const std::u32string &string, const Ref<Font> &font, const Math::vec3 &position, float maxWidth, const glm::vec4 &color, float scale, float lineHeightOffset = 0.f, float kerningOffset = 0.f)
	{
		if (string.empty())
			return;

		float textureIndex = 0.0f;

		glm::mat4 transform = translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 0));

		Ref<Texture2D> fontAtlas = font->m_Atlas.texture;

		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *fontAtlas.get())
			{
				textureIndex = static_cast<float>(i);
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = fontAtlas;
			s_Data.TextureSlotIndex++;
		}

		std::vector<int> nextLines;
		{
			double x = 0.0;
			double fsScale = 1 / (font->m_Atlas.ascender_y - font->m_Atlas.decender_y);
			double y = -fsScale * font->m_Atlas.ascender_y;
			int lastSpace = -1;
			for (int i = 0; i < string.size(); i++)
			{
				char32_t character = string[i];
				if (character == '\n')
				{
					x = 0;
					y -= fsScale * font->m_Atlas.line_height + lineHeightOffset;
					continue;
				}

				auto &glyph = font->m_Atlas.glyphs[character];
				if (!font->m_Atlas.glyphs.contains(character))
					glyph = font->m_Atlas.glyphs['?'];
				if (!font->m_Atlas.glyphs.contains('?'))
					continue;

				if (character != ' ')
				{
					glm::vec2 quadMin(static_cast<float>(glyph.pl), static_cast<float>(glyph.pb));
					glm::vec2 quadMax(static_cast<float>(glyph.pr), static_cast<float>(glyph.pt));

					quadMin *= fsScale;
					quadMax *= fsScale;
					quadMin += glm::vec2(x, y);
					quadMax += glm::vec2(x, y);

					if (quadMax.x > maxWidth && lastSpace != -1)
					{
						i = lastSpace;
						nextLines.emplace_back(lastSpace);
						lastSpace = -1;
						x = 0;
						y -= fsScale * font->m_Atlas.line_height + lineHeightOffset;
					}
				}
				else
				{
					lastSpace = i;
				}

				double advance = font->m_Atlas.kerning[std::pair(character, string[i + 1])];
				x += fsScale * advance + kerningOffset;
			}
		}

		{
			double x = 0.0;
			double fsScale = 1 / (font->m_Atlas.ascender_y - font->m_Atlas.decender_y);
			double y = 0.0; // -fsScale * metrics.ascenderY;
			for (int i = 0; i < string.size(); i++)
			{
				char32_t character = string[i];
				if (character == '\n' || NextLine(i, nextLines))
				{
					x = 0;
					y -= fsScale * font->m_Atlas.line_height + lineHeightOffset;
					continue;
				}

				auto &glyph = font->m_Atlas.glyphs[character];
				if (!font->m_Atlas.glyphs.contains(character))
					glyph = font->m_Atlas.glyphs['?'];
				if (!font->m_Atlas.glyphs.contains('?'))
					continue;

				double l, b, r, t, pl, pb, pr, pt;
				l = glyph.l;
				b = glyph.b;
				r = glyph.r;
				t = glyph.t;
				pl = glyph.pl;
				pb = glyph.pb;
				pr = glyph.pr;
				pt = glyph.pt;

				pl *= fsScale, pb *= fsScale, pr *= fsScale, pt *= fsScale;
				pl += x, pb += y, pr += x, pt += y;

				double texelWidth = 1. / fontAtlas->GetWidth();
				double texelHeight = 1. / fontAtlas->GetHeight();
				l *= texelWidth, b *= texelHeight, r *= texelWidth, t *= texelHeight;

				s_Data.QuadVertexBufferPtr->Position = transform * glm::vec4(pl, pb, 0.0f, 1.0f);
				s_Data.QuadVertexBufferPtr->Color = color;
				s_Data.QuadVertexBufferPtr->TexCoord = { l, b };
				s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
				s_Data.QuadVertexBufferPtr++;

				s_Data.QuadVertexBufferPtr->Position = transform * glm::vec4(pl, pt, 0.0f, 1.0f);
				s_Data.QuadVertexBufferPtr->Color = color;
				s_Data.QuadVertexBufferPtr->TexCoord = { l, t };
				s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
				s_Data.QuadVertexBufferPtr++;

				s_Data.QuadVertexBufferPtr->Position = transform * glm::vec4(pr, pt, 0.0f, 1.0f);
				s_Data.QuadVertexBufferPtr->Color = color;
				s_Data.QuadVertexBufferPtr->TexCoord = { r, t };
				s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
				s_Data.QuadVertexBufferPtr++;

				s_Data.QuadVertexBufferPtr->Position = transform * glm::vec4(pr, pb, 0.0f, 1.0f);
				s_Data.QuadVertexBufferPtr->Color = color;
				s_Data.QuadVertexBufferPtr->TexCoord = { r, b };
				s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
				s_Data.QuadVertexBufferPtr++;

				s_Data.QuadIndexCount += 6;

				double advance = font->m_Atlas.kerning[std::pair(character, string[i + 1])];
				x += fsScale * advance + kerningOffset;
			}
		}
	}


	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}
}
