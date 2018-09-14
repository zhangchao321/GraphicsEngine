#version 330 core 
#extension GL_NV_shadow_samplers_cube : enable
uniform samplerCube U_Texture;
uniform vec4 U_CameraPos;
in vec4 V_WorldPos;
in vec4 V_Normal;
out vec4 outColor;
void main()
{
	vec3 eyeVec =V_WorldPos.xyz-U_CameraPos.xyz;
	vec3 n =normalize(V_Normal.xyz);
	vec3 r =refract(eyeVec,n,1.0/1.52);
	outColor =textureCube(U_Texture,r);
}