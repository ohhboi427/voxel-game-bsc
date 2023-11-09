#version 460 core

const vec2 UVs[3] =
{
	vec2(0.0, 0.0),
	vec2(2.0, 0.0),
	vec2(0.0, 2.0),
};

layout(location = 0) out vec2 UV;

void main()
{
	UV = UVs[gl_VertexID];

	gl_Position = vec4((UV * 2.0) - 1.0, 0.0, 1.0);
}
