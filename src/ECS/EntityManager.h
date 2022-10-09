#pragma once

#include "Core/ECSTypes.h"

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
        EntityID Create()
        {
            LE_ASSERT(mEntityCount < MAX_ENTITIES); // Max entities reached!

            usize size = mAvailableIDs.size();
            LE_ASSERT(size > 0); // Error! Available IDs was empty!

            EntityID nextID;
            if (size > 1)
            {
                nextID = mAvailableIDs.back();
                mAvailableIDs.pop_back();
            }
            else
                nextID = mAvailableIDs[0]++;

            ++mEntityCount;
            return nextID;
        }

        void Destroy(EntityID entity)
        {
            LE_ASSERT(entity >= 0 && entity < MAX_ENTITIES); // Invalid entity!;

            mAvailableIDs.push_back(entity);
            --mEntityCount;
        }

    private:
        std::vector<EntityID> mAvailableIDs;
        usize mEntityCount = 0;

        friend class Registry;
    };
}