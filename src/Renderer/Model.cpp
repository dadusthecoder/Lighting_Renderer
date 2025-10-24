#include <filesystem>
#include "Model.h"
#include "Scene.h"

// Constructor
Model::Model(const std::string &filepath) : m_ModelFilepath(filepath)
{

    m_TextureFilePath = std::filesystem::path(filepath).parent_path().string();
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        filepath,
        aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_GlobalScale |
            aiProcess_PreTransformVertices);
    // Error handling
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG(LogLevel::DEBUG, "Importing  Model  " + filepath);
        LOG(LogLevel::_ERROR, "Failed to load model: " + filepath);
        LOG(LogLevel::_ERROR, "Assimp error: " + std::string(importer.GetErrorString()));
    }
    else
    {
        // Logging import success
        LOG(LogLevel::DEBUG, "Model imported successfully: " + filepath);
        LOG(LogLevel::DEBUG, "Number of m_Meshes: " + std::to_string(scene->mNumMeshes));
        LOG(LogLevel::DEBUG, "Loading model...");
        LOG(LogLevel::DEBUG, "Processing root node...");
        processNode(scene->mRootNode, scene);
        std::cout << m_Nodes.size();
        LOG(LogLevel::DEBUG, "Model loaded successfully: " + filepath);
    }
}

Model::Model(const std::string &filepath, lgt::Scene*_scene) : m_ModelFilepath(filepath)
{
 
    m_TextureFilePath = std::filesystem::path(filepath).parent_path().string();
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        filepath,
        aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_GlobalScale |
            aiProcess_PreTransformVertices);
    // Error handling
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG(LogLevel::DEBUG, "Importing  Model  " + filepath);
        LOG(LogLevel::_ERROR, "Failed to load model: " + filepath);
        LOG(LogLevel::_ERROR, "Assimp error: " + std::string(importer.GetErrorString()));
    }
    else
    {
        // Logging import success
        LOG(LogLevel::DEBUG, "Model imported successfully: " + filepath);
        LOG(LogLevel::DEBUG, "Number of m_Meshes: " + std::to_string(scene->mNumMeshes));
        LOG(LogLevel::DEBUG, "Loading model...");
        LOG(LogLevel::DEBUG, "Processing root node...");
        processNode(scene->mRootNode, scene);
        std::cout << m_Nodes.size();
        LOG(LogLevel::DEBUG, "Model loaded successfully: " + filepath);
        // create a Scene 
        for (auto node : m_Nodes)
        {
            Renderable  component;
            lgt::Entity  e = _scene->m_Roster->createEntity(node.name);
             component.Transform = node._transform;
            for (auto mesh : node.meshes)
            {
                    component._meshes.push_back(mesh);
            }
            e.addComponent<Renderable>(component);
            _scene->m_Entites.push_back(e);
        }
    }
}

// Cleanup resources
void Model::cleanUp()
{
    LOG(LogLevel::DEBUG, "Destroying model");

    for (size_t i = 0; i < m_Meshes.size(); ++i)
    {
        m_Meshes[i].cleanUp();
    }

    LOG(LogLevel::DEBUG, "Model destroyed");
}

