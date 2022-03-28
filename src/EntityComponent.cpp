#include "EntityComponent.hpp"

Entity Registry::createEntity()
{
    if (entities.size() == 0)
    {
        entities.push_back({0});
        return {0};
    }
    else
    {
        entities.push_back(entities[entities.size() - 1].id + 1);
        return {entities[entities.size() - 1].id};
    }
}