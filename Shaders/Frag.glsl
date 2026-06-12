#version 420

in vec3 VertexTexCoords;
in vec4 VertexColor;
in vec3 PixelPosition;

struct Light {
	vec4 Color;
	vec3 Attenuation;
	vec3 Position;
	float Range;
	bool Enabled;
};

layout(std140, binding = 0) uniform Lights {
	Light lights[100];
};

uniform vec4 RenderColor;
uniform vec4 SelectionColor;
uniform vec2 TextureAngle;

uniform int NumLights = 0;
uniform bool IsObjectSelected;
uniform bool IsObjectInvisible;
uniform bool IsLightsEnabled;
uniform bool fTexture;

out vec4 Color;

uniform sampler2D ourTexture;

void main() {
	Color = VertexColor * RenderColor;
	if (fTexture) {
		vec2 textureCoords = vec2(VertexTexCoords);
		vec2 textureRotation = vec2(tan(textureCoords.x), tan(textureCoords.y));

		vec4 TextureColor = texture(ourTexture, textureCoords);
		Color *= TextureColor;
	}

	if (IsObjectSelected)
		Color = mix(Color, SelectionColor, 0.5f);

	if (IsObjectInvisible)
		Color.w = 0.5f;

	if (IsLightsEnabled) {
		vec3 finalLightColor = vec3(0.f, 0.f, 0.f);

		for (int i = 0; i < NumLights; ++i) {
			if (lights[i].Enabled) {
				float distance = length(lights[i].Position - PixelPosition) / lights[i].Range;
				vec3 lightColor = vec3(lights[i].Color);
				lightColor /= lights[i].Attenuation.x + (lights[i].Attenuation.y * distance) + (lights[i].Attenuation.z * distance * distance);
				
				finalLightColor += lightColor;
			}
		}

		Color.rgb = clamp(Color.rgb * finalLightColor, 0.f, 1.f);
	}
}	