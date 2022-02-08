#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TextCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in int a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 ViewProjection;
} u_Camera;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
};

layout(location = 0) out VertexOutput Output;
layout(location = 3) out flat float v_TexIndex;
layout(location = 4) out flat int v_EntityID;

void main()
{
	Output.Color = a_Color;
	Output.TexCoord = a_TextCoord;
	v_TexIndex = a_TexIndex;
	v_EntityID = a_EntityID;

	gl_Position = u_Camera.ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450

layout(location = 0) out vec4 color;
layout(location = 1) out int entityID;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
};

layout(location = 0) in VertexOutput Input;
layout(location = 3) in flat float v_TexIndex;
layout(location = 4) in flat int v_EntityID;

layout(binding = 0) uniform sampler2D u_Textures[32];

vec4 getTextureColor(vec2 textCoord)
{
	vec4 texColor = Input.Color;
	switch (int(v_TexIndex))
	{
	case  0: texColor *= texture(u_Textures[0], textCoord); break;
	case  1: texColor *= texture(u_Textures[1], textCoord); break;
	case  2: texColor *= texture(u_Textures[2], textCoord); break;
	case  3: texColor *= texture(u_Textures[3], textCoord); break;
	case  4: texColor *= texture(u_Textures[4], textCoord); break;
	case  5: texColor *= texture(u_Textures[5], textCoord); break;
	case  6: texColor *= texture(u_Textures[6], textCoord); break;
	case  7: texColor *= texture(u_Textures[7], textCoord); break;
	case  8: texColor *= texture(u_Textures[8], textCoord); break;
	case  9: texColor *= texture(u_Textures[9], textCoord); break;
	case 10: texColor *= texture(u_Textures[10], textCoord); break;
	case 11: texColor *= texture(u_Textures[11], textCoord); break;
	case 12: texColor *= texture(u_Textures[12], textCoord); break;
	case 13: texColor *= texture(u_Textures[13], textCoord); break;
	case 14: texColor *= texture(u_Textures[14], textCoord); break;
	case 15: texColor *= texture(u_Textures[15], textCoord); break;
	case 16: texColor *= texture(u_Textures[16], textCoord); break;
	case 17: texColor *= texture(u_Textures[17], textCoord); break;
	case 18: texColor *= texture(u_Textures[18], textCoord); break;
	case 19: texColor *= texture(u_Textures[19], textCoord); break;
	case 20: texColor *= texture(u_Textures[20], textCoord); break;
	case 21: texColor *= texture(u_Textures[21], textCoord); break;
	case 22: texColor *= texture(u_Textures[22], textCoord); break;
	case 23: texColor *= texture(u_Textures[23], textCoord); break;
	case 24: texColor *= texture(u_Textures[24], textCoord); break;
	case 25: texColor *= texture(u_Textures[25], textCoord); break;
	case 26: texColor *= texture(u_Textures[26], textCoord); break;
	case 27: texColor *= texture(u_Textures[27], textCoord); break;
	case 28: texColor *= texture(u_Textures[28], textCoord); break;
	case 29: texColor *= texture(u_Textures[29], textCoord); break;
	case 30: texColor *= texture(u_Textures[30], textCoord); break;
	case 31: texColor *= texture(u_Textures[31], textCoord); break;
	}
	return texColor;
}

