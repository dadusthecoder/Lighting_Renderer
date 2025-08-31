#include "renderer.h"
#include "shader.h"
#include "camera.h"
#include <unordered_map>

shader::shader(const std::string& filepath)
    : m_filepath(filepath), m_RenderID(0)
{
    shadersource source = parseShader(filepath);
    m_RenderID = createShader(source.vertexSource, source.fragmentSource);

    // Cache uniform locations for better performance
    cacheUniformLocations();

    LOG(LogLevel::_IMP, "Shader loaded from: " + filepath + " | ID: " + std::to_string(m_RenderID));
}

shader::shader(const std::string& filepath ,ShaderType type )
    : m_filepath(filepath), m_RenderID(0) ,m_type(type)
{
    shadersource source = parseShader(filepath);
    m_RenderID = createShader(source.vertexSource, source.fragmentSource);

    // Cache uniform locations for better performance
    cacheUniformLocations();

    LOG(LogLevel::_IMP, "Shader loaded from: " + filepath + " | ID: " + std::to_string(m_RenderID));
}

shader::~shader()
{
    glDeleteProgram(m_RenderID);
    LOG(LogLevel::_IMP, "Shader deleted | ID: " + std::to_string(m_RenderID));
}

void shader::use() const
{
    glUseProgram(m_RenderID);
}

void shader::useWithCamera(camera& Camera)
{
    glUseProgram(m_RenderID);
    setMat4("u_view", Camera.GetViewMatrix());
    setMat4("u_projection", Camera.GetProjectionMatrix());
    setVec3("u_viewPos", Camera.GetCameraPos());
}

void shader::unuse() const
{
    glUseProgram(0);
}

shadersource shader::parseShader(const std::string& filepath)
{
    m_filepath = filepath;
    std::ifstream stream(filepath);

    if (!stream.is_open()) {
        LOG(LogLevel::_ERROR, "Failed to open shader file: " + filepath);
        return { "", "" };
    }

    std::string line;
    std::stringstream ss[2];

    enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("Vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("Fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else if (type != ShaderType::NONE) {
            ss[static_cast<int>(type)] << line << "\n";
        }
    }

    return { ss[0].str(), ss[1].str() };
}

unsigned int shader::compileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // Check compilation status
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> errorLog(length);
        glGetShaderInfoLog(id, length, &length, errorLog.data());

        std::string shaderTypeStr = (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment";
        LOG(LogLevel::_ERROR, shaderTypeStr + " shader compilation error: " + std::string(errorLog.data()));

        glDeleteShader(id);
        return 0;
    }
    else {
        std::string shaderTypeStr = (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment";
        LOG(LogLevel::DEBUG, shaderTypeStr + " shader compiled successfully.");
    }

    return id;
}

unsigned int shader::createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    if (vs == 0 || fs == 0) {
        LOG(LogLevel::_ERROR, "Shader compilation failed, cannot create program");
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        glDeleteProgram(program);
        return 0;
    }

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // Check linking status
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> errorLog(length);
        glGetProgramInfoLog(program, length, &length, errorLog.data());

        LOG(LogLevel::_ERROR, "Shader program linking error: " + std::string(errorLog.data()));

        glDeleteShader(vs);
        glDeleteShader(fs);
        glDeleteProgram(program);
        return 0;
    }

    // Validate program
    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (!success) {
        LOG(LogLevel::_WARNING, "Shader program validation failed");
    }

    // Clean up shaders (they're now linked into the program)
    glDeleteShader(vs);
    glDeleteShader(fs);

    LOG(LogLevel::_IMP, "Shader program linked successfully | Program ID: " + std::to_string(program));
    return program;
}

void shader::cacheUniformLocations()
{
    if (m_RenderID == 0) return;

    // Common uniforms that we'll cache for performance
    std::vector<std::string> commonUniforms = {
        // Matrices
        "u_model", "u_view", "u_projection", "u_normalMatrix",

        // Material properties
        "u_material.ambient", "u_material.diffuse", "u_material.specular",
        "u_material.shininess", "u_material.normalStrength", "u_material.hasNormalMap",
        "u_material.hasSpecularMap",

        // Light properties
        "u_light.position", "u_light.color", "u_light.intensity",
        "u_light.constant", "u_light.linear", "u_light.quadratic",

        // Textures
        "u_diffuseMap", "u_normalMap", "u_specularMap",

        // Other
        "u_viewPos", "u_useColor", "u_color"
    };

    for (const auto& uniform : commonUniforms) {
        int location = glGetUniformLocation(m_RenderID, uniform.c_str());
        if (location != -1) {
            m_uniformLocationCache[uniform] = location;
        }
    }

    LOG(LogLevel::DEBUG, "Cached " + std::to_string(m_uniformLocationCache.size()) + " uniform locations");
}

int shader::getUniformLocation(const std::string& name) const
{
    // Check cache first
    auto it = m_uniformLocationCache.find(name);
    if (it != m_uniformLocationCache.end()) {
        return it->second;
    }

    // If not in cache, get location and cache it
    int location = glGetUniformLocation(m_RenderID, name.c_str());
    if (location == -1) {
        LOG(LogLevel::_WARNING, "Uniform '" + name + "' not found in shader");
    }
    else {
        // Cache the location for future use (const_cast is safe here)
        const_cast<shader*>(this)->m_uniformLocationCache[name] = location;
    }

    return location;
}

