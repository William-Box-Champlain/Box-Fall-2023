#version 450                          
layout (location = 0) in vec3 vPos;  
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUv;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

out vec3 Normal;
out vec3 WorldPosition;
out vec2 Uv;

void main()
{    
    WorldPosition = vec3(_Model * vec4(vPos,1));
    Normal = vNormal;
    Uv = vUv;
    gl_Position = _Projection * _View * _Model * vec4(vPos,1);
}
