#include <Chroma.h>
#include <imgui.h>
#include <Chroma/Renderer/VertexArray.h>
#include <Chroma/Renderer/Buffer.h>
#include <Chroma/Renderer/RenderCommand.h>
#include <Chroma/Renderer/Renderer.h>
#include <Chroma/Renderer/OrthographicCamera.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Chroma/Core.h>


class ExampleLayer : public Chroma::Layer
{
public:
	ExampleLayer() 
		: Layer("Example") ,m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{


		m_VertexArray.reset(Chroma::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.4f, 0.1f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.0f, 0.5f, 0.2f, 1.0f
		};
		Chroma::Ref<Chroma::VertexBuffer> vertexBuffer;
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

		Chroma::Ref<Chroma::IndexBuffer> indexBuffer;

		indexBuffer.reset(Chroma::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(Chroma::VertexArray::Create());

		float squareVertices[5 * 4] = {
			 -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			 -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};


		Chroma::Ref<Chroma::VertexBuffer> squareVB;
		squareVB.reset(Chroma::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		Chroma::BufferLayout squareVBLayout = {
			{ Chroma::ShaderDataType::Float3, "a_Position" },
			{ Chroma::ShaderDataType::Float2, "a_TexCoord" }
		};

		squareVB->SetLayout(squareVBLayout);

		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

		Chroma::Ref<Chroma::IndexBuffer> squareIB;
		squareIB.reset(Chroma::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);


		std::string vertexSource = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;
			
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string vertexFlatColorSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
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

		std::string fragmentFlatColorSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec4 u_Color;
			
			void main()
			{
				color = u_Color + vec4(v_Position/2 + 0.3, 1.0);
			}
		)";

		std::string vertexTextureSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TextCoord;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec2 v_TextCoord;
			
			void main()
			{
				v_TextCoord = a_TextCoord;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentTextureSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec2 v_TextCoord;

			uniform sampler2D u_Texture;
			
			void main()
			{
				color = texture(u_Texture, v_TextCoord);
			}
		)";

		m_FlatShader.reset(Chroma::Shader::Create(vertexFlatColorSrc, fragmentFlatColorSrc));
		m_Shader.reset(Chroma::Shader::Create(vertexSource, fragmentSource));
		m_TextureShader.reset(Chroma::Shader::Create(vertexTextureSrc, fragmentTextureSrc));

		m_Texture = Chroma::Texture2D::Create("assets/textures/grid.png");
		m_TextureShader->UploadUniformInt("u_Texture", 0);
		m_TextureShader->Bind();



		m_SquarePosition = glm::vec3(0.0f);
	}

	void OnUpdate(Chroma::Timestep time) override
	{

		glm::vec3 position = m_Camera.GetPosition();

		if (Chroma::Input::IsKeyPressed(CHROMA_KEY_UP))
		{
			position.y += 5.0f * time;
		}

		if (Chroma::Input::IsKeyPressed(CHROMA_KEY_DOWN))
		{
			position.y -= 5.0f * time;
		}

		if (Chroma::Input::IsKeyPressed(CHROMA_KEY_LEFT))
		{
			position.x -= 5.0f * time;
		}

		if (Chroma::Input::IsKeyPressed(CHROMA_KEY_RIGHT))
		{
			position.x += 5.0f * time;
		}

		if (Chroma::Input::IsKeyPressed(CHROMA_KEY_W))
		{
			m_SquarePosition.y += 5.0f * time;
		}

		if (Chroma::Input::IsKeyPressed(CHROMA_KEY_S))
		{
			m_SquarePosition.y -= 5.0f * time;
		}

		if (Chroma::Input::IsKeyPressed(CHROMA_KEY_A))
		{
			m_SquarePosition.x -= 5.0f * time;
		}

		if (Chroma::Input::IsKeyPressed(CHROMA_KEY_D))
		{
			m_SquarePosition.x += 5.0f * time;
		}

		m_Camera.SetPosition(position);

		float rotation = m_Camera.GetRotation();

		if (Chroma::Input::IsKeyPressed(CHROMA_KEY_KP_ADD))
		{
			rotation += 20.0f * time;
		}

		if (Chroma::Input::IsKeyPressed(CHROMA_KEY_KP_SUBTRACT))
		{
			rotation -= 20.0f * time;
		}

		m_Camera.SetRotation(rotation);


		Chroma::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Chroma::RenderCommand::Clear();

		Chroma::Renderer::BeginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.3f));

		for (int i = 0; i < 5; i++)
		{

			m_FlatShader->UploadUniformFloat4("u_Color", m_SquareColor);


			glm::vec3 pos(i * 0.33f, 0.0f, 0.0f);
			pos += m_SquarePosition;
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
			Chroma::Renderer::Submit(m_FlatShader, m_SquareVA, transform);
		}

		m_Texture->Bind();
		Chroma::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		
		// Triangle
		// Chroma::Renderer::Submit(m_Shader, m_VertexArray);

		Chroma::Renderer::EndScene();
	}

	void OnEvent(Chroma::Event& event) override
	{

	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorPicker4("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();

	}

private:
	Chroma::Ref<Chroma::VertexArray> m_VertexArray;
	Chroma::Ref<Chroma::VertexArray> m_SquareVA;
	Chroma::Ref<Chroma::Shader> m_Shader;
	Chroma::Ref<Chroma::Shader> m_FlatShader;
	Chroma::Ref<Chroma::Shader> m_TextureShader;

	Chroma::Ref<Chroma::Texture2D> m_Texture;

	Chroma::OrthographicCamera m_Camera;

	glm::vec3 m_SquarePosition;
	glm::vec4 m_SquareColor;

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