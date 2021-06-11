#include "chromapch.h"
#include "Renderer2D.h"

#include "Chroma/Renderer/VertexArray.h"
#include "Chroma/Renderer/Shader.h"
#include "Chroma/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <Chroma/Profiler/Instrumentor.h>

namespace Chroma
{
	struct QuadVertex
	{
		glm::vec3	Position;
		glm::vec4	Color;
		glm::vec2	TexCoord;
		float		TexIndex;
	};

	struct RenderData
	{
		const uint32_t MaxQuads = 256;
		const uint32_t MaxVertices = MaxQuads * 9;
		const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; //TODO: Query graphics card driver for max #

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;

		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;

		glm::vec4 QuadVertexPositions[4];
		
		Renderer2D::Statistics Stats;
	};

	static RenderData s_Data;

	//static const glm::vec2 s_TextureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

	static Frustum s_CullingFrustum;

	void Renderer2D::Init()
	{
		CHROMA_PROFILE_FUNCTION();
		s_Data.QuadVertexArray = VertexArray::Create();

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ Chroma::ShaderDataType::Float3, "a_Position" },
			{ Chroma::ShaderDataType::Float4, "a_Color" },
			{ Chroma::ShaderDataType::Float2, "a_TextCoord" },
			{ Chroma::ShaderDataType::Float, "a_TexIndex" }
		});

		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];


		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

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

		Ref<IndexBuffer> quadIB;
		quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);

		delete[] quadIndices;


		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data.MaxTextureSlots];
		for (int32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetUniformIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

	}
	void Renderer2D::Shutdown()
	{	
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		CHROMA_PROFILE_FUNCTION();
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		s_Data.TextureShader->SetUniformMat4("u_Transform", glm::mat4(1.0f));

		s_Data.QuadIndexCount = 0;
		s_Data.TextureSlotIndex = 1;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_CullingFrustum = Frustum(camera.GetViewProjectionMatrix());

	}

	void Renderer2D::EndScene()
	{
		CHROMA_PROFILE_FUNCTION();
		uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		Flush();
	}

	void Renderer2D::Flush()
	{
		CHROMA_PROFILE_FUNCTION();
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			s_Data.TextureSlots[i]->Bind(i);

		if (s_Data.QuadIndexCount > 0)
		{
			RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
			s_Data.Stats.DrawCalls++;
		}

	}

	void Renderer2D::FlushAndReset()
	{
		EndScene();
		
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::DrawQuad(const Math::vec2& position, const Math::vec2& size, const Math::vec4& color, float rotation)
	{
		DrawQuad({ position.x, position.y, 0.0f }, { size.x, size.y, 0.0f }, color, rotation);
	}

	void Renderer2D::DrawQuad(const Math::vec3& position, const Math::vec3& size, const Math::vec4& color, float rotation)
	{
		CHROMA_PROFILE_FUNCTION();

		glm::mat4 transform;

		if (rotation == 0)
			transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), size);
		else
			transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), size);

		glm::vec3 p2 = position + size;


		//CHROMA_CORE_WARN("POS: ({0}, {1}, {2}), SIZE: ({3}, {4}, {5})", position.x, position.y, position.z, size.x, size.y, size.z);
		//CHROMA_CORE_WARN("MIN: ({0}, {1}, {2}), MAX: ({3}, {4}, {5})", glm::min(position, p2).x, glm::min(position, p2).y, glm::min(position, p2).z, glm::max(position, p2).x, glm::max(position, p2).y, glm::max(position, p2).z);

		//if (!s_CullingFrustum.CubeIntersects(glm::min(position, p2), glm::max(position, p2)))
			//return;

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

	void Renderer2D::DrawQuad(const Math::vec2& position, const Math::vec2& size, const Ref<Texture2D>& texture, const Math::vec4& color, float rotation)
	{
		DrawQuad({ position.x, position.y, 0.0f }, { size.x, size.y, 0.0f }, texture, color, rotation);
	}

	void Renderer2D::DrawQuad(const Math::vec2& position, const Math::vec2& size, const Ref<Texture2D>& texture, float rotation)
	{
		DrawQuad({ position.x, position.y, 0.0f }, { size.x, size.y, 0.0f }, texture, glm::vec4(1.0f), rotation);
	}

	void Renderer2D::DrawQuad(const Math::vec3& position, const Math::vec3& size, const Ref<Texture2D>& texture, float rotation)
	{
		DrawQuad(position, size, texture, glm::vec4(1.0f), rotation);
	}

	void Renderer2D::DrawQuad(const Math::mat4& transform, const Math::vec4& color)
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

	void Renderer2D::DrawQuad(const Math::mat4& transform, const Ref<Texture2D>& texture, const Math::vec4& color)
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
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
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

	void Renderer2D::DrawQuad(const Math::vec3& position, const Math::vec3& size, const Ref<Texture2D>& texture, const Math::vec4& color, float rotation)
	{
		CHROMA_PROFILE_FUNCTION();

		glm::vec3 p2 = position + size;

		glm::mat4 transform;

		if (rotation == 0)
			transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), size);
		else
			transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), size);

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
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
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


	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}
}
