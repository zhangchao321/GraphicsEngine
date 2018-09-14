#version 330 core
in vec2 V_TexCoord;
uniform sampler2D U_Texture;
void main()
{
	vec4 color =texture2D(U_Texture,V_TexCoord);
	float gray =(color.r+color.g+color.b)/3;
	gl_FragColor =vec4(gray, gray, gray,1.0);
}