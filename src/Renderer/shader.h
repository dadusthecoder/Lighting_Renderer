#pragma once


#include <cstdlib>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<unordered_map>

class camera;
struct Material;

enum ShaderType {
    DEPTHSHADER ,
    COLORSHADER
};

struct shadersource {
    std::string vertexSource;
    std::string fragmentSource;
};

class shader {
private:
    std::string m_filepath;
    GLuint m_RenderID;
    ShaderType m_type  = ShaderType::COLORSHADER;
    mutable std::unordered_map<std::string, int> m_uniformLocationCache;

    // Helper methods
    shadersource parseShader(const std::string& filepath);
    unsigned int compileShader(unsigned int type, const std::string& source);
    unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
    void cacheUniformLocations();
    int getUniformLocation(const std::string& name) const;

public:
    // Constructor and destructor
    explicit shader(const std::string& filepath);
    explicit shader(const std::string& filepath , ShaderType type);

    ~shader();

    // Delete copy constructor and assignment operator to prevent issues
    shader(const shader&) = delete;
    shader& operator=(const shader&) = delete;

    // Move constructor and assignment operator
    shader(shader&& other) noexcept
        : m_filepath(std::move(other.m_filepath))
        , m_RenderID(other.m_RenderID)
        , m_uniformLocationCache(std::move(other.m_uniformLocationCache))
    {
        other.m_RenderID = 0;
    }

    shader& operator=(shader&& other) noexcept {
        if (this != &other) {
            if (m_RenderID != 0) {
                glDeleteProgram(m_RenderID);
            }

            m_filepath = std::move(other.m_filepath);
            m_RenderID = other.m_RenderID;
            m_uniformLocationCache = std::move(other.m_uniformLocationCache);

            other.m_RenderID = 0;
        }
        return *this;
    }

    // Modern shader binding methods
    void use() const;
    void useWithCamera(camera& Camera);
    void unuse() const;

    // Modern uniform setting methods
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

    // High-level uniform setting methods
    void setMaterial(const Material& material) const;
    void setLight(const glm::vec3& position, const glm::vec3& color, float intensity = 1.0f,
        float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f) const;
    void setTextures(int diffuseUnit = 0, int normalUnit = 1, int specularUnit = 2 , int depthunit = 3 ) const;

    // Backward compatibility methods (deprecated - use modern equivalents)
    [[deprecated("Use setInt() instead")]]
    unsigned int setuniform1i(const std::string& name, int value) const;

    [[deprecated("Use setFloat() instead")]]
    unsigned int setuniform1f(const std::string& name, float value) const;

    [[deprecated("Use setVec4() instead")]]
    unsigned int setuniform4f(const std::string& name, float v0, float v1, float v2, float v3) const;

    [[deprecated("Use setMat4() instead")]]
    unsigned int setuniform4matf(const std::string& name, const glm::mat4& matrix) const;

    [[deprecated("Use setVec3() instead")]]
    unsigned int setuniformvec3(const std::string& name, const glm::vec3& vec) const;

    [[deprecated("Use use() instead")]]
    void Bind() const;

    [[deprecated("Use useWithCamera() instead")]]
    void Bind_UseCamera(camera& Camera);

    [[deprecated("Use unuse() instead")]]
    void Unbind() const;

    // Utility methods
    GLuint getID() const;
    ShaderType getType() const ;
    const std::string& getPath() const;
    bool isValid() const;
    void reload();
    void printActiveUniforms() const;

    // RAII helper for automatic shader binding/unbinding
    class ScopedBind {
    public:
        explicit ScopedBind(const shader& shaderObj);
        ~ScopedBind();

        // Prevent copying
        ScopedBind(const ScopedBind&) = delete;
        ScopedBind& operator=(const ScopedBind&) = delete;

    private:
        const shader& m_shader;
    };
};