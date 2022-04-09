#include "EntityComponent.hpp"

Entity Registry::createEntity()
{
    if (m_entities.size() == 0)
    {
        m_entities.push_back({0});
        return {0};
    }
    else
    {
        m_entities.push_back(m_entities[m_entities.size() - 1].id + 1);
        return {m_entities[m_entities.size() - 1].id};
    }
}