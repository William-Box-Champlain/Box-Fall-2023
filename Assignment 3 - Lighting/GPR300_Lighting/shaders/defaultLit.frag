#version 450                          
out vec4 FragColor;

in vec3 WorldPosition;
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
    vec3 color;

    vec3 position;

    vec3 direction;

    float intensity;
    float radius;

    float minAngle;
    float maxAngle;
};

struct PointLight
{
    vec3 position;

    vec3 color;

    float intensity;

    float radius;

};

struct DirLight
{
    vec3 direction;

    vec3 color;

    float intensity;
};

vec3 CalculateDirectionalLighting(DirLight light, vec3 normal, vec3 cameraDirection);

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragmentPosition, vec3 cameraDirection);

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragmentPosition, vec3 cameraDirection, vec3 cameraPosition);

uniform vec3 uEyePosition;

uniform Material material;

uniform DirLight dirLight;
uniform SpotLight spotLight;

#define MAX_NUMBER_OF_POINT_LIGHTS 10
uniform int numberOfPointLights;
uniform PointLight pointLights[MAX_NUMBER_OF_POINT_LIGHTS];

void main(){
    vec3 normal = normalize(Normal);
    vec3 viewDirection = normalize(uEyePosition - WorldPosition);

    vec3 totalLight = CalculateDirectionalLighting(dirLight,normal,viewDirection);

    totalLight += CalculateSpotLight(spotLight, normal, WorldPosition,viewDirection,uEyePosition);

    for(int i = 0; i < numberOfPointLights; i++) totalLight += CalculatePointLight(pointLights[i],normal,WorldPosition,viewDirection);

    FragColor = vec4(abs(Normal),1.0f);
}

vec3 CalculateDirectionalLighting(DirLight light, vec3 normal, vec3 cameraDirection)
{
    vec3 ambient = material.ambient * light.intensity;

    vec3 lightDirection = normalize(-light.direction);
    vec3 diffuse = material.diffuse * max(dot(lightDirection,normal),0.0f) * light.intensity;

    vec3 halfVector = normalize((light.direction + cameraDirection));
    vec3 specular = material.specular * max(dot(normal,halfVector),0.0f) * material.shininess * light.intensity;

    return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragmentPosition, vec3 cameraDirection)
{
    vec3 temp;

    float dist = distance(fragmentPosition,light.position);
    float attenuatedIntensity = pow(clamp((1 - pow((dist/light.radius),4.0f)),0.0f,1.0f),2.0f);

    vec3 ambient = material.ambient * attenuatedIntensity;

    vec3 lightDirection = normalize(fragmentPosition - light.position);
    vec3 diffuse = material.diffuse * max(dot(lightDirection,normal),0.0f) * attenuatedIntensity;

    vec3 halfVector = normalize((lightDirection + cameraDirection));
    vec3 specular = material.specular * max(dot(normal,halfVector),0.0f) * material.shininess * attenuatedIntensity;

    return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragmentPosition, vec3 cameraDirection, vec3 cameraPosition)
{
    vec3 temp;

    vec3 lightDir = normalize(fragmentPosition - light.position);
    float theta = dot(lightDir,normalize(-light.direction));

    if(theta > cos(light.maxAngle))
    {
        
    }

    return vec3(0.0f);
}