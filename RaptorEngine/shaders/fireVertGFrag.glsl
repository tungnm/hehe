#version 330

uniform float transparency;
uniform sampler2D fireTexture;
smooth in vec2 outTexCoord;

uniform vec3 particleOrigin;
uniform float radius;

in vec2 fragmentScreenSpacePos;

//layout (location = 0) out vec4 outputColor;
//layout (location = 1) out vec3 posToG;
//layout (location = 2) out vec3 normToG;
layout (location = 3) out vec4 diffuseToG;
//layout (location = 4) out vec3 tanToG;
//layout (location = 5) out vec3 normalMapToG;

void main()
{
//calculate distance from particle origin:
//here we use all coordinate in screen space, and pass in a radius of the particle to use linear interpolation
//at the origin, the fragment has full opacity, at the radius, the fragment has 0 opacity.
float d= length((fragmentScreenSpacePos-particleOrigin.xy));
d/=radius*0.72;
float blending=max(0.0f,(1.0-d)*1.0+d*0.01);//linear interpolation
diffuseToG=vec4(texture(fireTexture,outTexCoord).xyz*blending*transparency,-200);
//outputColor=vec4(0.0);

}

//note: similar to the skybox fragment shader, this shader acts like a normal BuildGBuffer fragment shader, with the layout of ouput match the G-Buffer's
//also notice the trick of using normToG.a for classification in later ProcessGBuffer pass