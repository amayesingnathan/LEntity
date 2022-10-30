#pragma once

#include "Core/ECSTypes.h"
#include "Containers/PackedSet.h"
#include "Containers/SparseSet.h"

namespace LEnt {
    
    class IComponentPool
    {
    public:
        virtual ~IComponentPool() = default;
        virtual void tryDestroy(EntityID entity) = 0;
        virtual bool exists(EntityID entity) const = 0;
        virtual usize size() const = 0;
        virtual void clear() = 0;

        virtual PackedSet<EntityID>::Iterator begin() = 0;
        virtual PackedSet<EntityID>::Iterator end() = 0;
    };
    
    template<typename T>
    class ComponentPool : public IComponentPool
    {
    public:
        ComponentPool() = default;
    private:
        template<typename... Args>
        T& insert(EntityID entity, Args&&... args)
        {
            LE_ASSERT(entity >= 0);
            LE_ASSERT(!exists(entity));

            return AddInternal(entity, std::forward<Args>(args)...);
        }
        template<typename... Args>
        T& insert_or_replace(EntityID entity, Args&&... args)
        {
            LE_ASSERT(entity >= 0);

            return AddInternal(entity, std::forward<Args>(args)...);
        }
        template<typename... Args>
        T& replace(EntityID entity, Args&&... args)
        {
            LE_ASSERT(entity >= 0);
            LE_ASSERT(exists(entity));

            return AddInternal(entity, std::forward<Args>(args)...);
        }
        
        void tryDestroy(EntityID entity) override
        {
            LE_PROFILE_FUNCTION();

            if (entity < 0)
                return; // Invalid entity
            
            // Get index of entity to destroy in Entity/Component lists. 
            i32 destroyIndex = mEntityIndices[(usize)entity];
            if (destroyIndex < 0)
                return; // Entity does not have this component
            
            // Place hole for this entity in mEntityIndices
            usize destroyUIndex = (usize)destroyIndex;
            mEntityIndices[(usize)entity] = -1;
            
            // Overwrite the data of entity to destroy with data from end of Entity/Component lists.
            EntityID lastEntity = mEntityList.back();
            if (mEntityList[destroyUIndex] != lastEntity)
            {
                mEntityList[destroyUIndex] = lastEntity;
                mComponentList[destroyUIndex] = std::move(mComponentList.back());

                // Set the index of the newly moved entity to the overwritten data;
                mEntityIndices[(usize)lastEntity] = destroyIndex;
            }
            
            // Remove (now moved) data at end of lists.
            mEntityList.pop_back();
            mComponentList.pop_back();
        };

        T& get(EntityID entity) { return GetInternal(entity); }
        const T& get(EntityID entity) const { return GetInternal(entity); }

        bool exists(EntityID entity) const override
        {
            LE_PROFILE_FUNCTION();

            LE_ASSERT(entity >= 0, "Invalid entity!");
            return mEntityIndices[(usize)entity] >= 0;
        }

        usize size() const override { return mEntityList.size(); }

        void clear() override 
        {
            LE_PROFILE_FUNCTION();

            mEntityIndices.clear();
            mEntityList.clear();
            mComponentList.clear();
        }

        PackedSet<EntityID>::Iterator begin() override { return mEntityList.begin(); }
        PackedSet<EntityID>::Iterator end() override { return mEntityList.end(); }

    private:
        template<typename... Args>
        T& AddInternal(EntityID entity, Args&&... args)
        {
            LE_PROFILE_FUNCTION();

            mEntityIndices[(usize)entity] = (i32)mEntityList.size();
            mEntityList.push_back(entity);
            mComponentList.emplace_back(std::forward<Args>(args)...);
            return mComponentList.back();
        }

        T& GetInternal(EntityID entity) const
        {
            LE_PROFILE_FUNCTION();

            LE_ASSERT(entity >= 0, "Invalid entity!");

            i32 index = mEntityIndices[entity];
            LE_ASSERT(index >= 0, "Entity does not have component!");
            LE_ASSERT(index < mComponentList.size(), "Internal error! Returned index is greater than number of components in pool!");

            return mComponentList[index];
        }
    
    private:
        SparseSet<i32> mEntityIndices;
        PackedSet<EntityID> mEntityList;
        PackedSet<T> mComponentList;
        
        friend class ComponentManager;
        template<typename... T>
        friend class ComponentView;
    };
}