#version 330 core
in vec4 position;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
void main()
{
	gl_Position =ProjectionMatrix*ViewMatrix*ModelMatrix*position;
}