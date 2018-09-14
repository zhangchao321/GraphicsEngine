#version 330 core
#extension GL_NV_shadow_samplers_cube : enable
in vec3 V_Texcoord;
uniform samplerCube U_Texture;
out vec4 outColor;
void main()
{
	outColor=textureCube(U_Texture,normalize(V_Texcoord));
}