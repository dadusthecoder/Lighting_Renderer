#shader Vertex
#version 440 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_matp;  
uniform mat4 u_matv;  
uniform float u_Time;

layout(location = 0) out vec3 v_WorldPos;

void main()
{
    vec3 displaced = a_Position;
    v_WorldPos = displaced;
    gl_Position = u_matp * u_matv * vec4(displaced, 1.0);
}

#shader Fragment
#version 450 core

layout(location = 0) in vec3 v_WorldPos;
layout(location = 0) out vec4 FragColor;

uniform vec3 u_viewp;  

void main()
{
    vec3 baseColor = vec3(0.10, 0.10, 0.10);
    float dist = distance(u_viewp, v_WorldPos);
    float fade = clamp(1.0 - dist / 50.0, 0.0, 1.0);
    FragColor = vec4(baseColor,fade);
}

