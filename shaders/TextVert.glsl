#version 330

//THIS IS BASICALLY THE SAME AS QUAD VERT, WITH A SLIGHT MODIFICATION TO MAP THE U,V COORDINATE TO THE CORRECT CELL
//ON THE TEXTURE IMAGE

uniform mat4 MV;//this one is to position the quad on 2d screen
layout(location = 0) in vec3 position;
layout (location=2) in vec2 texCord;
out vec2 outTextCord;

//THIS MAPPING IS CALCULATE FROM THE HCLIENT AND SENT TO VERTEX SHADER AS A UNIFORM PER LETTER
uniform vec2 texOffset;
void main()
{
	gl_Position = MV*vec4(position.x,position.y,position.z,1.0);//
	
	//linear interpolation to map u (and similar to v) from [0->1] to [xOffxet -> xOffset+1/8] (because a cell size is 1/8)
	outTextCord=(vec2(1.0)-texCord)*texOffset+texCord*(texOffset+vec2(0.125));


/*	vec2(
	(1.0-texCord.x)*texOffset.x+texCord.x*(texOffset.x+0.125),
	(1.0-texCord.y)*texOffset.y+texCord.y*(texOffset.y+0.125)
	); 
	*/
	//pass the mapped tex coord
}