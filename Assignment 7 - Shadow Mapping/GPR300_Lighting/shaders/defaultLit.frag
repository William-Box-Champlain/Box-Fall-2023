#version 450                          
out vec4 FragColor;

in vert
{
    vec3 mWorldPosition;
    vec3 mNormal;
    vec2 mUv;
    mat3 mTBN;
    vec4 mlightSpacePosition;
} vertIn;


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
    float mFallOff;
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

float ShadowCalculations(vec4 lightSpacePosition, vec3 normal, vec3 toLight);

vec3 CalculateDiffuse(vec3 materialDiffuse, vec3 normal, vec3 lightDirection);

vec3 CalculateSpecular(vec3 materialSpecular, vec3 normal, vec3 halfVector, float materialShininess);

vec3 CalculateDirectionalLighting(DirLight light, vec3 normal, vec3 cameraDirection);

vec3 CalculatePointLight(PointLight light, float shadow, vec3 normal, vec3 fragmentPosition, vec3 cameraDirection);

vec3 CalculateSpotLight(SpotLight light, float shadow, vec3 normal, vec3 fragmentPosition, vec3 cameraDirection, vec3 cameraPosition);

uniform vec3 uEyePosition;

uniform Material material;

uniform DirLight dirLight;
uniform SpotLight spotLight;

#define MAX_NUMBER_OF_POINT_LIGHTS 10
uniform int numberOfPointLights;
uniform PointLight pointLights[MAX_NUMBER_OF_POINT_LIGHTS];

uniform sampler2D uTexture;
uniform sampler2D uNoise;
uniform sampler2D uNormalMap;

uniform sampler2D uShadowMap;

uniform float uTime;
uniform float uSampleSize;

uniform float uMinBias;
uniform float uMaxBias;

void main(){
    vec3 normal = normalize(vertIn.mNormal);

    vec3 textureNormal = texture(uNormalMap,vertIn.mUv).rgb;
    textureNormal = (textureNormal*2.0f) - 1.0f;
    textureNormal = normalize(vertIn.mTBN * textureNormal);

    vec3 viewDirection = normalize(uEyePosition - vertIn.mWorldPosition);

    

    vec3 totalLight = vec3(0.0f);

    totalLight += CalculateDirectionalLighting(dirLight,textureNormal,viewDirection);

    //totalLight += CalculateSpotLight(spotLight, textureNormal, vertIn.mWorldPosition,viewDirection,uEyePosition);

    //for(int i = 0; i < numberOfPointLights; i++) totalLight += CalculatePointLight(pointLights[i],textureNormal,vertIn.mWorldPosition,viewDirection);

    //UV Noise stuff
    vec2 scrollingUV = vec2(1.0f,1.0f);
    vec2 noise = texture(uNoise,scrollingUV).rr * uSampleSize;
    vec4 color = texture(uTexture,vertIn.mUv+noise);

    FragColor = color * vec4(totalLight,1.0f);
}

float ShadowCalculations(vec4 lightSpacePosition, vec3 normal, vec3 toLight)
{
    float shadow = 0.0;
    
    //perspective divide
    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;

    //transform to 0 to 1 range
    projCoords = projCoords * 0.5 + 0.5;

    //get closest depth value from light's perspective
    float closestDepth = texture(uShadowMap,projCoords.xy).r;

    //get depth of fragment from light's perspective
    float currentDepth = projCoords.z;

    //bias calculation
    float bias = max(uMaxBias * (1.0 - dot(normal,toLight)),uMinBias);
    vec2 texelSize = 1.0 / textureSize(uShadowMap,0);

    //check if current depth is in the shadow
    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            float pcfDepth = texture(uShadowMap,projCoords.xy + vec2(x,y) * texelSize).r;
            shadow += (currentDepth - bias) > pcfDepth ? 1.0:0.0;
        }
    }
    shadow /= 9;
    return shadow;
};

vec3 CalculateDirectionalLighting(DirLight light, vec3 normal, vec3 cameraDirection)
{
    vec3 ambient = material.mAmbient * light.mIntensity;

    vec3 lightDirection = normalize(light.mDirection);
    vec3 diffuse = CalculateDiffuse(material.mDiffuse,normal,-lightDirection);

    float shadow = ShadowCalculations(vertIn.mlightSpacePosition,-lightDirection,normal);

    vec3 halfVector = normalize((-light.mDirection + cameraDirection));
    vec3 specular = CalculateSpecular(material.mSpecular,normal,halfVector,material.mShininess) * light.mIntensity;

    return light.mColor * (ambient + (1.0-shadow) * (diffuse + specular));
}

vec3 CalculatePointLight(PointLight light, float shadow, vec3 normal, vec3 fragmentPosition, vec3 cameraDirection)
{
    vec3 temp;

    float dist = distance(fragmentPosition,light.mPosition);
    float attenuatedIntensity = pow(clamp((1 - pow((dist/light.mRadius),4.0f)),0.0f,1.0f),2.0f);

    vec3 ambient = material.mAmbient ;

    vec3 lightDirection = -normalize(fragmentPosition - light.mPosition);
    vec3 diffuse = CalculateDiffuse(material.mDiffuse,normal,lightDirection);

    vec3 halfVector = normalize((lightDirection + cameraDirection));
    vec3 specular = CalculateSpecular(material.mSpecular,normal,halfVector,material.mShininess);

    return light.mColor * (ambient + (1.0-shadow) * (diffuse + specular)) * attenuatedIntensity;
}

vec3 CalculateSpotLight(SpotLight light, float shadow, vec3 normal, vec3 fragmentPosition, vec3 cameraDirection, vec3 cameraPosition)
{
    //calculate directionality and theta
    vec3 DirToFrag = -normalize(fragmentPosition - light.mPosition);
    //float costTheta = dot(DirToFrag,normalize(-light.mDirection));
    //float theta = acos(costTheta);
    float theta = dot(DirToFrag,normalize(-light.mDirection));
    //calculate angular attentuation
    float angularAttenuation = pow(clamp(((theta - light.mMinAngle)/(light.mMinAngle-light.mMaxAngle)),0.0f,1.0f),light.mFallOff);

    //calculate distance and distance attenuation
    float dist = distance(fragmentPosition,light.mPosition);
    float distanceAttenutation = pow(clamp((1-pow((dist/light.mRadius),4.0f)),0.0f,1.0f),2.0f);

    //calculate light contributions
    vec3 ambient = material.mAmbient;

    vec3 diffuse = CalculateDiffuse(material.mDiffuse,normal,DirToFrag);

    vec3 halfVector = normalize((DirToFrag + cameraDirection));
    vec3 specular = CalculateSpecular(material.mSpecular,normal,halfVector,material.mShininess);

    //combine above calculated values and return
    return light.mColor * (ambient + (1.0-shadow) * (diffuse + specular)) * distanceAttenutation * angularAttenuation;
}

vec3 CalculateDiffuse(vec3 materialDiffuse, vec3 normal, vec3 lightDirection)
{
    return material.mDiffuse * max(dot(normal,lightDirection),0.0f);
}

vec3 CalculateSpecular(vec3 materialSpecular, vec3 normal, vec3 halfVector, float materialShininess)
{
    vec3 specular = materialSpecular * pow(max(dot(normal,halfVector),0.0f),materialShininess);
    return specular;
}