#version 450 core

// Vertex attributes from VAO
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 textcoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;      // New
layout(location = 4) in vec3 bitangent;    // New

// Outputs to fragment shader
layout(location = 0) out vec2 TextCoord;
layout(location = 1) out vec3 FragPos;
layout(location = 2) out mat3 TBN;         // New

// Uniform matrices
uniform mat4 u_matp;
uniform mat4 u_matv;
uniform mat4 u_matm;

void main() {
    vec3 T = normalize(mat3(u_matm) * tangent);
    vec3 B = normalize(mat3(u_matm) * bitangent);
    vec3 N = normalize(mat3(u_matm) * normal);
    TBN = mat3(T, B, N); // Tangent space matrix

    FragPos = vec3(u_matm * vec4(pos, 1.0));
    TextCoord = textcoord;

    gl_Position = u_matp * u_matv * vec4(FragPos, 1.0);
}



