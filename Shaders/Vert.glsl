#version 400 compatibility	
							
uniform mat4 Projection;
uniform mat4 World;
uniform mat4 View;
							
layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;
layout(location = 2) in vec3 Texcoord;
layout(location = 3) in vec4 Color;

out vec3 VertexTexCoords;	
							
void main() {				
	gl_Position = Projection * View * World * Position;
							
	VertexTexCoords = Texcoord * vec3(length(World[0]), length(World[1]), 1.f);
}