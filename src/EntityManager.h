#pragma once

#include "ECSTypes.h"

namespace LEnt {

    class EntityManager
    {
    public:
        EntityManager()
        {
            mAvailableIDs.reserve(MAX_ENTITIES);
            mAvailableIDs.emplace_back(0);
        }

    private:
        EntityID Create();
        void Destroy(EntityID entity);

    private:
        std::vector<EntityID> mAvailableIDs;
        usize mEntityCount = 0;

        friend class Registry;
    };
}