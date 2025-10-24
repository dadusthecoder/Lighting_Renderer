#pragma once 
#include "Defines.h"
#include "ComponentManager.h"
#include <vector>
#include <queue>

namespace lgt {

    class Roster;

    class Entity {
    public:
        Entity(EntityHandle handle, Roster* registry, UUID id , std::string name);

        template<typename ComponentType, typename... Args>
        void addComponent(Args&&... args);

        template<typename ComponentType>
        bool removeComponent();

        template<typename ComponentType>
        ComponentType& getComponent();

        UUID getUUID() const { return m_UUID; }
        EntityHandle getHandle() const { return m_Handle; }
       
        const std::string& getName()const{
            return m_Name;
        }
    private:
        std::string m_Name;
        EntityHandle m_Handle;
        UUID m_UUID;  // unique ID for safety
        Roster* m_Registry;
    };

    //entity Roster
    class Roster {
    private:
        std::queue<EntityHandle>  m_AvailHandles;
        std::vector<UUID>         m_EntityIds;
        ComponentManager          m_ComponenetManager;

        template<typename ComponentType, typename... Args>
        void addComponent(const EntityHandle& handle, Args&&... args) {
            m_ComponenetManager.addComponent<ComponentType>(
                handle, ComponentType{ std::forward<Args>(args)... });
        }

        template<typename ComponentType>
        bool removeComponent(const EntityHandle& handle) {
            return m_ComponenetManager.removeComponent<ComponentType>(handle);
        }

        template<typename ComponentType>
        bool hasComponent(const EntityHandle& handle) {
            return m_ComponenetManager.hasComponent<ComponentType>(handle);
        }

        template<typename ComponentType>
        ComponentType& getComponent(const EntityHandle& handle) {
            LGT_ASSERT_MSG(m_ComponenetManager.hasComponent<ComponentType>(handle),
                "Trying to get an invalid component {Entity Dose not have requested Component}");
            return m_ComponenetManager.getComponent<ComponentType>(handle);
        }

        void removeAllComponents(const EntityHandle handle) {
            m_ComponenetManager.removeAllComponents(handle);
        }

    public:
        Roster() {
            for (EntityHandle e = 0; e < MAX_ENTITIES; e++) {
                m_AvailHandles.push(e);
                m_EntityIds.push_back(UUID()); // generate fresh UUIDs
            }
        }

        ~Roster() = default;

        Entity createEntity(std::string name  = "NoName") {

            LGT_ASSERT_MSG(!m_AvailHandles.empty(), "Entity Overflow");
            EntityHandle handle = m_AvailHandles.front();
            m_AvailHandles.pop();

            UUID id;
            m_EntityIds[handle] = id;

            return Entity(handle, this, id , name);
        }

        void destroyEntity(const EntityHandle& handle) {
            removeAllComponents(handle);
            m_EntityIds[handle] = UUID(); // reset UUID
            m_AvailHandles.push(handle);
        }

        const std::shared_ptr<Archetype> getArchetype(const Signature& signature) const {
            return m_ComponenetManager.getArchetype(signature);
        }

        friend class Entity;
    };

    // Entity method definitions
    inline Entity::Entity(EntityHandle handle, Roster* registry, UUID id  , std::string name)
        : m_Handle(handle), m_Registry(registry), m_UUID(id) ,m_Name(name) {
    }

    template<typename ComponentType, typename... Args>
    void Entity::addComponent(Args&&... args) {
        m_Registry->addComponent<ComponentType>(m_Handle, std::forward<Args>(args)...);
    }

    template<typename ComponentType>
    bool Entity::removeComponent() {
        return m_Registry->removeComponent<ComponentType>(m_Handle);
    }

    template<typename ComponentType>
    ComponentType& Entity::getComponent() {
        return m_Registry->getComponent<ComponentType>(m_Handle);
    }

} // namespace lgt
