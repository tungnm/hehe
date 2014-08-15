#version 330
uniform mat4 MV;//this one is to position the quad on 2d screen
layout(location = 0) in vec3 position;
layout (location=2) in vec2 texCord;
out vec2 outTextCord;
void main()
{
	gl_Position = MV*vec4(position.x,position.y,position.z,1.0);//
	outTextCord=texCord; //pass the texture coordinate directly to fragment shader
}