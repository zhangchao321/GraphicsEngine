#version 330 core
in vec4 position;
in vec4 normal;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 IT_ModelMatrix;
out vec4 V_WorldPos;
out vec4 V_Normal;
void main()
{
	V_WorldPos=ModelMatrix*position;
	V_Normal=IT_ModelMatrix*normal;
	gl_Position=ProjectionMatrix*ViewMatrix*ModelMatrix*position;
}
