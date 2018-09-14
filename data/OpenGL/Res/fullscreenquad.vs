#version 330 core
in vec4 position;
in vec4 texcoord;
out vec2 V_TexCoord;
void main()
{
	V_TexCoord =texcoord.xy;
	gl_Position =position;
}