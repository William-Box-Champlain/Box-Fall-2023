#version 450
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uScreenTexture;

void main()
{
	vec3 col = texture(uScreenTexture,TexCoords).rgb;
	//FragColor = vec4(vec3(1.0 - col),1.0);
	FragColor = vec4(col,1.0);
}