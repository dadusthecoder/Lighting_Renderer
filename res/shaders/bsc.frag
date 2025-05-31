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
uniform sampler2D u_Texture1;       // Specular map
uniform sampler2D u_Texture2;       // Normal map
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
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular;

    float SpecularMap = texture(u_Texture1 , TextCoord).r;
    vec3 ambient = 0.1 * material.ambient;

    vec4 lighting = vec4(u_color * (ambient + diffuse + SpecularMap*specular), 1.0);
    vec4 DiffuseMap = texture(u_Texture0, TextCoord);

    if (u_usec) {
        color = lighting;
    } else {
        color = DiffuseMap * lighting;
    }
}


