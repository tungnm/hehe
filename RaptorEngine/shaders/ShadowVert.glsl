#version 330

layout (location=0) in vec3 VertexPosition;

uniform mat4 MVPLight;


void main()
{
vec3 transformPos=VertexPosition;

gl_Position = MVPLight * vec4(transformPos,1.0);

}