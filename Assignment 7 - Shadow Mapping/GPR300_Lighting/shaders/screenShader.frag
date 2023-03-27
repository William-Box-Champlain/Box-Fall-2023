#version 450
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uScreenTexture;
uniform int uEffect;
uniform vec2 uOffsetDistance;

const float offset = 1.0 / 300.0;

void main()
{
	vec3 totalcolor = vec3(0.0);

	vec2 offsets[9] = vec2[](
    vec2(-offset,offset), vec2(0,offset), vec2(offset,offset),
    vec2(-offset,0), vec2(0,0), vec2(offset,0),
    vec2(-offset,-offset),vec2(0,-offset),vec2(offset,-offset)
	);

	mat3 kernal;

	mat3 identity = mat3(
    0.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 0.0
	);

	mat3 gaussianBlur = mat3(
	0.065f, 0.125f, 0.065f, //first column
	0.125f, 0.25f,  0.125f, //second column
	0.065f, 0.125f, 0.065f  //third column
	);

	mat3 sharpen = mat3(
	 0.0,-1.0, 0.0,
	-1.0, 5.0,-1.0,
	 0.0,-1.0, 0.0
	);

	mat3 edgeDetect = mat3(
	1.0, 1.0, 1.0,
	1.0,-8.0, 1.0,
	1.0, 1.0, 1.0
	);

	if(uEffect == 0)
	{
		kernal = identity;
	}
	if(uEffect == 1)
	{
		kernal = gaussianBlur;
	}
	if(uEffect == 2)
	{
		kernal = sharpen;
	}
	if(uEffect == 3)
	{
		kernal = edgeDetect;
	}

	for(int i = 0; i < 9 ; i++)
	{
		vec3 color = texture(uScreenTexture,TexCoords + (offsets[i] * uOffsetDistance)).rgb;
		//vec3 color = texture(uScreenTexture,TexCoords).rgb;

		int col = i%3;
		int row = i/3;

		color *= kernal[col][row];

		totalcolor += color;
	}
	FragColor = vec4(totalcolor,1.0);
}