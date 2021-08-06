#pragma once

#include "chromapch.h"
#include "Chroma/Core/Core.h"

namespace Chroma
{
	/// @brief Shader uniform data types
	enum class ShaderDataType
	{
		None = 0, 
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		Mat3, Mat4,
		Bool
	};

	/// @brief Gets the size of a shader data type.
	/// @param type Type of the data
	/// @return size of the data
	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:		return 4;
			case ShaderDataType::Float2:    return 4 * 2;
			case ShaderDataType::Float3:    return 4 * 3;
			case ShaderDataType::Float4:    return 4 * 4;
			case ShaderDataType::Mat3:		return 4 * 3 * 3;
			case ShaderDataType::Mat4:		return 4 * 4 * 4;
			case ShaderDataType::Int:		return 4;
			case ShaderDataType::Int2:		return 4 * 2;
			case ShaderDataType::Int3:		return 4 * 3;
			case ShaderDataType::Int4:		return 4 * 4;
			case ShaderDataType::Bool:		return 1;
		}

		CHROMA_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	/// @brief Buffer element.
	struct BufferElement
	{
		/// @brief Name of the element
		std::string Name;
		/// @brief Type of the data.
		ShaderDataType Type = ShaderDataType::None;
		/// @brief Size of the buffer element.
		uint32_t Size = 0;
		/// @brief Offset of the element.
		uint32_t Offset = 0;
		/// @brief Whether the data should be normalized.
		bool Normalized = false;

		/// @brief Constructs an empty BufferElement.
		BufferElement() {}

		/// @brief Constructs a BufferElement.
		/// @param type Type of the uniform data.
		/// @param name Name of the buffer element.
		/// @param normalized Whether the data should be normalized.
		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{
		}

		/// @brief Gets number of components in the buffer element.
		/// @return Number of components.
		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case ShaderDataType::Float:		return 1;
				case ShaderDataType::Float2:    return 2;
				case ShaderDataType::Float3:    return 3;
				case ShaderDataType::Float4:    return 4;
				case ShaderDataType::Mat3:		return 3 * 3;
				case ShaderDataType::Mat4:		return 4 * 4;
				case ShaderDataType::Int:		return 1;
				case ShaderDataType::Int2:		return 2;
				case ShaderDataType::Int3:		return 3;
				case ShaderDataType::Int4:		return 4;
				case ShaderDataType::Bool:		return 1;
			}
		}

	};

	/// @brief Iterable layout of a buffer.
	class CHROMA_API BufferLayout
	{
	public:
		/// @brief Constructs an empty BufferLayout.
		BufferLayout() {}
		/// @brief Constructs a new BufferLayout.
		/// @param elements List of BufferElements to populate the layout.
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetAndStride();
		}

		/// @brief Gets the BufferElements from the layout.
		/// @return List of BufferElement.
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		/// @brief Gets the Stride.
		inline uint32_t GetStride() const { return m_Stride; }


		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};


	/// @brief 
	class CHROMA_API VertexBuffer 
	{
	public:
		virtual ~VertexBuffer() {};
		
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		static Ref<VertexBuffer> Create(uint32_t size);
		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
	};

	// Assumes 32-bit index buffer
	class CHROMA_API IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {};

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);

	};
}