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
            LE_PROFILE_FUNCTION();

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
            LE_PROFILE_FUNCTION();

            LE_ASSERT(entity >= 0); // Invalid entity!;

            mAvailableIDs.push_back(entity);
            --mEntityCount;
        }

        void Clear()
        {
            LE_PROFILE_FUNCTION();

            mAvailableIDs.clear();
            mAvailableIDs.emplace_back(0);
            mEntityCount = 0;
        }

        template<typename Func>
        void ForEach(Func func)
        {
            LE_PROFILE_FUNCTION();

            std::vector<EntityID> entities;
            GetActiveEntities(entities);

            for (EntityID entity : entities)
                func(entity);
        }

        void GetActiveEntities(std::vector<EntityID>& entities)
        {
            LE_PROFILE_FUNCTION();

            EntityID maxEntity = *std::max_element(mAvailableIDs.begin(), mAvailableIDs.end());
            entities.reserve(maxEntity);

            for (EntityID i = 0; i < maxEntity; i++)
                entities.emplace_back(i);

            entities.erase(std::remove_if(entities.begin(), entities.end(), [&, this](EntityID id)
            {
                return mAvailableIDs.find(id) == mAvailableIDs.end();
            }), entities.end());
        }

    private:
        PackedSet<EntityID> mAvailableIDs;
        usize mEntityCount = 0;

        friend class Registry;
    };
}