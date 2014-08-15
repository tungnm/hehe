#version 330

//note:
 //The fragment shader has access to the same set of uniforms as the vertex shader, 
 //but cannot declare or access attribute variables

layout(location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location=2) in vec2 texCord;


 out vec2 outTextCord;
void main()
{
	
	gl_Position = vec4(position.x,position.y,position.z,1.0);//
	outTextCord=texCord; //pass the texture coordinate directly to fragment shader
}