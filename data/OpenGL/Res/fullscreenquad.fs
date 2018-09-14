#version 330 core
in vec2 V_TexCoord;
uniform sampler2D U_Texture;
void main()
{
	gl_FragColor =texture2D(U_Texture,V_TexCoord);
}