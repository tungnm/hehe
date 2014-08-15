#version 330

uniform mat4 MV;
uniform mat4 MVP;
uniform mat3 normalTransform; 

uniform vec3 color;

layout(location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

smooth out vec3 theColor;

void main()
{

//    vec4 totalOffset = vec4(offset.x, offset.y, 0.0, 0.0);
    // gl_Position = position;
	

	gl_Position = MVP*vec4(position.x,position.y,position.z,1.0);// + totalOffset;
	//gl_Position = position;
	//theColor=vec4(1.0,0.0,0.0,1.0);
	//theColor=vec4(1.0,1.0,0.0,1.0);
	
	vec3 N=normalize(normalTransform*normal);
	//vec3 N=normalize(normal);
	vec3 eyeCord=( MV*vec4(position,1.0)).xyz;
	
	vec3 V=normalize(-eyeCord);
	//vec3 V=vec3(0.0,0.0,1.0); //non local viewer trick
	
	vec3 L=normalize(vec3(1.0,3.0,2.0)-eyeCord);//directionalLight);
	//vec3 L=normalize(light.xyz-eyeCord);//i dont multiply light by MV so light will stay still with viewer, all object get lit
	
	
	
	//vec3 L=MV*vec3 (0.5773,0.5773,0.5773); //directional light trick, all L vectors are parralel, representing powerful but far away light(like the sun)
	//this is a normalized 45 degree vector
	vec3 R=reflect(-L,N);
	vec3 diffuse;
	vec3 specular=vec3(0.0);
	float kd=max(dot(L,N),0.0);



	diffuse=color*kd;
	vec3 ambient=vec3(0.1,0.1,0.1);
	if (kd>0.0)
	specular=vec3(0.5,0.5,0.5)*pow(max(dot(R,V),0.0),5);

	

	theColor=ambient+diffuse+specular;
}