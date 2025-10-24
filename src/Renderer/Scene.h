#pragma once
#include "Mesh.h"
#include "renderer.h"
#include "ecs/ECS.h"

struct Renderable
{
    std::vector<Mesh> _meshes;
    glm::mat4 Transform;
};
LGT_REGISTER_COMPONENT(lgt, Renderable);

namespace lgt
{
    class Scene
    {
    public:
        void Render(const shader &Shader)
        {
            for (auto e : m_Entites)
            {
                auto &component = e.getComponent<Renderable>();
                Shader.setMat4("u_model", component.Transform);
                for (auto &mesh : component._meshes)
                {
                    mesh.render(Shader);
                }
            }
        }

        const std::vector<Entity> getEntites()
        {
            return m_Entites;
        }
        void RenderScenePanel( glm::mat4 view,glm::mat4 proj , ImGuizmo::OPERATION operation ) 
        {

            if (!this)
                return;

            ImGui::Begin("Entities");

            if (m_Entites.empty())
            {
                ImGui::TextDisabled("No entities in scene");
            }
            else
            {
                for (auto &entity : m_Entites)
                {
                    bool isSelected = (m_Selcted == entity.getHandle());

                    // Draw selectable entity row
                    if (ImGui::Selectable(entity.getName().c_str(),isSelected))
                    {

                        m_Selcted = entity.getHandle();
                        ImGuizmo::SetOrthographic(false);
                        ImGuizmo::SetDrawlist();

                        float windowheight = (float)ImGui::GetWindowHeight();
                        float windowwidth = (float)ImGui::GetWindowWidth();

                        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowwidth, windowheight);

                        ImGuizmo::Manipulate(
                            glm::value_ptr(view),
                            glm::value_ptr(proj),
                            operation,
                            ImGuizmo::LOCAL,
                            glm::value_ptr(entity.getComponent<Renderable>().Transform));
                    }

                    // Right-click context menu
                    if (ImGui::BeginPopupContextItem(entity.getName().c_str()))
                    {
                        if (ImGui::MenuItem("Select"))
                        {
                            //m_Selcted = entity.getHandle();
                        }
                    }
                }
            }

            ImGui::End();
        }

        Scene()
        {
            m_Roster = std::make_unique<Roster>();
        }

    private:
        EntityHandle m_Selcted;
        Scope<Roster> m_Roster;
        std::vector<Entity> m_Entites;

        friend Model;
    };
}
