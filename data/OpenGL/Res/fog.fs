#version 330 core
uniform vec4 U_FogOption;
uniform vec4 U_FogColor;
in vec4 V_Color;
in vec4 V_EyeSpacePos;
out vec4 outColor;
float CalculateLinearFog(float distance)
{
	float fogAlpha =(distance-U_FogOption.x)/(U_FogOption.y-U_FogOption.x);
	fogAlpha =1-clamp(fogAlpha,0,1);
	return fogAlpha;
}

void main()
{
	float fogAlpha =CalculateLinearFog(V_EyeSpacePos.z/V_EyeSpacePos.w);
	outColor =mix(U_FogColor,V_Color,fogAlpha);
}