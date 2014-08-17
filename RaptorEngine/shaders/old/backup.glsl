#version 330
 in vec2 outTextCord;
 uniform sampler2D gPosTex;
 uniform sampler2D gNormTex;
 uniform sampler2D gDiffTex;
 uniform sampler2D randomTex;

 //3 samplers to read from g buffer

 out vec4 outputColor;
 /*
 void main()
 {
 //retrieve data from G buffer:
 vec3 pos= vec3(texture(gPosTex,outTextCord));
  vec4 norm= vec4(texture(gNormTex,outTextCord));
   vec3 diff= vec3(texture(gDiffTex,outTextCord));
	//quick diffuse model:

	vec3 s = normalize(vec3(1.0,7.0,1.0) - pos);
  float sDotN = max( dot(s,norm), 0.0 );
  vec3 diffuse = vec3(0.5,0.5,0.5)* diff * sDotN;
  outputColor=vec4(diffuse,1.0);

 }
 */


 //uniform sampler2D Tex0, Tex1, Tex2;
 vec2 screenSize=vec2(600,600);
float random_size = 64.0; // size of random texture
float zFar=60.0;
float SampleRad=1.0;
float Intensity=1.0;
float Scale=1.0;
float Bias=0.0;

	//tex0: random, tex1: normal, tex2: position
float calcAO(vec2 uv, vec2 coord, vec3 pos, vec3 norm)
{
	vec3 diff = texture(gPosTex, uv + coord).xyz - pos;
	vec3 v = normalize(diff);
	float d = length(diff) * Scale;
	return max(0.0, dot(norm, v) - Bias) * (1.0 / (1.0 + d)) * Intensity;
}

void main()
{
	vec2 rand;
	vec3 n;

	vec2 vec[4];
	vec[0] = vec2(1, 0);
	vec[1] = vec2(-1, 0);
	vec[2] = vec2(0, 1);
	vec[3] = vec2(0, -1);
    vec2 uv = outTextCord;
	vec2 randomMap = texture(randomTex, screenSize* uv / random_size).xy;
	vec3 normalMap = texture(gNormTex, uv).xyz;
	vec3 pos = texture(gPosTex, uv).xyz;
	n = normalize(normalMap * 2.0 - 1.0);
	rand = normalize(randomMap * 2.0 - 1.0);
	
	float ao = 0.0;
 	float rad = SampleRad / pos.z;

	int iterations =1; //int(mix(4.0, 1.0, -pos.z/zFar));
	for (int i = 0; i < iterations; ++i) {
		vec2 coord1 = reflect(vec[i], rand) * rad;
		vec2 coord2 = vec2(coord1.x * 0.707 - coord1.y * 0.707, coord1.x * 0.707 + coord1.y * 0.707);
  		
		ao += calcAO(uv, coord1 * 0.25, pos, n);
	//	ao += calcAO(uv, coord2 * 0.5, pos, n);
	//	ao += calcAO(uv, coord1 * 0.75, pos, n);
	//	ao += calcAO(uv, coord2, pos, n);	
	}
		 
	ao /= float(iterations) * 1.0;
	
	outputColor = vec4(1-ao);
}
