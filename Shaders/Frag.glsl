#version 420

in vec3 VertexPosition;
in vec3 VertexTexCoords;
in vec3 VertexNormal;
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

uniform bool fFullBright;
uniform bool fLight;
uniform vec3 AmbientColor;
uniform vec3 LightDirection;
uniform vec3 LightColor;

out vec4 Color;

uniform sampler2D ourTexture;


vec3 CalcDiffuse(vec3 normal, vec3 lightDir) {
	float dotNL = max(0, dot(normal, lightDir));
	return LightColor * dotNL;
}
vec3 CalcBlinnPhongSpecular(vec3 normal, vec3 lightDir) {
	vec3 blinnPhong = normalize(lightDir + normal);
	float dotBlinnPhong = max(0, dot(normal, blinnPhong));
	return LightColor * pow(dotBlinnPhong, 32.f);
}
vec4 CalcDirectionLight() {
	vec3 lightDir = -LightDirection;
	vec3 viewDir = normalize(-VertexPosition);

	vec3 diffuse = CalcDiffuse(VertexNormal, lightDir);
	vec3 specular = CalcBlinnPhongSpecular(VertexNormal, lightDir);
	return vec4(AmbientColor + diffuse + specular, 1.f);
}

void main() {
	Color = VertexColor * RenderColor;
	if (fLight && !fFullBright) {
		Color *= CalcDirectionLight();
	}

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