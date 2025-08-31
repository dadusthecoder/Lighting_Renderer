#include "Texture.h"


Texture::Texture(const std::string& filepath) :
	m_RenderID(0), m_height(0), m_width(0), m_bpp(0), m_localbuffer(nullptr)
{
	GLenum dataFormat = GL_RGBA;
	GLenum internalFormat = GL_RGBA8;

	stbi_set_flip_vertically_on_load(1);
	m_localbuffer = stbi_load(filepath.c_str(), &m_width, &m_height, &m_bpp, 0);

	if (!m_localbuffer) {
		LOG(LogLevel::_ERROR, "Failed to load image: " + filepath);
		return;
	}

	switch (m_bpp)
	{
	case 1:
		dataFormat = GL_RED;
		internalFormat = GL_RED;
		break;
	case 3:
		dataFormat = GL_RGB;
		internalFormat = GL_RGB;
		break;
	case 4:
		dataFormat = GL_RGBA;
		internalFormat = GL_RGBA;
		break;
	default:
		LOG(LogLevel::_ERROR ,"Unsupported texture format: "  + std::to_string(m_bpp )) ;
		break;
	}

	glGenTextures(1, &m_RenderID);
	glBindTexture(GL_TEXTURE_2D, m_RenderID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, dataFormat, GL_UNSIGNED_BYTE, m_localbuffer);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(m_localbuffer);
}
GLuint Texture::GetTextureID()
{
	return m_RenderID;
}
void  Texture::cleanUp()
{
	LOG(LogLevel::DEBUG, "Deleting Texture ID: " + std::to_string(m_RenderID));
	glDeleteTextures(1, &m_RenderID);
	LOG(LogLevel::DEBUG, "Texture deleted");
}

void Texture::Bind(unsigned int slot) const
{   
		glActiveTexture(GL_TEXTURE0+slot);
 		glBindTexture(GL_TEXTURE_2D, m_RenderID);
 }

void Texture::Unbind() const 
{
	glBindTexture(GL_TEXTURE_2D,0);
}

void Texture::setType(TextureType type)
{
	m_type = type; 
}

TextureType Texture::getType() const
{
	return m_type;
}