ivec2 getTextureSize()
{
	ivec2 size;
	switch (int(v_TexIndex))
	{
	case  0: size = textureSize(u_Textures[0], 0); break;
	case  1: size = textureSize(u_Textures[1], 0); break;
	case  2: size = textureSize(u_Textures[2], 0); break;
	case  3: size = textureSize(u_Textures[3], 0); break;
	case  4: size = textureSize(u_Textures[4], 0); break;
	case  5: size = textureSize(u_Textures[5], 0); break;
	case  6: size = textureSize(u_Textures[6], 0); break;
	case  7: size = textureSize(u_Textures[7], 0); break;
	case  8: size = textureSize(u_Textures[8], 0); break;
	case  9: size = textureSize(u_Textures[9], 0); break;
	case 10: size = textureSize(u_Textures[10], 0); break;
	case 11: size = textureSize(u_Textures[11], 0); break;
	case 12: size = textureSize(u_Textures[12], 0); break;
	case 13: size = textureSize(u_Textures[13], 0); break;
	case 14: size = textureSize(u_Textures[14], 0); break;
	case 15: size = textureSize(u_Textures[15], 0); break;
	case 16: size = textureSize(u_Textures[16], 0); break;
	case 17: size = textureSize(u_Textures[17], 0); break;
	case 18: size = textureSize(u_Textures[18], 0); break;
	case 19: size = textureSize(u_Textures[19], 0); break;
	case 20: size = textureSize(u_Textures[20], 0); break;
	case 21: size = textureSize(u_Textures[21], 0); break;
	case 22: size = textureSize(u_Textures[22], 0); break;
	case 23: size = textureSize(u_Textures[23], 0); break;
	case 24: size = textureSize(u_Textures[24], 0); break;
	case 25: size = textureSize(u_Textures[25], 0); break;
	case 26: size = textureSize(u_Textures[26], 0); break;
	case 27: size = textureSize(u_Textures[27], 0); break;
	case 28: size = textureSize(u_Textures[28], 0); break;
	case 29: size = textureSize(u_Textures[29], 0); break;
	case 30: size = textureSize(u_Textures[30], 0); break;
	case 31: size = textureSize(u_Textures[31], 0); break;
	}
	return size;
}

