#include<filesystem>
#include "Model.h"

// Constructor
Model::Model(const std::string& filepath):m_ModelFilepath(filepath)
{
    
    m_TextureFilePath = std::filesystem::path(filepath).parent_path().string();
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        filepath,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        //aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace |
        aiProcess_PreTransformVertices
    );

   
    // Error handling
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG(LogLevel::DEBUG, "Importing  Model  " + filepath);
        LOG(LogLevel::_ERROR, "Failed to load model: " + filepath);
        LOG(LogLevel::_ERROR, "Assimp error: " + std::string(importer.GetErrorString()));
        return;
    }

    // Logging import success
    LOG(LogLevel::DEBUG, "Model imported successfully: " + filepath);
    LOG(LogLevel::DEBUG, "Number of m_Meshes: " + std::to_string(scene->mNumMeshes));
    LOG(LogLevel::DEBUG, "Loading model...");
    LOG(LogLevel::DEBUG, "Processing root node...");

    // Process root node recursively
    processNode(scene->mRootNode, scene);

    LOG(LogLevel::DEBUG, "Model loaded successfully: " + filepath);
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
Material Model::LoadMaterial(aiMaterial* M) const
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
    else
    {
        LOG(LogLevel::_WARNING, "Diffuse color not found. Defaulting to (0, 0, 0).");
        material.diffuse = glm::vec3(0.0f);
    }

    // Specular color
    if (M->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
    {
        material.specular = glm::vec3(color.r, color.g, color.b);
        LOG(LogLevel::_INFO, "Specular: " + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b));
    }
    else
    {
        LOG(LogLevel::_WARNING, "Specular color not found. Defaulting to (0, 0, 0).");
        material.specular = glm::vec3(1.0f);
    }

    // Ambient color
    if (M->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
    {
        material.ambient = glm::vec3(color.r, color.g, color.b);
        LOG(LogLevel::_INFO, "Ambient: " + std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b));
    }
    else
    {
        LOG(LogLevel::_WARNING, "Ambient color not found. Defaulting to (0, 0, 0).");
        material.ambient = glm::vec3(.0f);
    }

    // Shininess
    if (M->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
    {
        material.shininess = shininess;
        LOG(LogLevel::_INFO, "Shininess: " + std::to_string(shininess));
    }
    else
    {
        LOG(LogLevel::_WARNING, "Shininess not found. Defaulting to 16.0.");
        material.shininess = 16.0f;
    }

    return material;
}

// Process mesh data
Mesh Model::processMesh(const aiMesh* mesh, const aiScene* scene)
{
    std::vector<aiTextureType> textureTypes = {
                     aiTextureType_DIFFUSE
                    //,aiTextureType_SPECULAR
                    ,aiTextureType_NORMALS
                    //,aiTextureType_UNKNOWN
                  };
    Material                              material;
    std::vector<vertex>                   Vertices;
    std::vector<unsigned int>             Indices;
    std::vector<std::shared_ptr<Texture>> Textures;
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
            v.norm = glm::vec3(0.0f);

        // Texture Coordinates
        if (mesh->mTextureCoords[0])
            v.textcoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
            v.textcoord = glm::vec2(0.0f);

        // Tangents
        if (mesh->HasTangentsAndBitangents())
            v.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
        else
            v.tangent = glm::vec3(0.0f);
        
        // Bitangents
        if (mesh->HasTangentsAndBitangents())
            v.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        else
            v.bitangent = glm::vec3(0.0f);

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
        aiMaterial* M = scene->mMaterials[mesh->mMaterialIndex];
        aiString materialName;

        if (M->Get(AI_MATKEY_NAME, materialName) == AI_SUCCESS)
        {
            LOG(LogLevel::_INFO, "Material Name: " + std::string(materialName.C_Str()));
        }
        for (auto& type : textureTypes)
        {
            for (unsigned int i = 0; i < M->GetTextureCount(type); ++i)
            {
                aiString str;
                M->GetTexture(type, i, &str);
                std::string texturePath = m_TextureFilePath+"/"+std::string(str.C_Str());
                Textures.push_back(std::make_shared<Texture>(texturePath));
                LOG(LogLevel::_INFO, "Loaded " + std::to_string(i+1) +
                    " texture of type " + std::to_string(static_cast<int>(type)) +
                    ": " + texturePath);
            }
        }
        material = LoadMaterial(M);
    }
    else
    {
        LOG(LogLevel::_INFO, "Mesh does not have a material. Using default material.");
        material = {
            glm::vec3(1.0f),         // Specular
            glm::vec3(0.5f),         // Ambient
            glm::vec3(1.0f, 0.0f, 0.0f), // Diffuse
            16.0f                  // Shininess
        };
    }
   
    return Mesh(Vertices, Indices, material ,Textures);
}

// Recursively process nodes
void Model::processNode(const aiNode* node, const aiScene* scene)
{
    std::string nodeInfo =
        "Node: " + std::string(node->mName.C_Str()) +
        " | m_Meshes: " + std::to_string(node->mNumMeshes) +
        " | Children: " + std::to_string(node->mNumChildren);

    LOG(LogLevel::_INFO, nodeInfo);

    // Process all m_Meshes for this node
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(processMesh(mesh, scene));
    }

    // Recursively process each child node
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene);
    }
}

// Render the model
void Model::Render(const shader& Shader)
{
    for (size_t i = 0; i < m_Meshes.size(); ++i)
    {
        m_Meshes[i].render(Shader);
    }
}
