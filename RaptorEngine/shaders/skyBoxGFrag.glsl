#version 330

in vec3 reflectDir;
uniform samplerCube CubeMapTex; 

//out vec4 outputColor;

layout (location = 3) out vec4 diffuseToG;

void main()
{

vec3 cubeTexture=texture(CubeMapTex,reflectDir).xyz;
diffuseToG=vec4(cubeTexture,-300.0);

//the line above is a  trick. So for deferred shading pipeline, after the build G-buffer pass, there's no distinction of object 
//anymore, all are pixels with data such as position, normal, tagent... stored in the G-Buffer. For this reason, inside the fragmentshader of
//the ProcessGBuffer pass, we would not know if the current fragment belogn to which object, skybox or just normal object?. So I invented
//a trick that I would store a number in the diffuseToG.a slot to identify the fragment type. In this case I store -300. For fire particle
//pixel, I stored -200
//note that after the BuildGBuffer pass, the next pass is just a Full Screen Quad render, with a fragment shader to use the 
//information in G-Buffer and do lighting calculation... In that fragment shader, I will do if statement to check the value of 
//diffuseToG.a and act accordingly.

//Refer to the DeferredNormalSSAO file to see this branching.
//Refer to GBuffer.SetUp to know more about the structure of G-Buffer textures
	
}

//this shader is special, because this is a deferred rendering pipeline, so the output of this shader will be the G-Buffer,
//So the location of output must match the layout of G-Buffer. Actually what I did with this shader is that I copy
//over the BuildGFrag.glsl file( the one that output to GBuffer all the data for normal mapping). Then I remove all the output I dont
//need, such as normalMapToG or tanToG.