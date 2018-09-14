#version 330 core
uniform vec4 U_FogOption;
uniform vec4 U_FogColor;
in vec4 V_Color;
in vec4 V_EyeSpacePos;
out vec4 outColor;
float CalculateExpFog(float distance)
{
	float fogAlpha =exp(-(distance*U_FogOption.z));
	fogAlpha =clamp(fogAlpha,0,1);
	return fogAlpha;
}

void main()
{
	float fogAlpha =CalculateExpFog(V_EyeSpacePos.z/V_EyeSpacePos.w);
	outColor =mix(U_FogColor,V_Color,fogAlpha);
}