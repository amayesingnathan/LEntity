#include "EntityManager.h"

namespace LEnt {

	EntityID EntityManager::Create()
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

    void EntityManager::Destroy(EntityID entity)
    {
        LE_ASSERT(entity >= 0 && entity < MAX_ENTITIES); // Invalid entity!;

        mAvailableIDs.push_back(entity);
        --mEntityCount;
    }
}