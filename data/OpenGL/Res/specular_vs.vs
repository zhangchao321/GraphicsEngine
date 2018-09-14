#version 330 core
in vec4 position;
in vec4 texcoord;
in vec4 normal;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 IT_ModelMatrix;
uniform vec4 U_AmbientMaterial;
uniform vec4 U_DiffuseMaterial;
uniform vec4 U_SpecularMaterial;
uniform vec4 U_AmbientLight;
uniform vec4 U_DiffuseLight;
uniform vec4 U_LightPos;
uniform vec4 U_SpecularLight;
uniform vec4 U_CameraPos;
out vec4 V_Color;
void main()
{
	vec3 L =U_LightPos.xyz;
	L =normalize(L);
	vec3 n=normalize((IT_ModelMatrix*normal).xyz);
	float diffuseIntensity =max(0.0,dot(L,n));
	vec4 specularColor=vec4(0.0,0.0,0.0,0.0);
	if(diffuseIntensity>0.0)
	{
		vec3 reflectVec =reflect(-L,n);
		reflectVec =normalize(reflectVec);
		vec3 worldPos =(ModelMatrix*position).xyz;
		vec3 viewVec =U_CameraPos.xyz-worldPos;
		viewVec =normalize(viewVec);
		specularColor =U_SpecularLight*U_SpecularMaterial*pow(max(0,dot(viewVec,reflectVec)),4.0);
	}
	vec4 diffuseColor =U_DiffuseLight*U_DiffuseMaterial*diffuseIntensity;
	V_Color =diffuseColor+U_AmbientLight*U_AmbientMaterial+specularColor;
	gl_Position =ProjectionMatrix*ViewMatrix*ModelMatrix*position;
}