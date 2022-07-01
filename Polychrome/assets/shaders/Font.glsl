#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TextCoord;
layout(location = 3) in float a_TexIndex;

layout(std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
} u_Camera;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	mat4 ViewProjection;
};

layout(location = 0) out VertexOutput Output;
layout(location = 6) out flat float v_TexIndex;

void main()
{
	Output.Color = a_Color;
	Output.TexCoord = a_TextCoord;
	Output.ViewProjection = u_Camera.ViewProjection;
	v_TexIndex = a_TexIndex;

	gl_Position = u_Camera.ViewProjection * vec4(a_Position, 1.0);
}


#type fragment
#version 450

layout(location = 0) out vec4 color;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
};

layout(location = 0) in VertexOutput Input;
layout(location = 6) in flat float v_TexIndex;

layout(binding = 0) uniform sampler2D u_Textures[32];

float msdf_median(float r, float g, float b, float a) 
{
	return min(max(min(r, g), min(max(r, g), b)), a);
}

/** 2D
float ScreenPxRange()
{
	float pixRange = 8.0f;
	float geoSize = 128.0f;
	return geoSize / 32.0f * pixRange;
}
*/


float ScreenPxRange()
{
	float pxRange = 2.0f;
    vec2 unitRange = vec2(pxRange)/vec2(textureSize(u_Textures[int(v_TexIndex)], 0));
    vec2 screenTexSize = vec2(1.0)/fwidth(Input.TexCoord);
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}


void main()
{
	vec4 bgColor = vec4(Input.Color.rgb, 0.0);
	vec4 fgColor = Input.Color;

	vec4 msdf = texture(u_Textures[int(v_TexIndex)], Input.TexCoord);
	vec2 msdf_size = vec2(textureSize(u_Textures[int(v_TexIndex)], 0));
	vec2 dest_size = vec2(1.0) / fwidth(Input.TexCoord);
	float px_size = max(0.5 * dot((vec2(2.0) / msdf_size), dest_size), 1.0);
	float d = msdf_median(msdf.r, msdf.g, msdf.b, msdf.a) - 0.5;

	float a = clamp(d * px_size + 0.5, 0.0, 1.0);
	color = mix(bgColor, fgColor, a);
}
