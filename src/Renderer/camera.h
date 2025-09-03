#pragma once
#include"renderer.h" 

class camera
{
private:
	glm::vec3 front=glm::vec3(0.0,0.0,-1.0);
	glm::vec3 m_position;
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 Campos;
	glm::vec3 newfront;
	glm::mat4 View = glm::mat4{ 1.0f };
	glm::mat4 Projection = glm::mat4{ 1.0f };

	float yaw=0,pitch=0,lastx = 0, lasty = 0;
	bool  firstclick=true;
	int   m_h, m_w;
	
public:
	 camera(float height, float width,glm::vec3 positon);
     void  inputs(GLFWwindow* window,  float& speed, const float& sensitivity);
	 glm::vec3 getPosition();
	 glm::vec3 getFront();
	 const glm::mat4 GetViewMatrix();
     const glm::mat4 GetProjectionMatrix();
	 const glm::vec3 GetCameraPos();
	 const glm::vec3 GetDirection();
}; 

class ShadowCamera
{
private:

	glm::vec3 m_direction = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 m_position = glm::vec3(2.0f, 4.0f, 2.0f);
	glm::vec3 m_up = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 m_newdirection;
	glm::mat4 m_view = glm::mat4{ 1.0f };
	glm::mat4 m_projection = glm::mat4{ 1.0f };

	float m_h, m_w;

public:
	ShadowCamera(float height, float width, glm::vec3 lightpos , glm::vec3 lightdir);
	void  Update(glm::vec3 lightpos , glm::vec3 lightdir);
	glm::vec3 getPosition();
	glm::vec3 getFront();
	const glm::mat4 GetViewMatrix();
	const glm::mat4 GetProjectionMatrix();
	const glm::vec3 GetCameraPos();
	
};
