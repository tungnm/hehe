#version 330
 in vec3 pos;
  in vec3 norm;
  uniform mat4 MV;
  uniform sampler2D depthSampler;
  //in vec2 outTextCord;
  in float depth;
layout (location = 0) out vec4 outputColor;
layout (location = 1) out vec3 diffToG;
layout (location = 2) out vec3 normToG;
layout (location = 3) out vec3 depthToG; //this list match the glDrawBuffer[GL_none, GL_COLORattachmetn0,GL_COLORattachmetn1,GL_COLORattachmetn2]


void main()
{
/*
	vec3 s = normalize((MV*vec4(1.0,7.0,1.0,1.0)).xyz - pos);
  float sDotN = max( dot(s,norm), 0.0 );
  vec3 diffuse = vec3(0.5,0.5,0.5)* sDotN;
  */
//this shader simple save all the data to G buffer without any calculation on lighting, shading
//diffToG=vec3(0.6,0.6,0.6);
	
	/*
	vec4 lightPos=MV*vec4(1.0,0.0,-8.0,1.0);
	vec3 V=normalize(-pos);
	vec3 L=normalize(lightPos-pos);
	vec3 R=reflect(-s,norm);
	vec3 diffuse;
	vec3 specular=vec3(0.0);
	float kd=max(dot(L,norm),0.0);

	diffuse=vec3(0.5,0.5,0.5)*kd;
	vec3 ambient=vec3(0.1,0.1,0.1);
	if (kd>0.0)
	specular=vec3(0.4,0.4,0.4)*pow(max(dot(R,V),0.0),3);
	*/









  vec3 s = normalize((MV*vec4(1.0,7.0,1.0,1.0)).xyz - pos);
  float sDotN = max( dot(s,norm), 0.0 );
  vec3 R=reflect(-s,norm);vec3 V=normalize(-pos);
  vec3 specular=vec3(0.0);
  if (sDotN>0.0)
	specular=vec3(0.4,0.4,0.4)*pow(max(dot(R,V),0.0),3);
  vec3 diffuse = vec3(0.5,0.5,0.5)* sDotN;
  vec3 ambient=vec3(0.1,0.1,0.1);
  diffToG=ambient+diffuse+specular;
normToG=norm;
depthToG=vec3(depth);
//hard coded the diffuse here, the base color, this one coudl be texture without shading of the object
//note that this one doesn't output the location - outputColor( this location 0 is probally for rendering to default frame
//buffer

}