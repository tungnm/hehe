#version 330

uniform mat4 M;
uniform mat4 MVP;
uniform mat3 normalTransform; 
uniform vec3 camPos;
layout(location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
 out vec3 reflectDir;
 


void main()

{	
	


	vec3 worldPos=vec3(M*vec4(position,1.0));
	vec3 worldNorm=normalize(normalTransform*normal); 
	vec3 worldView=normalize(camPos-worldPos);
	reflectDir=refract(-worldView, worldNorm, 0.67 );
	gl_Position = MVP*vec4(position.x,position.y,position.z,1.0);//
	
}