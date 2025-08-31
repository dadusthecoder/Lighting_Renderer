//#include "testmodel.h"
//#include "helpers/Filedial.h"
//#include "renderer/camera.h"
//
//
//testModel::testModel() : m_speed(0.030f)
//{
//    // ImGui styling
//    ImGuiStyle& style = ImGui::GetStyle();
//    style.WindowRounding = 3.0f;
//    style.FrameRounding = 2.0f;
//    style.ScrollbarRounding = 2.0f;
//    style.GrabRounding = 2.0f;
//    style.WindowPadding = ImVec2(8, 8);
//    style.FramePadding = ImVec2(6, 4);
//
//    ImVec4* colors = style.Colors;
//    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
//    colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
//    colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
//    colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
//    colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
//    colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
//    colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
//    colors[ImGuiCol_Border] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
//
//    // Initialize transformation matrices
//    m_modelMatrix = glm::mat4(1.0f);
//
//    // Initialize lighting parameters
//    m_lightSettings = {
//        glm::vec3(2.0f, 4.0f, 2.0f),  // Direction
//        glm::vec3(2.0f, 4.0f, 2.0f),  // position
//        glm::vec3(1.0f, 1.0f, 1.0f),  // color
//        1.0f,                          // intensity
//        1.0f,                          // constant
//        0.09f,                         // linear
//        0.032f                         // quadratic
//    };
//
//    // Initialize rendering components
//    m_render = std::make_unique<renderer>();
//    m_render->logGlVersion();
//    m_render->Initauad();
//
//    // Load default model and shader
//    try {
//        m_model = std::make_unique<Model>("res/modles/Sphere.fbx");
//        m_colorshader = std::make_unique<shader>("res/shaders/bsc.shader", ShaderType::COLORSHADER);
//        m_depthshader = std::make_unique<shader>("res/shaders/Depth.shader", ShaderType::DEPTHSHADER);
//        m_shadowdebugshader = std::make_unique<shader>("res/shaders/ShadowDebug.shader", ShaderType::COLORSHADER);
//
//        m_grid = std::make_unique<Grid>();
//
//        m_shadowdebugbuffer = std::make_unique<FrameBuffer>(SHADOW_WIDTH, SHADOW_HEIGHT);
//        m_colorbuffer = std::make_unique<FrameBuffer>(1920, 1080);
//        m_depthbuffer = std::make_unique<DepthBuffer>();
//    }
//    catch (const std::exception& e) {
//        LOG(LogLevel::_ERROR, "Failed to initialize model or shader: " + std::string(e.what()));
//    }
//
//    //for scene
//    m_sceneSize = ImVec2(m_colorbuffer->GetWidth(), m_colorbuffer->GetHeight());
//
//    m_camera = std::make_unique<camera>(800.0f, 800.0f, m_viewPos);
//    m_shadowcam = std::make_unique<ShadowCamera>(SHADOW_WIDTH, SHADOW_HEIGHT, m_lightSettings.position, m_camera->GetDirection());
//}
//
//testModel::~testModel()
//{
//    if (m_model) {
//        m_model->cleanUp();
//    }
//}
//
//
////main render function 
//void testModel::onRender()
//{
//    m_render->Clear(); //clear main(default freambuffer first)
//
//    renderShadowPass();
//    renderShadowDebugPass();
//    renderColorPass();
//
//}
//
//void testModel::onUpdate(GLFWwindow* window)
//{
//    m_window = window;
//
//    m_camera->inputs(window, m_speed, 40.0f);
//    m_deltaTime = 1.0f / m_performanceStats.fps;
//    m_timestep += m_deltaTime;
//
//    //temperory
//
//    //
//
//    if (m_physicsSettings.enableGravity) {
//
//
//        m_physicsSettings.velocity.y += m_physicsSettings.gravityForce * m_deltaTime;
//        m_transformSettings.position += m_physicsSettings.velocity * m_deltaTime;
//
//        if (m_transformSettings.position.y < m_physicsSettings.groundLevel) {
//            m_transformSettings.position.y = m_physicsSettings.groundLevel;
//            m_physicsSettings.velocity.y = -m_physicsSettings.velocity.y * m_physicsSettings.bounceDamping;
//        }
//    }
//
//    m_shadowcam->Update(m_lightSettings.position, m_lightSettings.direction);
//    updateModelMatrix();
//
//    //temp code for input
//
//    if (glfwGetKey(m_window, GLFW_KEY_P) == GLFW_PRESS && m_timestep > 0.56f) {
//        m_timestep = 0.0f;
//
//        switch (m_renderpasstype)
//        {
//        case RenderPassType::SHADOW_PASS:
//            setRenderPass(RenderPassType::COLOR_PASS);
//            break;
//        case RenderPassType::COLOR_PASS:
//            setRenderPass(RenderPassType::SHADOW_PASS);
//
//            break;
//        default:
//            break;
//        }
//
//    }
//}
//
//void testModel::updateModelMatrix()
//{
//    m_modelMatrix = glm::mat4(1.0f);
//    m_modelMatrix = glm::translate(m_modelMatrix, m_transformSettings.position);
//    m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_transformSettings.rotation.x), glm::vec3(1, 0, 0));
//    m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_transformSettings.rotation.y), glm::vec3(0, 1, 0));
//    m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_transformSettings.rotation.z), glm::vec3(0, 0, 1));
//    m_modelMatrix = glm::scale(m_modelMatrix, m_transformSettings.scale);
//}
//
//void testModel::onImguiRender()
//{
//    ImGui_ImplOpenGL3_NewFrame();
//    ImGui_ImplGlfw_NewFrame();
//    ImGui::NewFrame();
//
//    //ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
//    //ImGui::SetNextWindowSize(ImVec2(400, 700), ImGuiCond_Once);
//
//    ImGui::Begin("Advanced Scene Controller2", nullptr
//
//        //,ImGuiWindowFlags_NoResize |
//        // ImGuiWindowFlags_NoMove |
//        // ImGuiWindowFlags_NoCollapse
//    );
//
//    renderLightingControls();
//    renderMaterialControls();
//    renderEnvironmentControls();
//    renderCameraControls();
//    renderTransformControls();
//    renderPhysicsControls();
//    renderPerformanceStats();
//    renderGridControls();
//    renderAssetControls();
//    ImGui::End();
//
//    renderGizmoTest(m_camera->GetViewMatrix(), m_camera->GetProjectionMatrix());
//
//    ImGui::Begin("Scene", nullptr);
//    switch (m_renderpasstype)
//    {
//    case RenderPassType::SHADOW_PASS:
//        displayShadowPass();
//        break;
//    case RenderPassType::COLOR_PASS:
//        displayColorPass();
//        break;
//    default:
//        break;
//    }
//    ImGui::End();
//
//    ImGui::Render();
//    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//}
//
//void testModel::displayShadowPass() {
//
//    ImGui::Image((ImTextureID)(intptr_t)m_depthbuffer->GetTextureId(), m_sceneSize, ImVec2(0, 1), ImVec2(1, 0));
//}
//void testModel::displayColorPass() {
//
//    ImGui::Image((ImTextureID)(intptr_t)m_colorbuffer->GetTextureId(), m_sceneSize, ImVec2(0, 1), ImVec2(1, 0));
//}
//
//void testModel::renderLightingControls()
//{
//    ImGui::SeparatorText("Advanced Lighting");
//    if (ImGui::CollapsingHeader("Light Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
//        ImGui::DragFloat3("Light Position", &m_lightSettings.position[0], 0.1f, -100.0f, 100.0f, "%.2f");
//        ImGui::DragFloat3("Light Direction", &m_lightSettings.direction[0], 0.1f, -100.0f, 100.0f, "%.2f");
//
//        ImGui::ColorEdit3("Light Color", &m_lightSettings.color[0]);
//        ImGui::SliderFloat("Intensity", &m_lightSettings.intensity, 0.0f, 5.0f);
//
//        if (ImGui::Button("Align Light to Camera")) {
//            m_lightSettings.position = m_camera->GetCameraPos();
//            m_lightSettings.direction = m_camera->GetDirection();
//        }
//        if (ImGui::CollapsingHeader("Attenuation")) {
//            ImGui::SliderFloat("Constant", &m_lightSettings.constant, 0.1f, 2.0f);
//            ImGui::SliderFloat("Linear", &m_lightSettings.linear, 0.001f, 0.5f, "%.3f");
//            ImGui::SliderFloat("Quadratic", &m_lightSettings.quadratic, 0.001f, 0.1f, "%.4f");
//
//            // Show attenuation visualization
//            float distance = glm::length(m_lightSettings.position - m_transformSettings.position);
//            float attenuation = 1.0f / (m_lightSettings.constant +
//                m_lightSettings.linear * distance +
//                m_lightSettings.quadratic * distance * distance);
//            ImGui::Text("Distance: %.2f | Attenuation:t %.3f", distance, attenuation);
//
//        }
//    }
//}
//
//void testModel::renderMaterialControls()
//{
//    ImGui::SeparatorText("Material & Rendering");
//    if (ImGui::CollapsingHeader("Render Mode", ImGuiTreeNodeFlags_DefaultOpen)) {
//        const char* renderModes[] = { "Fill", "Wireframe", "Point" };
//        static int currentMode = 0;
//        if (ImGui::Combo("Polygon Mode", &currentMode, renderModes, IM_ARRAYSIZE(renderModes))) {
//            m_renderMode = static_cast<RenderMode>(currentMode);
//            m_render->setRenderMode(m_renderMode);
//        }
//
//        ImGui::Checkbox("Use Solid Color", &m_renderingSettings.useColor);
//        if (m_renderingSettings.useColor) {
//            ImGui::ColorEdit3("Solid Color", &m_renderingSettings.solidColor[0]);
//        }
//
//        if (ImGui::CollapsingHeader("Advanced Material")) {
//            ImGui::SliderFloat("Normal Map Strength", &m_materialSettings.normalStrength, 0.0f, 3.0f);
//            ImGui::Checkbox("Enable Normal Mapping", &m_materialSettings.hasNormalMap);
//            ImGui::Checkbox("Enable Specular Mapping", &m_materialSettings.hasSpecularMap);
//
//            ImGui::ColorEdit3("Ambient", &m_materialSettings.ambient[0]);
//            ImGui::ColorEdit3("Diffuse", &m_materialSettings.diffuse[0]);
//            ImGui::ColorEdit3("Specular", &m_materialSettings.specular[0]);
//            ImGui::SliderFloat("Shininess", &m_materialSettings.shininess, 1.0f, 256.0f);
//        }
//    }
//}
//
//void testModel::renderEnvironmentControls()
//{
//    ImGui::SeparatorText("Environment");
//    if (ImGui::CollapsingHeader("Background & Movement", ImGuiTreeNodeFlags_DefaultOpen)) {
//        ImGui::ColorEdit3("Background Color", &m_backgroundColor[0]);
//        ImGui::SliderFloat("Camera Speed", &m_speed, 0.001f, 1.0f, "%.3f");
//        ImGui::TextWrapped("Tip: Lower speed helps with precision movement.");
//    }
//}
//
//void testModel::renderCameraControls()
//{
//    ImGui::SeparatorText("Camera");
//    if (ImGui::CollapsingHeader("Camera Info")) {
//        glm::vec3 camPos = m_camera->GetCameraPos();
//        glm::vec3 camFront = m_camera->getFront();
//        ImGui::Text("Position:  (%.2f, %.2f, %.2f)", camPos.x, camPos.y, camPos.z);
//        ImGui::Text("Direction: (%.2f, %.2f, %.2f)", camFront.x, camFront.y, camFront.z);
//
//        if (ImGui::Button("Reset Camera Position")) {
//            // Reset camera to default position
//            m_camera = std::make_unique<camera>(800.0f, 800.0f, glm::vec3(0.0f, 0.0f, 3.0f));
//        }
//    }
//}
//
//void testModel::renderTransformControls()
//{
//    ImGui::SeparatorText("Model Transformations");
//    if (ImGui::CollapsingHeader("Transform Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
//        ImGui::DragFloat3("Position", &m_transformSettings.position[0], 0.1f, -100.0f, 100.0f);
//        ImGui::SameLine();
//        if (ImGui::Button("Reset##Pos")) m_transformSettings.position = glm::vec3(0.0f);
//
//        ImGui::DragFloat3("Rotation", &m_transformSettings.rotation[0], 0.5f, -180.0f, 180.0f);
//        ImGui::SameLine();
//        if (ImGui::Button("Reset##Rot")) m_transformSettings.rotation = glm::vec3(0.0f);
//
//        ImGui::DragFloat3("Scale", &m_transformSettings.scale[0], 0.1f, 0.01f, 20.0f);
//        ImGui::SameLine();
//        if (ImGui::Button("Reset##Sca")) m_transformSettings.scale = glm::vec3(1.0f);
//
//        if (ImGui::Button("Reset All Transforms")) {
//            m_transformSettings.position = glm::vec3(0.0f);
//            m_transformSettings.rotation = glm::vec3(0.0f);
//            m_transformSettings.scale = glm::vec3(1.0f);
//        }
//    }
//}
//
//void testModel::renderPhysicsControls()
//{
//    ImGui::SeparatorText("Physics Simulation");
//    if (ImGui::CollapsingHeader("Physics Settings")) {
//        ImGui::Checkbox("Enable Gravity", &m_physicsSettings.enableGravity);
//
//        if (m_physicsSettings.enableGravity) {
//            ImGui::SliderFloat("Gravity Force", &m_physicsSettings.gravityForce, -20.0f, -0.1f);
//            ImGui::SliderFloat("Ground Level", &m_physicsSettings.groundLevel, -10.0f, 10.0f);
//            ImGui::SliderFloat("Bounce Damping", &m_physicsSettings.bounceDamping, 0.0f, 1.0f);
//
//            ImGui::Text("Current Velocity: (%.2f, %.2f, %.2f)",
//                m_physicsSettings.velocity.x,
//                m_physicsSettings.velocity.y,
//                m_physicsSettings.velocity.z);
//
//            if (ImGui::Button("Reset Velocity")) {
//                m_physicsSettings.velocity = glm::vec3(0.0f);
//            }
//        }
//        else {
//            m_physicsSettings.velocity = glm::vec3(0.0f);
//        }
//    }
//}
//
//void testModel::renderAssetControls()
//{
//    ImGui::SeparatorText("Asset Management");
//    if (ImGui::CollapsingHeader("Load Assets"))
//    {
//        static std::string modelPath = "select (*.model)";
//        static std::string shaderPath = "select (*.shader)";
//
//        ImGui::Text("Model Path: %s", modelPath.c_str());
//        ImGui::SameLine();
//        if (ImGui::Button("...")) {
//            std::string selected = FileDial::OpenFile();
//            if (!selected.empty()) {
//                modelPath = selected;
//                loadModel(modelPath);
//            }
//        }
//        ImGui::Text("Shader Path: %s", shaderPath.c_str());
//        ImGui::SameLine();
//        if (ImGui::Button("..")) {
//            std::string selected = FileDial::OpenFile();
//            if (!selected.empty()) {
//                shaderPath = selected;
//                loadShader(shaderPath);
//            }
//        }
//
//        if (ImGui::Button("Reload Current Shader")) {
//            if (m_colorshader) {
//                m_colorshader->reload();
//            }
//        }
//
//        if (m_colorshader && ImGui::Button("Debug Shader Uniforms")) {
//            m_colorshader->printActiveUniforms();
//        }
//    }
//}
//
//
//void testModel::renderPerformanceStats()
//{
//    ImGui::SeparatorText("Performance");
//    m_performanceStats.fps = m_render->updateAndLogFPS(m_window);
//    ImGui::Text("FPS: %.1f", m_performanceStats.fps);
//    ImGui::Text("Frame Time: %.3f ms", 1000.0f / m_performanceStats.fps);
//
//    static bool showDemoWindow = false;
//    ImGui::Checkbox("Show ImGui Demo", &showDemoWindow);
//    if (showDemoWindow) {
//        ImGui::ShowDemoWindow(&showDemoWindow);
//    }
//
//    ImGui::TextDisabled("Enhanced PBR Renderer | OpenGL + ImGui");
//}
//
//void testModel::renderGridControls()
//{
//    ImGui::SeparatorText("Space-Time Fabric");
//    if (ImGui::CollapsingHeader("Fabric Settings")) {
//        auto& settings = m_grid->getSetting();
//
//        ImGui::Checkbox("Enable Animation", &settings.enableAnimation);
//        ImGui::Checkbox("Show Grid", &settings.enableGrid);
//        ImGui::Checkbox("Enable Gradient", &settings.enableGradient);
//
//        if (settings.enableAnimation) {
//            ImGui::SliderFloat("Wave Amplitude", &settings.waveAmplitude, 0.0f, 2.0f);
//            ImGui::SliderFloat("Wave Frequency", &settings.waveFrequency, 0.01f, 0.5f);
//        }
//
//        ImGui::ColorEdit3("Base Color", &settings.baseColor[0]);
//        if (settings.enableGradient) {
//            ImGui::ColorEdit3("Gradient Color", &settings.gradientColor[0]);
//        }
//
//        ImGui::SliderFloat("Fade Distance", &settings.fadeDistance, 10.0f, 200.0f);
//
//        if (settings.enableGrid) {
//            ImGui::SliderFloat("Grid Intensity", &settings.gridIntensity, 0.0f, 2.0f);
//        }
//
//        if (ImGui::Button("Reset Fabric Settings")) {
//            settings = GridSettings();
//        }
//    }
//}
//
//
//void testModel::loadModel(const std::string& filepath)
//{
//    try {
//        if (m_model) {
//            m_model->cleanUp();
//        }
//
//        m_model = std::make_unique<Model>(filepath);
//
//        // Reset transform when loading new model
//        m_transformSettings.position = glm::vec3(0.0f);
//        m_transformSettings.rotation = glm::vec3(0.0f);
//        m_transformSettings.scale = glm::vec3(1.0f);
//
//        LOG(LogLevel::_INFO, "Successfully loaded model: " + filepath);
//    }
//    catch (const std::exception& e) {
//        LOG(LogLevel::_ERROR, "Failed to load model '" + filepath + "': " + std::string(e.what()));
//    }
//}
//
//void testModel::loadShader(const std::string& filepath)
//{
//    try {
//        auto newShader = std::make_unique<shader>(filepath);
//
//        if (newShader->isValid()) {
//            m_colorshader = std::move(newShader);
//            LOG(LogLevel::_INFO, "Successfully loaded shader: " + filepath);
//        }
//        else {
//            LOG(LogLevel::_ERROR, "Shader validation failed: " + filepath);
//        }
//    }
//    catch (const std::exception& e) {
//        LOG(LogLevel::_ERROR, "Failed to load shader '" + filepath + "': " + std::string(e.what()));
//    }
//}
//
//void testModel::renderColorPass()
//{
//    m_colorbuffer->Use();
//    m_render->Clear(m_backgroundColor);
//    m_render->setViewport(m_sceneSize.x, m_sceneSize.y);
//
//    if (!m_colorshader || !m_colorshader->isValid() || !m_model) {
//        return;
//    }
//    shader::ScopedBind shaderBind(*m_colorshader);
//
//
//    m_colorshader->setMat4("u_model", m_modelMatrix);
//    m_colorshader->setMat4("u_view", m_camera->GetViewMatrix());
//    m_colorshader->setMat4("u_projection", m_camera->GetProjectionMatrix());
//
//
//    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(m_modelMatrix)));
//    m_colorshader->setMat3("u_normalMatrix", normalMatrix);
//
//    m_colorshader->setVec3("u_viewPos", m_camera->GetCameraPos());
//
//    m_colorshader->setMaterial(m_materialSettings);
//
//    m_colorshader->setLight(
//        m_lightSettings.position,
//        m_lightSettings.color,
//        m_lightSettings.intensity,
//        m_lightSettings.constant,
//        m_lightSettings.linear,
//        m_lightSettings.quadratic
//    );
//
//    m_colorshader->setTextures(0, 1, 2); // diffuse, normal, specular
//
//    m_colorshader->setBool("u_useColor", m_renderingSettings.useColor);
//    if (m_renderingSettings.useColor) {
//        m_colorshader->setVec3("u_color", m_renderingSettings.solidColor);
//    }
//
//    m_model->Render(*m_colorshader);
//    m_grid->render(*m_camera, m_deltaTime);
//    m_colorbuffer->Unuse();
//}
//
//void testModel::renderShadowPass() {
//
//    m_depthbuffer->Use();
//    glClear(GL_DEPTH_BUFFER_BIT);
//    m_render->setViewport(SHADOW_WIDTH, SHADOW_HEIGHT);
//
//    m_depthshader->use();
//
//    m_depthshader->setMat4("u_model", m_modelMatrix);
//    m_depthshader->setMat4("u_view", m_shadowcam->GetViewMatrix());
//    m_depthshader->setMat4("u_projection", m_shadowcam->GetProjectionMatrix());
//
//    m_model->Render(*m_depthshader);
//
//    m_depthshader->unuse();
//    m_depthbuffer->Unsue();
//}
//
//void testModel::renderShadowDebugPass()
//{
//    m_shadowdebugbuffer->Use();
//    m_render->Clear();
//    m_render->setViewport(SHADOW_WIDTH, SHADOW_HEIGHT);
//
//    m_shadowdebugshader->use();
//
//    m_shadowdebugshader->setInt("u_depthMap", 0);
//    m_depthbuffer->BindTex(0);
//
//    m_render->renderQuad();
//
//    m_depthbuffer->UnBindTex();
//    m_shadowdebugshader->unuse();
//    m_shadowdebugbuffer->Unuse();
//
//}
//
//inline void testModel::renderGizmoTest(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
//{
//
//    static glm::mat4 testModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
//    static ImGuizmo::OPERATION currentOp = ImGuizmo::TRANSLATE;
//
//    ImGuizmo::SetOrthographic(false);
//    ImGuizmo::SetDrawlist();
//
//    ImGuiIO& io = ImGui::GetIO();
//    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);  // Make sure origin is 0,0
//
//    ImGui::Begin("Gizmo Test");
//
//    if (ImGui::RadioButton("Translate", currentOp == ImGuizmo::TRANSLATE)) currentOp = ImGuizmo::TRANSLATE;
//    ImGui::SameLine();
//    if (ImGui::RadioButton("Rotate", currentOp == ImGuizmo::ROTATE)) currentOp = ImGuizmo::ROTATE;
//    ImGui::SameLine();
//    if (ImGui::RadioButton("Scale", currentOp == ImGuizmo::SCALE)) currentOp = ImGuizmo::SCALE;
//
//    ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix),
//        currentOp, ImGuizmo::WORLD,
//        glm::value_ptr(testModelMatrix));
//
//    if (ImGuizmo::IsUsing())
//    {
//        glm::vec3 skew;
//        glm::vec4 perspective;
//        glm::quat rotation;
//        glm::vec3 scale;
//        glm::vec3 position;
//
//        glm::decompose(testModelMatrix, scale, rotation, position, skew, perspective);
//        glm::vec3 euler = glm::degrees(glm::eulerAngles(rotation));
//        ImGui::Text("Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
//        ImGui::Text("Rotation: %.2f, %.2f, %.2f", euler.x, euler.y, euler.z);
//    }
//    ImGui::End();
//
//}