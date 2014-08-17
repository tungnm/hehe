#version 330

//BASICALLY THE SAME AS QUADFRAG, ONLY THAT ALL TEXT USE THE SAME TEXTURE SO ONLY NEED TO PASS THIS UNIFORM ONCE
//ALSO NEED TO DISCARD BLACK PIXEL 
in vec2 outTextCord;
uniform sampler2D tex1;
out vec4 outputColor;
void main()
{
	vec4 texColor=texture(tex1, outTextCord);
	 bvec3 toDiscard = lessThan(texColor.xyz ,
                                 vec3(0.1,0.1,0.1) );

if( all(toDiscard) )
    discard;
	
	
	outputColor=vec4(texColor.xyz*vec3(1.5,1.3,0.3),1.0);//
	

}
