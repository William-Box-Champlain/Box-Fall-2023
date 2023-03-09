#version 450                          
layout (location = 0) in vec3 vPos;  
layout (location = 1) in vec3 vNormal;
layout (location = 3) in vec3 vTangent;
layout (location = 2) in vec2 vUv;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection; 

out vec2 Uv;
out mat3 TBN;

out struct WorldVertex
{
    vec3 mWorldPosition;
    vec3 mWorldNormal;
} vertex;

void main()
{   
    //calculate bitangent
    vec3 T = normalize(vec3(_Model * vec4(vTangent,0.0)));
    vec3 N = normalize(vec3(_Model * vec4(vNormal,0.0)));
    vec3 B = cross(N,T);
    //create TBN matrix
    TBN = mat3(T,B,N);

    vertex.mWorldPosition = vec3(_Model * vec4(vPos,1));
    vertex.mWorldNormal = vNormal;
    Uv = vUv;
    gl_Position = _Projection * _View * _Model * vec4(vPos,1);
}