#version 330
 smooth in vec3 pos;
 smooth in vec3 norm;//already normalized and in world coordinate
  in vec2 outTextCord;
uniform sampler2D objectTex; 
uniform samplerCube diffuseIrrTex;

//uniform samplerCube diffuseIrrMap;
uniform vec3 camPos; 
uniform vec3 lightPos;//this one is also calculated from the client program and sent to shaders
 out vec4 outputColor;

void main()
{
/*diffuse irradiance map
	//	vec3 V=normalize(camPos-pos);
	//vec3 rV=reflect(-V,norm);
	//vec4 baseColor=vec4(0.6,0.2,0.2,1.0);
	vec4 diffuseIrr=texture(diffuseIrrTex,norm);
	vec4 texColor=texture(objectTex, outTextCord);
	outputColor=diffuseIrr;//*texColor;//*baseColor;//+specular*0.4;	
	*/

	//vec4 light=vec4(1.0,0.0,-8.0,1.0);
	vec3 V=normalize(camPos-pos);
	vec3 L=normalize(lightPos-pos);
	vec3 R=reflect(-L,norm);
	vec3 diffuse;
	vec3 specular=vec3(0.0);
	float kd=max(dot(L,norm),0.0);
	
	//with texture
	vec4 texColor=texture(objectTex, outTextCord);
	
	diffuse=vec3(0.5,0.5,0.5)*kd;
	vec3 ambient=vec3(0.1,0.1,0.1);
	if (kd>0.0)
	specular=vec3(0.4,0.4,0.4)*pow(max(dot(R,V),0.0),3);
	outputColor= vec4(ambient+kd*texColor.xyz+specular,1.0);
	//no shading:
	//outputColor=vec4(0.6,0.6,0.6,1.0);






}

//----------------diffuse gonna use diffuse irradiance environment map. So now virtual light for now
	//----------------diffuse value is independent of viewer and only depend on normal vector. So we only need the normal vector in
	//world space( not camera space)
	//NOTE: normal phong model need the V vector(from surface to viewer) to calculate the specular, that why we bring every other vector
	//to camera space
	//for cubemap technique, on the other hand, we need a viewer vector, and surface normal. The camera and the sky box always stay at
	//the center. The calculation of the ouput fragment color here depends on the relative directions of viewer and normal vector to the
	//skybox, and not the relativity in direction of these 2 vectors themself (like in calculation for normal phong). 
	//The transfer to camera space would make the direction of normal and viewer vector incorrect.
	
	//of all vector,


//vec3 V=vec3(0.0,0.0,1.0); //non local viewer trick NOTE: non local trick will not work well with environment map
	//vec3 L=normalize(light.xyz-pos);
	//vec3 L=MV*vec3 (0.5773,0.5773,0.5773); //directional light trick, all L vectors are parralel, representing powerful but far away light(like the sun)
	//vec3 R=reflect(-L,norm);
	//vec3 specular=vec3(0.0);
	//float kd=max(dot(L,norm),0.0);
	//vec3 ambient=vec3(0.1,0.1,0.1);
	//if (kd>0.0)
	//specular=vec3(0.4,0.4,0.4)*pow(max(dot(R,V),0.0),8	);