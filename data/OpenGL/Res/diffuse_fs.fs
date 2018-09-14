#version 330 core 
uniform vec4 U_AmbientMaterial;
uniform vec4 U_DiffuseMaterial;
uniform vec4 U_AmbientLight;
uniform vec4 U_DiffuseLight;
uniform vec4 U_LightPos;
in vec4 V_Normal;
out vec4 outColor;
void main()
{
	vec3 L =U_LightPos.xyz;
	L =normalize(L);
	vec3 n=normalize(V_Normal.xyz);
	float diffuseIntensity =max(0.0,dot(L,n));
	vec4 diffuseColor =U_DiffuseLight*U_DiffuseMaterial*diffuseIntensity;
	outColor =diffuseColor+U_AmbientLight*U_AmbientMaterial;
}