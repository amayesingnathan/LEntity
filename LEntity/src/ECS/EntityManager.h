#pragma once

#include "Core/ECSTypes.h"
#include "Containers/PackedSet.h"

namespace LEnt {

    class EntityManager
    {
    public:
        EntityManager()
        {
            mAvailableIDs.emplace_back(0);
        }

    private:
        EntityID Create()
        {
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
            LE_ASSERT(entity >= 0); // Invalid entity!;

            mAvailableIDs.push_back(entity);
            --mEntityCount;
        }

    private:
        PackedSet<EntityID> mAvailableIDs;
        usize mEntityCount = 0;

        friend class Registry;
    };
}