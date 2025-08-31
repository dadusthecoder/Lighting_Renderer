#shader Vertex
#version 450 core

// Vertex attributes
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 textcoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

// Outputs to fragment shader
layout(location = 0) out vec2 TexCoord;
layout(location = 1) out vec3 FragPos;
layout(location = 2) out vec3 Normal;
layout(location = 3) out vec3 Tangent;
layout(location = 4) out vec3 Bitangent;
layout(location = 5) out vec3 ViewPos;
layout(location = 6) out vec4 LightSapceFragPos;


// Uniform matrices
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

uniform mat4 u_lightprojection;
uniform mat4 u_lightview;

uniform mat3 u_normalMatrix; 
uniform vec3 u_viewPos;


void main() {

    FragPos = vec3(u_model * vec4(pos, 1.0));
    
    Normal = normalize(u_normalMatrix * normal);
    Tangent = normalize(u_normalMatrix * tangent);
    Bitangent = normalize(u_normalMatrix * bitangent);
    
    TexCoord = textcoord;
    ViewPos = u_viewPos;
    
    LightSapceFragPos = u_lightprojection * u_lightview * u_model * vec4(pos, 1.0);
    gl_Position = u_projection * u_view * vec4(FragPos, 1.0);
}

#shader Fragment
#version 450 core

layout(location = 0) in vec2 TexCoord;
layout(location = 1) in vec3 FragPos;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;
layout(location = 4) in vec3 Bitangent;
layout(location = 5) in vec3 ViewPos;
layout(location = 6) in vec4 LightSapceFragPos;


layout(location = 0) out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

// Material structure
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float normalStrength;
    bool hasNormalMap;
    bool hasSpecularMap;
};

// Uniforms
uniform sampler2D u_diffuseMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_specularMap;
uniform sampler2D u_depthMap;   

uniform Material u_material;
uniform Light u_light;
uniform bool u_useColor;
uniform vec3 u_color;

mat3 getTBN() {
    vec3 T = normalize(Tangent);
    vec3 B = normalize(Bitangent);
    vec3 N = normalize(Normal);
    
    T = normalize(T - dot(T, N) * N);
    
    return mat3(T, B, N);
}

vec3 getNormalFromMap() {
    if (!u_material.hasNormalMap) {
        return normalize(Normal);
    }
    
    vec3 tangentNormal = texture(u_normalMap, TexCoord).xyz * 2.0 - 1.0;
    tangentNormal.xy *= u_material.normalStrength;
    
    mat3 TBN = getTBN();
    return normalize(TBN * tangentNormal);
}

float calculateAttenuation(float distance) {
    float temp = (u_light.constant + u_light.linear * distance + u_light.quadratic * distance * distance);
    return 1.0 ;
}

float calculateShadow(vec4 lightspcefragpos , vec3 normal , vec3 lightdir)
{
    vec3 Projcoord = lightspcefragpos.xyz / lightspcefragpos.w ; //perspective devide 
    Projcoord = Projcoord*0.5 + 0.5;

    float bias = max(0.025f * (1-dot(normal , lightdir)),0.0005f) ;
    vec2 texalsize = 1.0f / textureSize(u_depthMap , 0);
    float x , y ;
    float shadow = 0.0 ;
    float sampleRad = 1.5 ;
    //anti aliasing using pcf
    for(y = -sampleRad; y <= sampleRad ; y++ ){

         for( x = -sampleRad ; x <= sampleRad ; x++ ){
                 float currentdepth = Projcoord.z ;
                 float closestdpeth = texture(u_depthMap , Projcoord.xy + vec2(x,y)*texalsize).r;
                 if(currentdepth  > closestdpeth){
                     shadow  += 1.0 ;
                 }
         }
    }
    if(Projcoord.z > 1.0) shadow = 0.0;
    return shadow/ 16;
}

// Blinn-Phong lighting calculation
vec3 calculateBlinnPhong(vec3 normal, vec3 lightDir, vec3 viewDir, float attenuation) {
    vec3 ambient = u_material.ambient * u_light.color * 0.15;
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * u_material.diffuse * u_light.color;
    
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), u_material.shininess);
    
    float specularMap = u_material.hasSpecularMap ? 
                       texture(u_specularMap, TexCoord).r : 1.0;
    
    vec3 specular = spec * u_material.specular * u_light.color * specularMap;
    
    diffuse *= attenuation;
    specular *= attenuation;
    
    float shadow = calculateShadow(LightSapceFragPos ,normal,lightDir );
   // return (ambient + (shadow)) * (diffuse + specular); --!WRONG
    return ambient + (1 - shadow)* diffuse + specular;

}

void main() {
    vec3 normal = getNormalFromMap();
    
    vec3 lightDir = normalize(u_light.position - FragPos);
    vec3 viewDir = normalize(ViewPos - FragPos);

    float distance = length(u_light.position - FragPos);
    float attenuation = calculateAttenuation(distance);
 
    vec3 lighting = calculateBlinnPhong(normal, lightDir, viewDir, attenuation);
    lighting *= u_light.intensity;
    
    vec4 diffuseColor = texture(u_diffuseMap, TexCoord);
    
    if (u_useColor) {
        FragColor = vec4(u_color * lighting, 1.0);
    } else {
        FragColor = vec4(diffuseColor.rgb * lighting, diffuseColor.a);
    }
    
    // Optional: Tone mapping for better color representation
    FragColor.rgb = FragColor.rgb / (FragColor.rgb + vec3(1.0));
    
    // Optional: Gamma correction
    //FragColor.rgb = pow(FragColor.rgb, vec3(1.0/2.2));
}