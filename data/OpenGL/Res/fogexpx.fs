#version 330 core
uniform vec4 U_FogOption;
uniform vec4 U_FogColor;
in vec4 V_Color;
in vec4 V_EyeSpacePos;
out vec4 outColor;
float CalculateExpXFog(float distance)
{
	float fogAlpha =exp(-pow(distance*U_FogOption.z,U_FogOption.w));
	fogAlpha =clamp(fogAlpha,0,1);
	return fogAlpha;
}

void main()
{
	float fogAlpha =CalculateExpXFog(V_EyeSpacePos.z/V_EyeSpacePos.w);
	outColor =mix(U_FogColor,V_Color,fogAlpha);
}