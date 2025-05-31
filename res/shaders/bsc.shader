#shader Vertex
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




#shader Fragment
#version 450 core

layout(location = 0) in vec2 TextCoord;
layout(location = 1) in vec3 FragPos;
layout(location = 2) in mat3 TBN; // TBN matrix from vertex shader

layout(location = 0) out vec4 color;

// Material definition
struct Material {
    vec3 specular;
    vec3 ambient;
    vec3 diffuse;
    float shininess;
};

// Uniforms
uniform sampler2D u_Texture0;       // Diffuse map
//uniform sampler2D u_Texture1;       // Specular map
uniform sampler2D u_Texture1;       // Normal map
uniform Material material;
uniform bool u_usec;
uniform vec3 u_color;
uniform vec3 lightp;
uniform vec3 u_viewp;

void main() {
    // Sample normal map and convert from [0,1] to [-1,1]
    vec3 normalMap = texture(u_Texture1, TextCoord).rgb;
    vec3 normal = normalize(normalMap * 2.0 - 1.0); // Tangent space normal
    normal = normalize(TBN * normal);               // World space normal

    vec3 lightDir = normalize(lightp - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse;

    vec3 viewDir = normalize(u_viewp - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),material.shininess);
    vec3 specular = spec * material.specular;

    //float SpecularMap = texture(u_Texture1, TextCoord).r;
    vec3 ambient = 0.1 * material.ambient;

    vec4 lighting = vec4(u_color * (ambient + diffuse), 1.0);
    vec4 DiffuseMap = texture(u_Texture0, TextCoord);

    if (u_usec) {
        color = lighting;
    } else {
        color = DiffuseMap * lighting;
    }
}



