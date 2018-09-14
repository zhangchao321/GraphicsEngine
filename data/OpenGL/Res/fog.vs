#version 330 core
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
in vec4 position;
out vec4 V_Color;
out vec4 V_EyeSpacePos;
void main()
{
	V_EyeSpacePos =ViewMatrix*ModelMatrix*position;
	V_Color =vec4(position.x+0.5,position.y+0.5,position.z+0.5,1);
	gl_Position =ProjectionMatrix*ViewMatrix*ModelMatrix*position;
}