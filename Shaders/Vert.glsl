#version 400 compatibility

uniform mat4 Projection;
uniform mat4 World;
uniform mat4 View;
uniform mat3 TexWorld;

layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;
layout(location = 2) in vec3 Texcoord;
layout(location = 3) in vec4 Color;

out vec3 VertexPosition;
out vec3 VertexTexCoords;
out vec3 VertexNormal;
out vec4 VertexColor;
out vec3 PixelPosition;

void main() {
	gl_Position = Projection * View * World * Position;
	PixelPosition = (Projection * World * Position).xyz;

	VertexPosition = Position.xyz;
	VertexNormal = Normal.xyz;
	
	vec2 scaledTexCoords = Texcoord.xy * vec2(length(World[0]), length(World[1]));
	VertexTexCoords = vec3((TexWorld * vec3(scaledTexCoords, 1.f)).xy, 0.f);
	VertexColor = Color;
}