// ===== Improved Uniform Setting Methods =====

void shader::setBool(const std::string& name, bool value) const
{
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform1i(loc, static_cast<int>(value));
}

void shader::setInt(const std::string& name, int value) const
{
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform1i(loc, value);
}

void shader::setFloat(const std::string& name, float value) const
{
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform1f(loc, value);
}

void shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform2fv(loc, 1, &value[0]);
}

void shader::setVec2(const std::string& name, float x, float y) const
{
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform2f(loc, x, y);
}

void shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform3fv(loc, 1, &value[0]);
}

void shader::setVec3(const std::string& name, float x, float y, float z) const
{
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform3f(loc, x, y, z);
}

void shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform4fv(loc, 1, &value[0]);
}

void shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
    int loc = getUniformLocation(name);
    if (loc != -1) glUniform4f(loc, x, y, z, w);
}

void shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    int loc = getUniformLocation(name);
    if (loc != -1) glUniformMatrix2fv(loc, 1, GL_FALSE, &mat[0][0]);
}

void shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    int loc = getUniformLocation(name);
    if (loc != -1) glUniformMatrix3fv(loc, 1, GL_FALSE, &mat[0][0]);
}

void shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    int loc = getUniformLocation(name);
    if (loc != -1) glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
}

void shader::setMaterial(const Material& material) const
{
    setVec3("u_material.ambient", material.ambient);
    setVec3("u_material.diffuse", material.diffuse);
    setVec3("u_material.specular", material.specular);
    setFloat("u_material.shininess", material.shininess);
    setFloat("u_material.normalStrength", material.normalStrength);
    setBool("u_material.hasNormalMap", material.hasNormalMap);
    setBool("u_material.hasSpecularMap", material.hasSpecularMap);
}

void shader::setLight(const glm::vec3& position, const glm::vec3& color, float intensity,
    float constant, float linear, float quadratic) const
{
    setVec3("u_light.position", position);
    setVec3("u_light.color", color);
    setFloat("u_light.intensity", intensity);
    setFloat("u_light.constant", constant);
    setFloat("u_light.linear", linear);
    setFloat("u_light.quadratic", quadratic);
}

void shader::setTextures(int diffuseUnit, int normalUnit, int specularUnit , int depthunit ) const
{
    setInt("u_diffuseMap", diffuseUnit);
    setInt("u_normalMap", normalUnit);
    setInt("u_specularMap", specularUnit);
    setInt("u_depthMap", depthunit);
}

// ===== Backward Compatibility Methods =====

unsigned int shader::setuniform1i(const std::string& name, int value) const
{
    setInt(name, value);
    return getUniformLocation(name);
}

unsigned int shader::setuniform1f(const std::string& name, float value) const
{
    setFloat(name, value);
    return getUniformLocation(name);
}

unsigned int shader::setuniform4f(const std::string& name, float v0, float v1, float v2, float v3) const
{
    setVec4(name, v0, v1, v2, v3);
    return getUniformLocation(name);
}

unsigned int shader::setuniform4matf(const std::string& name, const glm::mat4& matrix) const
{
    setMat4(name, matrix);
    return getUniformLocation(name);
}

unsigned int shader::setuniformvec3(const std::string& name, const glm::vec3& vec) const
{
    setVec3(name, vec);
    return getUniformLocation(name);
}

void shader::Bind() const { use(); }
void shader::Bind_UseCamera(camera& Camera) { useWithCamera(Camera); }
void shader::Unbind() const { unuse(); }

// ===== Utility Methods =====

GLuint shader::getID() const {
    return m_RenderID;
}

const std::string& shader::getPath() const {
    return m_filepath;
}

bool shader::isValid() const {
    return m_RenderID != 0;
}

void shader::reload() {
    if (m_RenderID != 0) {
        glDeleteProgram(m_RenderID);
    }

    LOG(LogLevel::_IMP, "Reloading shader from: " + m_filepath);

    shadersource source = parseShader(m_filepath);
    m_RenderID = createShader(source.vertexSource, source.fragmentSource);

    if (m_RenderID != 0) {
        // Clear and recache uniform locations
        m_uniformLocationCache.clear();
        cacheUniformLocations();
        LOG(LogLevel::_IMP, "Shader reloaded successfully");
    }
    else {
        LOG(LogLevel::_ERROR, "Failed to reload shader");
    }
}

void shader::printActiveUniforms() const {
    if (m_RenderID == 0) return;

    GLint numUniforms;
    glGetProgramiv(m_RenderID, GL_ACTIVE_UNIFORMS, &numUniforms);

    LOG(LogLevel::_INFO, "Active uniforms (" + std::to_string(numUniforms) + "):");

    for (GLint i = 0; i < numUniforms; ++i) {
        char name[256];
        GLsizei length;
        GLint size;
        GLenum type;

        glGetActiveUniform(m_RenderID, i, sizeof(name), &length, &size, &type, name);
        int location = glGetUniformLocation(m_RenderID, name);

        LOG(LogLevel::_INFO, "  [" + std::to_string(location) + "] " + std::string(name));
    }
}

// ===== RAII Shader Binder for Exception Safety =====
shader::ScopedBind::ScopedBind(const shader& shaderObj) : m_shader(shaderObj) {
    m_shader.use();
}

shader::ScopedBind::~ScopedBind() {
    m_shader.unuse();
}

ShaderType shader::getType() const
{
    return m_type;
}