vec4 getTextureColorOffset(vec2 textCoord, int index)
{
	vec4 texColor = Input.Color;
	if (index == 0)
	{
		switch (int(v_TexIndex))
		{
		case  0: texColor *= textureOffset(u_Textures[0], textCoord, ivec2(-1, 0)); break;
		case  1: texColor *= textureOffset(u_Textures[1], textCoord, ivec2(-1, 0)); break;
		case  2: texColor *= textureOffset(u_Textures[2], textCoord, ivec2(-1, 0)); break;
		case  3: texColor *= textureOffset(u_Textures[3], textCoord, ivec2(-1, 0)); break;
		case  4: texColor *= textureOffset(u_Textures[4], textCoord, ivec2(-1, 0)); break;
		case  5: texColor *= textureOffset(u_Textures[5], textCoord, ivec2(-1, 0)); break;
		case  6: texColor *= textureOffset(u_Textures[6], textCoord, ivec2(-1, 0)); break;
		case  7: texColor *= textureOffset(u_Textures[7], textCoord, ivec2(-1, 0)); break;
		case  8: texColor *= textureOffset(u_Textures[8], textCoord, ivec2(-1, 0)); break;
		case  9: texColor *= textureOffset(u_Textures[9], textCoord, ivec2(-1, 0)); break;
		case 10: texColor *= textureOffset(u_Textures[10], textCoord, ivec2(-1, 0)); break;
		case 11: texColor *= textureOffset(u_Textures[11], textCoord, ivec2(-1, 0)); break;
		case 12: texColor *= textureOffset(u_Textures[12], textCoord, ivec2(-1, 0)); break;
		case 13: texColor *= textureOffset(u_Textures[13], textCoord, ivec2(-1, 0)); break;
		case 14: texColor *= textureOffset(u_Textures[14], textCoord, ivec2(-1, 0)); break;
		case 15: texColor *= textureOffset(u_Textures[15], textCoord, ivec2(-1, 0)); break;
		case 16: texColor *= textureOffset(u_Textures[16], textCoord, ivec2(-1, 0)); break;
		case 17: texColor *= textureOffset(u_Textures[17], textCoord, ivec2(-1, 0)); break;
		case 18: texColor *= textureOffset(u_Textures[18], textCoord, ivec2(-1, 0)); break;
		case 19: texColor *= textureOffset(u_Textures[19], textCoord, ivec2(-1, 0)); break;
		case 20: texColor *= textureOffset(u_Textures[20], textCoord, ivec2(-1, 0)); break;
		case 21: texColor *= textureOffset(u_Textures[21], textCoord, ivec2(-1, 0)); break;
		case 22: texColor *= textureOffset(u_Textures[22], textCoord, ivec2(-1, 0)); break;
		case 23: texColor *= textureOffset(u_Textures[23], textCoord, ivec2(-1, 0)); break;
		case 24: texColor *= textureOffset(u_Textures[24], textCoord, ivec2(-1, 0)); break;
		case 25: texColor *= textureOffset(u_Textures[25], textCoord, ivec2(-1, 0)); break;
		case 26: texColor *= textureOffset(u_Textures[26], textCoord, ivec2(-1, 0)); break;
		case 27: texColor *= textureOffset(u_Textures[27], textCoord, ivec2(-1, 0)); break;
		case 28: texColor *= textureOffset(u_Textures[28], textCoord, ivec2(-1, 0)); break;
		case 29: texColor *= textureOffset(u_Textures[29], textCoord, ivec2(-1, 0)); break;
		case 30: texColor *= textureOffset(u_Textures[30], textCoord, ivec2(-1, 0)); break;
		case 31: texColor *= textureOffset(u_Textures[31], textCoord, ivec2(-1, 0)); break;
		}
	}
	if (index == 1)
	{
		switch (int(v_TexIndex))
		{
		case  0: texColor *= textureOffset(u_Textures[0], textCoord, ivec2(1, 0)); break;
		case  1: texColor *= textureOffset(u_Textures[1], textCoord, ivec2(1, 0)); break;
		case  2: texColor *= textureOffset(u_Textures[2], textCoord, ivec2(1, 0)); break;
		case  3: texColor *= textureOffset(u_Textures[3], textCoord, ivec2(1, 0)); break;
		case  4: texColor *= textureOffset(u_Textures[4], textCoord, ivec2(1, 0)); break;
		case  5: texColor *= textureOffset(u_Textures[5], textCoord, ivec2(1, 0)); break;
		case  6: texColor *= textureOffset(u_Textures[6], textCoord, ivec2(1, 0)); break;
		case  7: texColor *= textureOffset(u_Textures[7], textCoord, ivec2(1, 0)); break;
		case  8: texColor *= textureOffset(u_Textures[8], textCoord, ivec2(1, 0)); break;
		case  9: texColor *= textureOffset(u_Textures[9], textCoord, ivec2(1, 0)); break;
		case 10: texColor *= textureOffset(u_Textures[10], textCoord, ivec2(1, 0)); break;
		case 11: texColor *= textureOffset(u_Textures[11], textCoord, ivec2(1, 0)); break;
		case 12: texColor *= textureOffset(u_Textures[12], textCoord, ivec2(1, 0)); break;
		case 13: texColor *= textureOffset(u_Textures[13], textCoord, ivec2(1, 0)); break;
		case 14: texColor *= textureOffset(u_Textures[14], textCoord, ivec2(1, 0)); break;
		case 15: texColor *= textureOffset(u_Textures[15], textCoord, ivec2(1, 0)); break;
		case 16: texColor *= textureOffset(u_Textures[16], textCoord, ivec2(1, 0)); break;
		case 17: texColor *= textureOffset(u_Textures[17], textCoord, ivec2(1, 0)); break;
		case 18: texColor *= textureOffset(u_Textures[18], textCoord, ivec2(1, 0)); break;
		case 19: texColor *= textureOffset(u_Textures[19], textCoord, ivec2(1, 0)); break;
		case 20: texColor *= textureOffset(u_Textures[20], textCoord, ivec2(1, 0)); break;
		case 21: texColor *= textureOffset(u_Textures[21], textCoord, ivec2(1, 0)); break;
		case 22: texColor *= textureOffset(u_Textures[22], textCoord, ivec2(1, 0)); break;
		case 23: texColor *= textureOffset(u_Textures[23], textCoord, ivec2(1, 0)); break;
		case 24: texColor *= textureOffset(u_Textures[24], textCoord, ivec2(1, 0)); break;
		case 25: texColor *= textureOffset(u_Textures[25], textCoord, ivec2(1, 0)); break;
		case 26: texColor *= textureOffset(u_Textures[26], textCoord, ivec2(1, 0)); break;
		case 27: texColor *= textureOffset(u_Textures[27], textCoord, ivec2(1, 0)); break;
		case 28: texColor *= textureOffset(u_Textures[28], textCoord, ivec2(1, 0)); break;
		case 29: texColor *= textureOffset(u_Textures[29], textCoord, ivec2(1, 0)); break;
		case 30: texColor *= textureOffset(u_Textures[30], textCoord, ivec2(1, 0)); break;
		case 31: texColor *= textureOffset(u_Textures[31], textCoord, ivec2(1, 0)); break;
		}
	}
	if (index == 2)
	{
		switch (int(v_TexIndex))
		{
		case  0: texColor *= textureOffset(u_Textures[0], textCoord, ivec2(0, -1)); break;
		case  1: texColor *= textureOffset(u_Textures[1], textCoord, ivec2(0, -1)); break;
		case  2: texColor *= textureOffset(u_Textures[2], textCoord, ivec2(0, -1)); break;
		case  3: texColor *= textureOffset(u_Textures[3], textCoord, ivec2(0, -1)); break;
		case  4: texColor *= textureOffset(u_Textures[4], textCoord, ivec2(0, -1)); break;
		case  5: texColor *= textureOffset(u_Textures[5], textCoord, ivec2(0, -1)); break;
		case  6: texColor *= textureOffset(u_Textures[6], textCoord, ivec2(0, -1)); break;
		case  7: texColor *= textureOffset(u_Textures[7], textCoord, ivec2(0, -1)); break;
		case  8: texColor *= textureOffset(u_Textures[8], textCoord, ivec2(0, -1)); break;
		case  9: texColor *= textureOffset(u_Textures[9], textCoord, ivec2(0, -1)); break;
		case 10: texColor *= textureOffset(u_Textures[10], textCoord, ivec2(0, -1)); break;
		case 11: texColor *= textureOffset(u_Textures[11], textCoord, ivec2(0, -1)); break;
		case 12: texColor *= textureOffset(u_Textures[12], textCoord, ivec2(0, -1)); break;
		case 13: texColor *= textureOffset(u_Textures[13], textCoord, ivec2(0, -1)); break;
		case 14: texColor *= textureOffset(u_Textures[14], textCoord, ivec2(0, -1)); break;
		case 15: texColor *= textureOffset(u_Textures[15], textCoord, ivec2(0, -1)); break;
		case 16: texColor *= textureOffset(u_Textures[16], textCoord, ivec2(0, -1)); break;
		case 17: texColor *= textureOffset(u_Textures[17], textCoord, ivec2(0, -1)); break;
		case 18: texColor *= textureOffset(u_Textures[18], textCoord, ivec2(0, -1)); break;
		case 19: texColor *= textureOffset(u_Textures[19], textCoord, ivec2(0, -1)); break;
		case 20: texColor *= textureOffset(u_Textures[20], textCoord, ivec2(0, -1)); break;
		case 21: texColor *= textureOffset(u_Textures[21], textCoord, ivec2(0, -1)); break;
		case 22: texColor *= textureOffset(u_Textures[22], textCoord, ivec2(0, -1)); break;
		case 23: texColor *= textureOffset(u_Textures[23], textCoord, ivec2(0, -1)); break;
		case 24: texColor *= textureOffset(u_Textures[24], textCoord, ivec2(0, -1)); break;
		case 25: texColor *= textureOffset(u_Textures[25], textCoord, ivec2(0, -1)); break;
		case 26: texColor *= textureOffset(u_Textures[26], textCoord, ivec2(0, -1)); break;
		case 27: texColor *= textureOffset(u_Textures[27], textCoord, ivec2(0, -1)); break;
		case 28: texColor *= textureOffset(u_Textures[28], textCoord, ivec2(0, -1)); break;
		case 29: texColor *= textureOffset(u_Textures[29], textCoord, ivec2(0, -1)); break;
		case 30: texColor *= textureOffset(u_Textures[30], textCoord, ivec2(0, -1)); break;
		case 31: texColor *= textureOffset(u_Textures[31], textCoord, ivec2(0, -1)); break;
		}
	}
	if (index == 3)
	{
		switch (int(v_TexIndex))
		{
		case  0: texColor *= textureOffset(u_Textures[0], textCoord, ivec2(0, 1)); break;
		case  1: texColor *= textureOffset(u_Textures[1], textCoord, ivec2(0, 1)); break;
		case  2: texColor *= textureOffset(u_Textures[2], textCoord, ivec2(0, 1)); break;
		case  3: texColor *= textureOffset(u_Textures[3], textCoord, ivec2(0, 1)); break;
		case  4: texColor *= textureOffset(u_Textures[4], textCoord, ivec2(0, 1)); break;
		case  5: texColor *= textureOffset(u_Textures[5], textCoord, ivec2(0, 1)); break;
		case  6: texColor *= textureOffset(u_Textures[6], textCoord, ivec2(0, 1)); break;
		case  7: texColor *= textureOffset(u_Textures[7], textCoord, ivec2(0, 1)); break;
		case  8: texColor *= textureOffset(u_Textures[8], textCoord, ivec2(0, 1)); break;
		case  9: texColor *= textureOffset(u_Textures[9], textCoord, ivec2(0, 1)); break;
		case 10: texColor *= textureOffset(u_Textures[10], textCoord, ivec2(0, 1)); break;
		case 11: texColor *= textureOffset(u_Textures[11], textCoord, ivec2(0, 1)); break;
		case 12: texColor *= textureOffset(u_Textures[12], textCoord, ivec2(0, 1)); break;
		case 13: texColor *= textureOffset(u_Textures[13], textCoord, ivec2(0, 1)); break;
		case 14: texColor *= textureOffset(u_Textures[14], textCoord, ivec2(0, 1)); break;
		case 15: texColor *= textureOffset(u_Textures[15], textCoord, ivec2(0, 1)); break;
		case 16: texColor *= textureOffset(u_Textures[16], textCoord, ivec2(0, 1)); break;
		case 17: texColor *= textureOffset(u_Textures[17], textCoord, ivec2(0, 1)); break;
		case 18: texColor *= textureOffset(u_Textures[18], textCoord, ivec2(0, 1)); break;
		case 19: texColor *= textureOffset(u_Textures[19], textCoord, ivec2(0, 1)); break;
		case 20: texColor *= textureOffset(u_Textures[20], textCoord, ivec2(0, 1)); break;
		case 21: texColor *= textureOffset(u_Textures[21], textCoord, ivec2(0, 1)); break;
		case 22: texColor *= textureOffset(u_Textures[22], textCoord, ivec2(0, 1)); break;
		case 23: texColor *= textureOffset(u_Textures[23], textCoord, ivec2(0, 1)); break;
		case 24: texColor *= textureOffset(u_Textures[24], textCoord, ivec2(0, 1)); break;
		case 25: texColor *= textureOffset(u_Textures[25], textCoord, ivec2(0, 1)); break;
		case 26: texColor *= textureOffset(u_Textures[26], textCoord, ivec2(0, 1)); break;
		case 27: texColor *= textureOffset(u_Textures[27], textCoord, ivec2(0, 1)); break;
		case 28: texColor *= textureOffset(u_Textures[28], textCoord, ivec2(0, 1)); break;
		case 29: texColor *= textureOffset(u_Textures[29], textCoord, ivec2(0, 1)); break;
		case 30: texColor *= textureOffset(u_Textures[30], textCoord, ivec2(0, 1)); break;
		case 31: texColor *= textureOffset(u_Textures[31], textCoord, ivec2(0, 1)); break;
		}
	}
	return texColor;
}

void main()
{
	color = getTextureColor(Input.TexCoord);
	if (color.w == 0)
	{
		discard;
	}

	vec4 leftColor = getTextureColorOffset(Input.TexCoord, 0);
	vec4 rightColor = getTextureColorOffset(Input.TexCoord, 1);
	vec4 upColor = getTextureColorOffset(Input.TexCoord, 2);
	vec4 downColor = getTextureColorOffset(Input.TexCoord, 3);

	ivec2 size = getTextureSize();

	if (leftColor.w == 0 || rightColor.w == 0 || upColor.w == 0 || downColor.w == 0 || Input.TexCoord.x*size.x <= 1 || Input.TexCoord.y*size.y <= 1 || Input.TexCoord.x*size.x >= size.x - 1 || Input.TexCoord.y*size.y >= size.y - 1)
	{
		color = Input.Color;
	}
	else
	{
		discard;
	}

	//color = vec4(Input.TexCoord, 0.0, 1.0);

	entityID = v_EntityID;
}