#version 330 core
in vec4 position;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
out vec4 V_Color;
void main()
{
	V_Color =vec4(position.x+0.5, position.y+0.5, position.z+0.5,1.0f); 
	gl_Position =ProjectionMatrix*ViewMatrix*ModelMatrix*position;
}