#version 450                          
out vec4 FragColor;

in vec3 Normal;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct SpotLight
{
    vec3  position;
    vec3  direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
};

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


#define MAX_NUMBER_OF_POINT_LIGHTS 10
uniform int numberOfLights;
uniform PointLight pointLights[MAX_NUMBER_OF_POINT_LIGHTS];

void main(){         
    FragColor = vec4(abs(Normal),1.0f);
}
