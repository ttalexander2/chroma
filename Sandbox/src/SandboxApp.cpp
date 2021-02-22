#include <Chroma.h>
#include <imgui.h>
#include <Chroma/Renderer/VertexArray.h>
#include <Chroma/Renderer/Buffer.h>
#include <Chroma/Renderer/RenderCommand.h>
#include <Chroma/Renderer/Renderer.h>



class ExampleLayer : public Chroma::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{
		m_VertexArray.reset(Chroma::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.4f, 0.1f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.0f, 0.5f, 0.2f, 1.0f
		};
		std::shared_ptr<Chroma::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Chroma::VertexBuffer::Create(vertices, sizeof(vertices)));

		//Layout no longer exists after as its scope only exists in this section. Used to test setting the layout.
		{
			Chroma::BufferLayout layout = {
				{ Chroma::ShaderDataType::Float3, "a_Position" },
				{ Chroma::ShaderDataType::Float4, "a_Color" }
			};

			vertexBuffer->SetLayout(layout);

		}

		m_VertexArray->AddVertexBuffer(vertexBuffer);


		unsigned int indices[3] = { 0, 1, 2 };

		std::shared_ptr<Chroma::IndexBuffer> indexBuffer;

		indexBuffer.reset(Chroma::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(Chroma::VertexArray::Create());

		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,
			  0.75f, -0.75f, 0.0f,
			  0.75f,  0.75f, 0.0f,
			 -0.75f,  0.75f, 0.0f,
		};


		std::shared_ptr<Chroma::VertexBuffer> squareVB;
		squareVB.reset(Chroma::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		Chroma::BufferLayout squareVBLayout = {
			{Chroma::ShaderDataType::Float3, "a_Position" }
		};

		squareVB->SetLayout(squareVBLayout);

		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

		std::shared_ptr<Chroma::IndexBuffer> squareIB;
		squareIB.reset(Chroma::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);


		std::string vertexSource = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec3 v_Position;
			out vec4 v_Color;
			
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string vertexSourceBlue = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			out vec3 v_Position;
			
			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSource = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;
			
			void main()
			{
				color = vec4(v_Position + 0.5, 1.0);
			}
		)";

		std::string fragmentSourceBlue = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			
			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		m_ShaderBlue.reset(new Chroma::Shader(vertexSourceBlue, fragmentSourceBlue));
		m_Shader.reset(new Chroma::Shader(vertexSource, fragmentSource));
	}

	void OnUpdate() override
	{
		if (Chroma::Input::IsKeyPressed(CHROMA_KEY_TAB))
		{
			CHROMA_TRACE("Tab key is pressed!");
		}

		Chroma::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Chroma::RenderCommand::Clear();

		Chroma::Renderer::BeginScene();

		m_ShaderBlue->Bind();
		Chroma::Renderer::Submit(m_SquareVA);

		m_Shader->Bind();
		Chroma::Renderer::Submit(m_VertexArray);

		Chroma::Renderer::EndScene();
	}

	void OnEvent(Chroma::Event& event) override
	{

	}

	void OnImGuiRender() override
	{

	}

private:
	std::shared_ptr<Chroma::VertexArray> m_VertexArray;
	std::shared_ptr<Chroma::VertexArray> m_SquareVA;
	std::shared_ptr<Chroma::Shader> m_Shader;
	std::shared_ptr<Chroma::Shader> m_ShaderBlue;

};

class Sandbox : public Chroma::Application
{

public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}
};

Chroma::Application* Chroma::CreateApplication()
{
	return new Sandbox();
}