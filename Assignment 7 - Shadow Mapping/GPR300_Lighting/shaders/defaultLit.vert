#version 450                          
layout (location = 0) in vec3 vPos;  
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vTangent;
layout (location = 3) in vec2 vUv;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection; 
uniform mat4 _LightSpaceMatrix;

out vert
{
    vec3 mWorldPosition;
    vec3 mNormal;
    vec2 mUv;
    mat3 mTBN;
    vec4 mlightSpacePosition;
} vertOut;

void main()
{   
    //calculate bitangent
    vec3 T = normalize(vec3(_Model * vec4(vTangent,0.0)));
    vec3 N = normalize(vec3(_Model * vec4(vNormal,0.0)));
    vec3 B = normalize(cross(N,T));
    //create TBN matrix
    vertOut.mTBN = mat3(T,B,N);

    vertOut.mWorldPosition = vec3(_Model * vec4(vPos,1.0));
    vertOut.mNormal = vNormal;
    vertOut.mUv = vUv;
    vertOut.mlightSpacePosition = _LightSpaceMatrix * vec4(vec3(_Model * vec4(vPos,1.0)),1.0);
    gl_Position = _Projection * _View * vec4(vertOut.mWorldPosition,1.0);
}