// Load material properties
Material Model::LoadMaterial(aiMaterial *M) const
{
    Material material;
    aiColor3D color;
    float shininess;

    // Diffuse color
    if (M->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
    {
        material.diffuse = glm::vec3(color.r, color.g, color.b);
        LOG(LogLevel::_INFO, "Diffuse: " + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b));
    }
    // Specular color
    if (M->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
    {
        material.specular = glm::vec3(color.r, color.g, color.b);
        LOG(LogLevel::_INFO, "Specular: " + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b));
    }
    // Ambient color
    if (M->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
    {
        material.ambient = glm::vec3(color.r, color.g, color.b);
        LOG(LogLevel::_INFO, "Ambient: " + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b));
    }
    // Shininess
    if (M->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
    {
        material.shininess = shininess;
        LOG(LogLevel::_INFO, "Shininess: " + std::to_string(shininess));
    }
    return material;
}

// Process mesh data
Mesh Model::processMesh(const aiMesh *mesh, const aiScene *scene)
{
    // Updated texture type mapping for better organization
    struct TextureTypeInfo
    {
        aiTextureType assimpType;
        TextureType meshType;
        std::string name;
    };

    std::vector<TextureTypeInfo> textureTypes = {
        {aiTextureType_DIFFUSE, TextureType::DIFFUSE, "diffuse"},
        {aiTextureType_NORMALS, TextureType::NORMAL, "normal"},
        {aiTextureType_SPECULAR, TextureType::SPECULAR, "specular"},
        {aiTextureType_HEIGHT, TextureType::NORMAL, "height"} // Height maps can be used as normal maps
    };

    Material material;
    std::vector<vertex> Vertices;
    std::vector<unsigned int> Indices;
    std::vector<std::shared_ptr<Texture>> Textures;
    glm::mat4 meshTransform;

    LOG(LogLevel::_INFO, mesh->HasTangentsAndBitangents() ? "Mesh has tangent and bitangent data" : "Mesh lacks tangent and bitangent data");

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        vertex v;
        v.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        // Normals
        if (mesh->HasNormals())
            v.norm = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        else
            v.norm = glm::vec3(0.0f, 1.0f, 0.0f); // Default up vector

        // Texture Coordinates
        if (mesh->mTextureCoords[0])
            v.textcoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
            v.textcoord = glm::vec2(0.0f);

        // Tangents and Bitangents
        if (mesh->HasTangentsAndBitangents())
        {
            v.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            v.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        }
        else
        {
            // Generate basic tangent space if not available
            v.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
            v.bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
        }

        Vertices.push_back(v);
    }

    // Process indices (faces)
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; ++j)
        {
            Indices.push_back(face.mIndices[j]);
        }
    }

    // Logging mesh details
    LOG(LogLevel::DEBUG, "Model Mesh Loaded: " + std::string(mesh->mName.C_Str()));
    LOG(LogLevel::_INFO, mesh->HasTextureCoords(0) ? "Mesh has texture coordinates" : "Mesh does not have texture coordinates");
    LOG(LogLevel::_INFO, "Number of vertices: " + std::to_string(Vertices.size()));
    LOG(LogLevel::_INFO, "Number of indices: " + std::to_string(Indices.size()));

    // Process material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *M = scene->mMaterials[mesh->mMaterialIndex];
        aiString materialName;

        if (M->Get(AI_MATKEY_NAME, materialName) == AI_SUCCESS)
        {
            LOG(LogLevel::_INFO, "Material Name: " + std::string(materialName.C_Str()));
        }

        // Load material properties first
        material = LoadMaterial(M);

        // Process textures with improved organization
        for (auto &typeInfo : textureTypes)
        {
            unsigned int textureCount = M->GetTextureCount(typeInfo.assimpType);

            if (textureCount > 0)
            {
                // Update material flags based on available textures
                if (typeInfo.meshType == TextureType::NORMAL)
                    material.hasNormalMap = true;
                else if (typeInfo.meshType == TextureType::SPECULAR)
                    material.hasSpecularMap = true;
            }

            for (unsigned int i = 0; i < textureCount; ++i)
            {
                aiString str;
                M->GetTexture(typeInfo.assimpType, i, &str);
                std::string texturePath = m_TextureFilePath + "/" + std::string(str.C_Str());

                auto texture = std::make_shared<Texture>(texturePath);
                texture->setType(typeInfo.meshType); // Set the texture type
                Textures.push_back(texture);

                LOG(LogLevel::_INFO, "Loaded " + typeInfo.name + " texture " + std::to_string(i + 1) +
                                         ": " + texturePath);
            }
        }
    }

    if (scene)
        return Mesh(Vertices, Indices, material, Textures);
}

glm::mat4 AiToGlm(const aiMatrix4x4 &from)
{
    return glm::mat4(
        from.a1, from.b1, from.c1, from.d1,
        from.a2, from.b2, from.c2, from.d2,
        from.a3, from.b3, from.c3, from.d3,
        from.a4, from.b4, from.c4, from.d4);
}

// Recursively process nodes
void Model::processNode(const aiNode *node, const aiScene *scene)
{
    Node myNode;
    myNode.name = std::string(node->mName.C_Str());
    std::string nodeInfo =
        "Node: " + std::string(node->mName.C_Str()) +
        " | m_Meshes: " + std::to_string(node->mNumMeshes) +
        " | Children: " + std::to_string(node->mNumChildren);

    LOG(LogLevel::_INFO, nodeInfo);

    // get the node transform
    myNode._transform = AiToGlm(node->mTransformation);
    // m_transforms.push_back(AiToGlm(node->mTransformation));

    // Process all m_Meshes for this node
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        myNode.meshes.push_back(processMesh(mesh, scene));
        //  m_Meshes.push_back(std::move(processMesh(mesh, scene)));
    }

    m_Nodes.push_back(std::move(myNode));
    // Recursively process each child node
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene);
    }
}

// Updated render function to work with the new shader
void Model::Render(const shader &Shader, const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix,
                   const glm::mat4 &projectionMatrix, const glm::vec3 &viewPos, const glm::vec3 &lightPos,
                   const glm::vec3 &lightColor, bool useColor, const glm::vec3 &color)
{
    // Set transformation matrices
    // Shader.setMat4("u_model", modelMatrix);
    Shader.setMat4("u_view", viewMatrix);
    Shader.setMat4("u_projection", projectionMatrix);

    // Calculate and set normal matrix
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
    Shader.setMat3("u_normalMatrix", normalMatrix);

    // Set view position
    Shader.setVec3("u_viewPos", viewPos);

    // Set light properties
    Shader.setVec3("u_light.position", lightPos);
    Shader.setVec3("u_light.color", lightColor);
    Shader.setFloat("u_light.intensity", 1.0f);
    Shader.setFloat("u_light.constant", 1.0f);
    Shader.setFloat("u_light.linear", 0.09f);
    Shader.setFloat("u_light.quadratic", 0.032f);

    // Set color mode
    Shader.setBool("u_useColor", useColor);
    if (useColor)
    {
        Shader.setVec3("u_color", color);
    }

    // Render each mesh
    for (size_t i = 0; i < m_Meshes.size(); ++i)
    {
        m_Meshes[i].render(Shader);
    }
}

// Convenience overload that maintains backward compatibility
void Model::Render(const shader &Shader)
{

    for (auto node : m_Nodes)
    {
        Shader.setMat4("u_model", node._transform);
        for (auto mesh : node.meshes)
        {
            mesh.render(Shader);
        }
    }
}