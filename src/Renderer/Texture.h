#pragma once
#include<string>
#include"stb_image.h"
#include"renderer.h"

enum class TextureType {
	DIFFUSE = 0,
	NORMAL = 1,
	SPECULAR = 2,
	AMBIENT = 3,
	HEIGHT = 4
};

class Texture 
{
private :
	TextureType  m_type;
	unsigned int m_RenderID;
    int m_height, m_width, m_bpp;
	unsigned char* m_localbuffer;
public :
	void setType(TextureType type);
	TextureType getType() const;
	Texture(const std::string& filepath );
	GLuint GetTextureID();
	void cleanUp();
	void Bind(unsigned int slot)   const ;
	void Unbind() const ;
};