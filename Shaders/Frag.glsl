#version 400

in vec3 VertexTexCoords;
in vec4 VertexColor;

uniform vec4 RenderColor;
uniform vec2 TextureAngle;

uniform bool IsObjectSelected;
uniform bool IsObjectInvisible;

out vec4 Color;

uniform sampler2D ourTexture;

void main() {
	vec2 textureCoords = vec2(VertexTexCoords);
	vec2 textureRotation = vec2(tan(textureCoords.x / 180.f * 3.14f), tan(textureCoords.y / 180.f * 3.14f));

	vec4 TextureColor = texture(ourTexture, textureCoords);
	if (textureSize(ourTexture, 0) == vec2(1.f))
		TextureColor = vec4(1.f);
	Color = VertexColor * TextureColor * RenderColor;

	if (IsObjectSelected)
		Color = mix(Color, vec4(0.f, 1.f, 0.f, 1.f), 0.5f);
	if (IsObjectInvisible)
		Color.w = 0.5f;

}	