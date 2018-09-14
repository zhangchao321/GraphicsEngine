#include "vertexbuffer.h"
#include "utils.h"
GLuint VertexBuffer::CreateBufferObject(GLenum bufferType, GLsizeiptr size, GLenum usage, void*data) {
    GLuint object;
    glGenBuffers(1, &object);
    glBindBuffer(bufferType, object);
    glBufferData(bufferType, size, data, usage);
    glBindBuffer(bufferType, 0);
    return object;
}
void VertexBuffer::SetSize(int vertexCount) {
    mVertexCount = vertexCount;
    mVertexes = new Vertex[mVertexCount];
    memset(mVertexes, 0, sizeof(Vertex)*mVertexCount);
    mVBO = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(Vertex)*mVertexCount, GL_STATIC_DRAW, nullptr);
}
void VertexBuffer::SetPosition(int index, float x, float y, float z, float w) {
    mVertexes[index].Position[0] = x;
    mVertexes[index].Position[1] = y;
    mVertexes[index].Position[2] = z;
    mVertexes[index].Position[3] = w;
}
void VertexBuffer::SetColor(int index, float r, float g, float b, float a) {
    mVertexes[index].Color[0] = r;
    mVertexes[index].Color[1] = g;
    mVertexes[index].Color[2] = b;
    mVertexes[index].Color[3] = a;
}
void VertexBuffer::SetTexcoord(int index, float x, float y) {
    mVertexes[index].Texcoord[0] = x;
    mVertexes[index].Texcoord[1] = y;
}
void VertexBuffer::SetNormal(int index, float x, float y, float z) {
    mVertexes[index].Normal[0] = x;
    mVertexes[index].Normal[1] = y;
    mVertexes[index].Normal[2] = z;
    mVertexes[index].Normal[3] = 1.0;
}
void VertexBuffer::Bind() {
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex)*mVertexCount, mVertexes);
}
void VertexBuffer::Unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
Vertex&VertexBuffer::Get(int index) {
    return mVertexes[index];
}
