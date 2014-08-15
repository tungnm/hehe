#version 330

uniform mat4 MV;//modelView
uniform mat4 MVP;
uniform mat3 normalTransform; 


//note:
 //The fragment shader has access to the same set of uniforms as the vertex shader, 
 //but cannot declare or access attribute variables

layout(location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

 smooth out vec3 pos;
 smooth out vec3 norm;
 smooth out float depth;

void main()
{
	pos=vec3(MV*vec4(position,1.0));
	gl_Position = MVP*vec4(position.x,position.y,position.z,1.0);//
	 depth = (-pos.z)/(25.0);
	norm=normalize(normalTransform*normal); //normal to world space too
}