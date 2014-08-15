#version 330
in vec2 outTextCord;
 uniform sampler2D tex1;
 uniform sampler2D texOrigin;
 uniform vec2 screenSize;
 out vec4 outputColor;

 //for gaussian blur:
 float PixOffset[11] = float[](0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0);
 float Weight[11]=float[](0.103686,0.100287,0.0907433,0.0768126,0.0608271,0.0450618,0.0312297,0.0202476,0.0122808,0.00696828,0.0036989);
void main()
{

	//pass 3:
	float dx = 1.0 / screenSize.x;
  vec4 sum = texture(tex1, outTextCord)* Weight[0];
  vec4 diffuse=texture(texOrigin, outTextCord);

  for( int i = 1; i < 10; i++ )
    {
    sum += texture( tex1, 
                    outTextCord + vec2(PixOffset[i],0.0) * dx ) 
                    * Weight[i];
    sum += texture( tex1, 
                    outTextCord - vec2(PixOffset[i],0.0) * dx ) 
                    * Weight[i];
    }
	
	//oColor = 1.0 - exp2(-vColor * exposure); 
	//sample.rgb / (sample.rgb + 1.0);*/
	 outputColor=sum+diffuse; //note: in comment for bloom to work
	
}
