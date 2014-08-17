#version 330

//this is a common vertex shader for skybox, nothing special, just calculate the reflected vector and pass it to fragment shader
uniform mat4 M;
uniform mat4 MVP;
uniform vec3 camPos;
layout(location = 0) in vec3 position;
out vec3 reflectDir;

void main()
{
	vec3 worldPos=vec3(M*vec4(position,1.0));
	reflectDir=worldPos-camPos;
	gl_Position = MVP*vec4(position.x,position.y,position.z,1.0);//
	
}