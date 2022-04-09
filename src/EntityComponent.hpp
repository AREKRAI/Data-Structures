#pragma once

#include <vector>
#include <memory>
#include <optional>

#include <stdint.h>

// TODO:
// - Add removing entities
// - Add removing components

struct Entity
{
public:
    uint64_t id;

public:
    Entity(uint64_t id) : id(id) {}
};

struct Registry
{
public:
    ~Registry()
    {
    }

    Entity createEntity();

    template<typename T, typename ... AT>
    void addComponent(Entity entity, AT ... args)
    {
        size_t typeIdHash = typeid(T).hash_code();

        bool pushBackComponent = true;

        for (size_t ind = 0; ind < m_components.size(); ind++)
        {
            if (m_components[ind].typeID == typeIdHash)
            {
                ComponentList& componentList = m_components[ind];
                size_t slotIndex = 0;

                for (size_t entityInd = 0; entityInd < componentList.entities.size(); entityInd++)
                {
                    if (entity.id == componentList.entities[entityInd].id)
                    {
                        return; // entity already has this component
                    }
                    else if (entity.id > componentList.entities[entityInd].id)
                    {
                        componentList.entities.insert(componentList.entities.begin() + entityInd, entity);
                        
                        std::shared_ptr<std::vector<T>> dataPointer = std::static_pointer_cast<std::vector<T>>(componentList.data);
                        dataPointer->insert(dataPointer->begin() + entityInd, T(args...));

                        break;
                    }
                }

                pushBackComponent = false;
                break;
            }
            else if (typeIdHash > m_components[ind].typeID)
            {
                ComponentList componentList;
                componentList.typeID = typeIdHash;
                componentList.entities.push_back(entity);

                componentList.data = std::make_shared<std::vector<T>>();
                std::shared_ptr<std::vector<T>> dataPointer = std::static_pointer_cast<std::vector<T>>(componentList.data);
                dataPointer->push_back(T(args...));

                // insert component
                m_components.insert(m_components.begin() + ind, componentList);
                pushBackComponent = false;
                break;
            }
        }

        if (pushBackComponent)
        {
            ComponentList componentList;
            componentList.typeID = typeIdHash;
            componentList.entities.push_back(entity);

            componentList.data = std::make_shared<std::vector<T>>();
            std::shared_ptr<std::vector<T>> dataPointer = std::static_pointer_cast<std::vector<T>>(componentList.data);
            dataPointer->push_back(T(args...));

            m_components.push_back(componentList);
        }
    }

    template<typename T>
    std::shared_ptr<std::vector<T>> getComponents()
    {
        std::optional<size_t> componentIndex = getComponentIndex<T>();

        if (componentIndex.has_value())
            return std::static_pointer_cast<std::vector<T>>(m_components[componentIndex.value()].data);

        return nullptr;
    }

    template<typename T>
    T& getComponent(Entity entity)
    {
        std::optional<size_t> componentIndex = getComponentIndex<T>();
        
        if (componentIndex.has_value())
            for (size_t entityInd = 0; entityInd < m_components[componentIndex.value()].entities.size(); entityInd++)
                if (m_components[componentIndex.value()].entities[entityInd].id == entity.id)
                    return std::static_pointer_cast<std::vector<T>>(m_components[componentIndex.value()].data)->at(entityInd);

        // this will result in a segmentation fault if the entity doesn't have the component -> this is the desired behaviour 
    }

    template<typename T>
    std::vector<Entity> getEntities()
    {
        std::optional<size_t> componentIndex = getComponentIndex<T>();

        if (componentIndex.has_value())
            return m_components[componentIndex.value()].entities;
        
        return {};
    }

    template<typename T>
    void removeComponent(Entity entity)
    {
        std::optional<size_t> componentIndex = getComponentIndex<T>();

        if (componentIndex.has_value())
        {
            ComponentList& componentList = m_components[componentIndex.value()];

            for (size_t entityInd = 0; entityInd < componentList.entities.size(); entityInd++)
            {
                if (entity.id == componentList.entities[entityInd].id)
                {
                    componentList.entities.erase(componentList.entities.begin() + entityInd);

                    std::shared_ptr<std::vector<T>> dataPointer = std::static_pointer_cast<std::vector<T>>(componentList.data);
                    dataPointer->erase(dataPointer->begin() + entityInd);
                    break;
                }
                else if (entity.id > componentList.entities[entityInd].id)
                {
                    // entity not found - it doesn't have this component
                    break;
                }
            }
        }
    }

    template<typename T>
    void removeComponent()
    {
        std::optional<size_t> componentIndex = getComponentIndex<T>();

        if (componentIndex.has_value())
            m_components.erase(m_components.begin() + componentIndex.value());
    }

private:
    struct ComponentList
    {
    public:
        size_t typeID;
        std::vector<Entity> entities; // Chaches the entities that use it if you want to get all enitities that have a component

        std::shared_ptr<void> data; // this is a shared pointer to std::vector<T>, as the template type in unknown it's a void
    };

    std::vector<ComponentList> m_components;
    
    std::vector<Entity> m_entities;

private:
    template<typename T>
    std::optional<size_t> getComponentIndex()
    {
        size_t typeIdHash = typeid(T).hash_code();

        for (size_t compListInd = 0; compListInd < m_components.size(); compListInd++)
        {
            if (m_components[compListInd].typeID == typeIdHash)
                return compListInd;
        }

        return {};
    }
};
