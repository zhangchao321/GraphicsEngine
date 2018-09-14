#include "shader.h"
#include "utils.h"
#include "vertexbuffer.h"
GLuint Shader::CompileShader(GLenum shaderType, const char*shaderCode) {
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderCode, nullptr);
	glCompileShader(shader);
	GLint compileResult = GL_TRUE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		char szLog[1024] = { 0 };
		GLsizei logLen = 0;
		glGetShaderInfoLog(shader, 1024, &logLen, szLog);
		printf("Compile Shader fail error log : %s \nshader code :\n%s\n", szLog, shaderCode);
		glDeleteShader(shader);
		shader = 0;
	}
	return shader;
}
GLuint Shader::CreateProgram(GLuint vsShader, GLuint fsShader) {
	GLuint program = glCreateProgram();
	glAttachShader(program, vsShader);
	glAttachShader(program, fsShader);
	glLinkProgram(program);
	glDetachShader(program, vsShader);
	glDetachShader(program, fsShader);
	GLint nResult;
	glGetProgramiv(program, GL_LINK_STATUS, &nResult);
	if (nResult == GL_FALSE) {
		char log[1024] = { 0 };
		GLsizei writed = 0;
		glGetProgramInfoLog(program, 1024, &writed, log);
		printf("create gpu program fail,link error : %s\n", log);
		glDeleteProgram(program);
		program = 0;
	}
	return program;
}
void Shader::Init(const char*vs, const char*fs) {
	int nFileSize = 0;
	const char*vsCode = (char*)LoadFileContent(vs,nFileSize);
	const char*fsCode = (char*)LoadFileContent(fs,nFileSize);
	GLuint vsShader = CompileShader(GL_VERTEX_SHADER, vsCode);
	if (vsShader==0){
		return;
	}
	GLuint fsShader = CompileShader(GL_FRAGMENT_SHADER, fsCode);
	if (fsShader == 0) {
		return;
	}
	mProgram=CreateProgram(vsShader, fsShader);
	glDeleteShader(vsShader);
	glDeleteShader(fsShader);
	if (mProgram!=0){
		mModelMatrixLocation = glGetUniformLocation(mProgram, "ModelMatrix");
		mViewMatrixLocation = glGetUniformLocation(mProgram, "ViewMatrix");
		mProjectionMatrixLocation = glGetUniformLocation(mProgram, "ProjectionMatrix");
		mPositionLocation = glGetAttribLocation(mProgram, "position");
		mColorLocation = glGetAttribLocation(mProgram, "color");
		mTexcoordLocation = glGetAttribLocation(mProgram, "texcoord");
		mNormalLocation = glGetAttribLocation(mProgram, "normal");
	}
}
void Shader::Bind(float *M, float *V, float*P) {
	glUseProgram(mProgram);
	if(mModelMatrixLocation >= 0)glUniformMatrix4fv(mModelMatrixLocation, 1, GL_FALSE, M);
	if (mViewMatrixLocation >= 0)glUniformMatrix4fv(mViewMatrixLocation, 1, GL_FALSE, V);
	if (mProjectionMatrixLocation >= 0)glUniformMatrix4fv(mProjectionMatrixLocation, 1, GL_FALSE, P);
	int iIndex = 0;
	for (auto iter = mUniformTextures.begin(); iter != mUniformTextures.end(); ++iter) {
		glActiveTexture(GL_TEXTURE0 + iIndex);
		glBindTexture(GL_TEXTURE_2D, iter->second->mTexture);
		glUniform1i(iter->second->mLocation, iIndex++);
	}
	for (auto iter = mUniformTextureCubes.begin(); iter != mUniformTextureCubes.end(); ++iter) {
		glActiveTexture(GL_TEXTURE0 + iIndex);
		glBindTexture(GL_TEXTURE_CUBE_MAP, iter->second->mTexture);
		glUniform1i(iter->second->mLocation, iIndex++);
	}
	for (auto iter = mUniformVec4s.begin(); iter != mUniformVec4s.end(); ++iter) {
		glUniform4fv(iter->second->mLocation, 1, iter->second->v);
	}
	if (mPositionLocation >= 0) {
		glEnableVertexAttribArray(mPositionLocation);
		glVertexAttribPointer(mPositionLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	}
	if (mColorLocation >= 0) {
		glEnableVertexAttribArray(mColorLocation);
		glVertexAttribPointer(mColorLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 4));
	}
	if (mTexcoordLocation >= 0) {
		glEnableVertexAttribArray(mTexcoordLocation);
		glVertexAttribPointer(mTexcoordLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 8));
	}
	if (mNormalLocation >= 0) {
		glEnableVertexAttribArray(mNormalLocation);
		glVertexAttribPointer(mNormalLocation, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 12));
	}
}
void Shader::SetTexture(const char * name, const char*imagePath) {
	auto iter = mUniformTextures.find(name);
	if (iter == mUniformTextures.end()) {
		GLint location = glGetUniformLocation(mProgram, name);
		if (location != -1) {
			UniformTexture*t = new UniformTexture;
			t->mLocation = location;
			t->mTexture = CreateTexture2DFromBMP(imagePath);
			mUniformTextures.insert(std::pair<std::string, UniformTexture*>(name, t));
		}
	} else {
		glDeleteTextures(1, &iter->second->mTexture);
		iter->second->mTexture = CreateTexture2DFromBMP(imagePath);
	}
}
void Shader::SetVec4(const char * name, float x, float y, float z, float w) {
	auto iter = mUniformVec4s.find(name);
	if (iter == mUniformVec4s.end()) {
		GLint location = glGetUniformLocation(mProgram, name);
		if (location != -1) {
			UniformVector4f*v = new UniformVector4f;
			v->v[0] = x;
			v->v[1] = y;
			v->v[2] = z;
			v->v[3] = w;
			v->mLocation = location;
			mUniformVec4s.insert(std::pair<std::string, UniformVector4f*>(name, v));
		}
	} else {
		iter->second->v[0] = x;
		iter->second->v[1] = y;
		iter->second->v[2] = z;
		iter->second->v[3] = w;
	}
}
GLuint Shader::SetTexture(const char * name, GLuint texture) {
	auto iter = mUniformTextures.find(name);
	GLuint oldTexture = 0;
	if (iter == mUniformTextures.end()) {
		GLint location = glGetUniformLocation(mProgram, name);
		if (location != -1) {
			UniformTexture*t = new UniformTexture;
			t->mLocation = location;
			t->mTexture = texture;
			mUniformTextures.insert(std::pair<std::string, UniformTexture*>(name, t));
		}
	}
	else {
		oldTexture = iter->second->mTexture;
		iter->second->mTexture = texture;
	}
	return oldTexture;
}
GLuint Shader::SetTextureCube(const char * name, GLuint texture) {
	auto iter = mUniformTextureCubes.find(name);
	GLuint oldTexture = 0;
	if (iter == mUniformTextureCubes.end()) {
		GLint location = glGetUniformLocation(mProgram, name);
		if (location != -1) {
			UniformTextureCube*t = new UniformTextureCube;
			t->mLocation = location;
			t->mTexture = texture;
			mUniformTextureCubes.insert(std::pair<std::string, UniformTextureCube*>(name, t));
		}
	}
	else {
		oldTexture = iter->second->mTexture;
		iter->second->mTexture = texture;
	}
	return oldTexture;
}
