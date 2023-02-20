#version 450                          
out vec4 FragColor;

in vec3 WorldPosition;
in vec3 Normal;

struct Material
{
    vec3 mAmbient;
    vec3 mDiffuse;
    vec3 mSpecular;
    float mShininess;
};

struct SpotLight
{
    vec3 mColor;

    vec3 mPosition;

    vec3 mDirection;

    float mIntensity;
    float mRadius;

    float mMinAngle;
    float mMaxAngle;
};

struct PointLight
{
    vec3 mPosition;

    vec3 mColor;

    float mIntensity;

    float mRadius;

};

struct DirLight
{
    vec3 mDirection;

    vec3 mColor;

    float mIntensity;
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

    FragColor = vec4(totalLight,1.0f);
}

vec3 CalculateDirectionalLighting(DirLight light, vec3 normal, vec3 cameraDirection)
{
    vec3 ambient = material.mAmbient * light.mIntensity;

    vec3 lightDirection = normalize(-light.mDirection);
    vec3 diffuse = material.mDiffuse * max(dot(normal,lightDirection),0.0f) * light.mIntensity;

    vec3 halfVector = normalize((light.mDirection + cameraDirection));
    vec3 specular = material.mSpecular * max(dot(normal,halfVector),0.0f) * material.mShininess * light.mIntensity;

    return light.mColor * (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragmentPosition, vec3 cameraDirection)
{
    vec3 temp;

    float dist = distance(fragmentPosition,light.mPosition);
    float attenuatedIntensity = pow(clamp((1 - pow((dist/light.mRadius),4.0f)),0.0f,1.0f),2.0f);

    vec3 ambient = material.mAmbient * attenuatedIntensity;

    vec3 lightDirection = normalize(fragmentPosition - light.mPosition);
    vec3 diffuse = material.mDiffuse * max(dot(normal,lightDirection),0.0f) * attenuatedIntensity;

    vec3 halfVector = normalize((lightDirection + cameraDirection));
    vec3 specular = material.mSpecular * max(dot(normal,halfVector),0.0f) * material.mShininess * attenuatedIntensity * light.mIntensity;

    return light.mColor * (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragmentPosition, vec3 cameraDirection, vec3 cameraPosition)
{
    vec3 temp;

    vec3 DirToFrag = normalize(fragmentPosition - light.mPosition);
    float costTheta = dot(DirToFrag,normalize(-light.mDirection));
    float theta = acos(costTheta);

    if(theta > light.mMaxAngle)
    {
        float angularAttenuation = pow(clamp(((theta - light.mMinAngle)/(light.mMinAngle-light.mMaxAngle)),0.0f,1.0f),2.0f);
        float dist = distance(fragmentPosition,light.mPosition);
        float distanceAttenutation = pow(clamp((1-pow((dist/light.mRadius),4.0f)),0.0f,1.0f),2.0f);

        vec3 ambient = material.mAmbient * distanceAttenutation * angularAttenuation;

        vec3 diffuse = material.mDiffuse * max(dot(normal,DirToFrag),0.0f) * angularAttenuation * distanceAttenutation;

        vec3 halfVector = normalize((DirToFrag + cameraDirection));
        vec3 specular = material.mSpecular * max(dot(normal,halfVector),0.0f) * material.mShininess * angularAttenuation * distanceAttenutation;

        return light.mColor * (ambient + diffuse + specular);
    }

    return vec3(0.0f);
}