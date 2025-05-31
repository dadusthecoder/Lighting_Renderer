#pragma once

#include "Renderer/renderer.h"
#include "Renderer/Mesh.h"
#include "Renderer/Model.h"




class testModel
{
private:
	Material niceMaterial = {
	glm::vec3(0.2f, 0.2f, 0.2f), // Ambient: Low intensity for a subtle base color
	glm::vec3(0.8f, 0.8f, 0.8f), // Diffuse: Bright and reflective for a metallic look
	glm::vec3(1.0f, 1.0f, 1.0f), // Specular: High intensity for sharp highlights
	64.0f                        // Shininess: High value for a glossy surface
	};
    std::unique_ptr<shader> m_shader;
    std::unique_ptr<Texture> m_texture;
    std::unique_ptr<renderer> m_render;
    std::unique_ptr<camera> m_camera;
    std::unique_ptr<Model> m_model;

    glm::vec3 modelRotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 modelPosition = { 0.0f, 0.0f, 0.0f };
    glm::vec3 modelScale = { 1.0f, 1.0f, 1.0f };

	glm::vec3 viewp  = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 lightp = glm::vec3(-5.0, 5.0, 1.0);
	glm::vec3 lightc = glm::vec3(0.4f, 0.4f, 0.4f);
	glm::vec3 backc  = glm::vec3(0.0, 0.0, 0.0);
  

	glm::mat4 cube  = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);

	float testTime=0;
    float m_fps=0;
    bool m_Gravity = false;
    glm::vec3 m_velocity = glm::vec3(0.0f);
    float m_gravityForce = -10.0f;
    float groundLevel = 1.0f;

    RenderMode m_renderMode = RenderMode::WIREFRAME;
    GLFWwindow* m_window = nullptr;
    float m_speed ;

   ImGuizmo::OPERATION m_guizmoOp = ImGuizmo::TRANSLATE;

public:
    testModel();
	~testModel();
    void onRender();
    void onUpdate(GLFWwindow* window);
    void onImguiRnder();
};
