#version 330

uniform mat4 M;
uniform mat4 MVP;
uniform mat3 normalTransform; 


//note:
 //The fragment shader has access to the same set of uniforms as the vertex shader, 
 //but cannot declare or access attribute variables

layout(location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

 smooth out vec3 pos;
 smooth out vec3 norm;

void main()
{
	//it does not matter to use normal/light vector in world space or view space for normal Phong model
	//because the relative position of these
	//two vector are the same and their dot product will be the same 
	//---calculate vectors need for reflection in world space:
	pos=vec3(M*vec4(position,1.0));
	gl_Position = MVP*vec4(position.x,position.y,position.z,1.0);//
	norm=normalize(normalTransform*normal); //normal to world space too
}