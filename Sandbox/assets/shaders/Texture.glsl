#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TextCoord;
layout(location = 3) in float a_TexIndex;

uniform mat4 u_ViewProjection;

out vec4 v_Color;
out vec2 v_TextCoord;
out float v_TexIndex;
			
void main()
{
	v_Color = a_Color;
	v_TextCoord = a_TextCoord;
	v_TexIndex = a_TexIndex;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TextCoord;
in float v_TexIndex;

uniform sampler2D u_Textures[32];
			
void main()
{

	color = texture(u_Textures[int(v_TexIndex)], v_TextCoord) * v_Color;
	//color = vec4(v_TextCoord, 0.0, 1.0);
}
