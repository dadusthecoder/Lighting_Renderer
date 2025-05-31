#include "testmodel.h"
#include "testGimzos.h"

#include "renderer/camera.h"


testModel::testModel() :m_speed(0.030f)
{

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 3.0f;
	style.FrameRounding = 2.0f;
	style.ScrollbarRounding = 2.0f;
	style.GrabRounding = 2.0f;
	style.WindowPadding = ImVec2(8, 8);
	style.FramePadding = ImVec2(6, 4);

	ImVec4* colors = style.Colors;
	colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);       // Dark gray
	colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);             // Panel titles
	colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);         // Inputs
	colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);          // Buttons
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
	colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);               // Bright text
	colors[ImGuiCol_Border] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);             // Subtle borders

	model = glm::mat4(1.0f);
	m_render = std::make_unique<renderer>();
	m_render->logGlVersion();
	m_model  = std::make_unique<Model>("res/modles/Sphere.fbx");
	m_shader = std::make_unique<shader>("res/shaders/bsc.shader");
	m_camera = std::make_unique<camera>(800.0f, 800.0f, viewp);
}

testModel::~testModel()
{
	m_model->cleanUp();
}

void testModel::onRender()
{
	m_render->Clear(backc);
	m_shader->Bind_UseCamera(*m_camera);
	m_shader->setuniform1i("u_usec", 1);
	m_shader->setuniformvec3("lightp", lightp);
	m_shader->setuniformvec3("u_color", lightc);
	m_shader->setuniform4matf("u_matm", model);
	m_model->Render(*m_shader);
	m_shader->Unbind();
	m_render->SpaceTime_fabric(*m_camera);
}

void testModel::onUpdate(GLFWwindow* window)
{
	cube = glm::translate(glm::mat4(1.0f), lightp);
	
	
	m_shader->setuniform1i("u_usec", 1);
	m_camera->inputs(window, m_speed, 40.0f);
	if (m_Gravity)
	{
		m_velocity.y += m_gravityForce * (1 / m_fps);
		modelPosition += m_velocity * (1.0f / m_fps);    // move model based on velocity

		// Check if we've hit the ground
		if (modelPosition.y < groundLevel)
		{
			modelPosition.y = groundLevel;
			m_velocity.y = -m_velocity.y * 0.9f;
		}
	}
	model = glm::mat4(1.0f);
	model = glm::translate(model, modelPosition);
	model = glm::rotate(model, glm::radians(modelRotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(modelRotation.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(modelRotation.z), glm::vec3(0, 0, 1));
	model = glm::scale(model, modelScale);
}

void testModel::onImguiRnder()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(350, 600), ImGuiCond_Once);

	ImGui::Begin("Scene Controller", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_AlwaysAutoResize);


	ImGui::SeparatorText("Lighting Settings");
	if (ImGui::CollapsingHeader("Light Setup", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat3("Light Position", &lightp[0], 0.1f, -100.0f, 100.0f, "%.2f");
		if (ImGui::Button("Align Light to Camera")) {
			lightp = m_camera->getPosition();
		}
		ImGui::ColorEdit3("Light Color", &lightc[0]);
	}

	ImGui::SeparatorText("Environment");
	if (ImGui::CollapsingHeader("Background & Speed", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::ColorEdit3("Background Color", &backc[0]);
		ImGui::DragFloat("Camera Speed", &m_speed, 0.01f, 0.0f, 1.0f);
		ImGui::TextWrapped("Tip: Lower speed helps with precision camera movement.");
	}

	ImGui::SeparatorText("Camera");
	if (ImGui::CollapsingHeader("Camera Status")) {
		glm::vec3 camPos = m_camera->getPosition();
		glm::vec3 camFront = m_camera->getFront();
		ImGui::Text("Position:  (%.2f, %.2f, %.2f)", camPos.x, camPos.y, camPos.z);
		ImGui::Text("Direction: (%.2f, %.2f, %.2f)", camFront.x, camFront.y, camFront.z);
	}

	ImGui::SeparatorText("Render Mode");
	const char* renderModes[] = { "Fill", "Wireframe", "Point" };
	static int currentMode = 0;
	if (ImGui::Combo("Render Mode", &currentMode, renderModes, IM_ARRAYSIZE(renderModes))) {
		m_renderMode = static_cast<RenderMode>(currentMode);
		m_render->Rendermode(m_renderMode);
	}

	ImGui::SeparatorText("Model Controls");
	if (ImGui::CollapsingHeader("Transformations", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat3("Position", &modelPosition[0], 0.1f, -100.0f, 100.0f);
		ImGui::SameLine(); if (ImGui::Button("ResetPos")) modelPosition = glm::vec3(0.0f);

		ImGui::DragFloat3("Rotation", &modelRotation[0], 0.5f, -180.0f, 180.0f);
		ImGui::SameLine(); if (ImGui::Button("ResetRot")) modelRotation = glm::vec3(0.0f);

		ImGui::DragFloat3("Scale   ", &modelScale[0], 0.1f, 0.01f, 20.0f);
		ImGui::SameLine(); if (ImGui::Button("ResetSca")) modelScale = glm::vec3(1.0f);
	}

	ImGui::SeparatorText("Load Assets");
	static char modelPath[256] = "res/modles/sopnza_palace/scene.gltf";
	ImGui::InputText("Model Path", modelPath, IM_ARRAYSIZE(modelPath));
	if (ImGui::Button("Load Model")) {
		std::string newPath(modelPath);
		if (m_model) m_model->cleanUp();
		try {
			m_model = std::make_unique<Model>(newPath);
			modelPosition = glm::vec3(0.0f);
			modelRotation = glm::vec3(0.0f);
			modelScale = glm::vec3(1.0f);
		}
		catch (const std::exception& e) {
			std::cerr << "Failed to load model: " << e.what() << std::endl;
		}
	}

	static char shaderPath[256] = "res/shaders/bsc.shader";
	ImGui::InputText("Shader Path", shaderPath, IM_ARRAYSIZE(shaderPath));
	if (ImGui::Button("Reload Shader")) {
		try {
			m_shader->Reload();
		}
		catch (const std::exception& e) {
			LOG(LogLevel::_ERROR, "Failed to reload shader: " + std::string(e.what()));
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset Shader Path")) {
		strcpy_s(shaderPath, "res/shaders/sponza_palace/scene.gltf");
	}

	ImGui::SeparatorText("Simulation");
	ImGui::Checkbox("Gravity", &m_Gravity);
	if (!m_Gravity) m_velocity = glm::vec3(0.0f);

	ImGui::SeparatorText("Performance");
	m_fps = m_render->logfps(m_window);
	ImGui::Text("FPS: %.1f", m_fps);
	ImGui::TextDisabled("Blender-style UI | OpenGL + ImGui");

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
