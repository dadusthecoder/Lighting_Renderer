#pragma once
#include"renderer.h"

struct vertex {
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 textcoord;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};
class Mesh {
private:

    glm::mat4 _transform;
    Material _material;
    std::vector<std::shared_ptr<Texture>> Textures;
    std::vector<unsigned int>m_indices;
    GLuint m_vao, m_vbo, m_ibo;

public:
    Mesh(const std::vector<vertex>& data, const std::vector<unsigned int>& indices,const Material& material , std::vector<std::shared_ptr<Texture>> textures);
    void cleanUp();
    void render( const shader& Shader) ;
    void setTransform(glm::mat4 transform);
    glm::mat4 getTransform();
};