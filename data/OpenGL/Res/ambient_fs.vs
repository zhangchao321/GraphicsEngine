#version 330 core
in vec4 position;
in vec4 texcoord;
in vec4 normal;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
out vec4 V_Color;
void main()
{ 
	gl_Position =ProjectionMatrix*ViewMatrix*ModelMatrix*position;
}