#version 330
smooth in vec3 theColor;
//light space coordinate, used for shadow map
//smooth in vec4 lightNDCPos;
//uniform sampler2DShadow shadowMap;
//
out vec4 outputColor;
 
void main()
{
 outputColor=vec4(theColor,1.0);
 }
