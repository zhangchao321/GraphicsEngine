#version 330 core
uniform vec4 U_AmbientMaterial;
uniform vec4 U_AmbientLight;
out vec4 outColor;
void main()
{
	outColor =U_AmbientMaterial*U_AmbientLight;
}