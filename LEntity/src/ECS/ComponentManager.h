#pragma once

#include "ComponentPool.h"

namespace LEnt {

    class ComponentManager
    {
    public:
        ComponentManager() = default;

    private:
        void ClearAll()
        {
            LE_PROFILE_FUNCTION();

            for (auto pool : mComponentPools)
                pool->clear();
        }

        template<typename... T>
        void Clear()
        {
            if constexpr (sizeof...(T) == 0)
            {
                ClearAll();
                return;
            }

            ([this]()
            {
                GetComponentPool<T>()->clear();
            }(), ...);
        }

        template<typename T>
        bool HasComponentPool() const { return mPoolIndices.count(Component<T>::Type) != 0; }

        template<typename T>
        void RegisterType() const
        {
            LE_ASSERT(!HasComponentPool<T>(), "Component type is already registered!");

            mPoolIndices.emplace(Component<T>::Type, mComponentPools.size());
            mComponentPools.push_back(std::make_shared<ComponentPool<T>>());
        }

        template<typename T, typename... Args>
        T& AddComponent(EntityID entity, Args&&... args)
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            std::shared_ptr<ComponentPool<T>> pool = GetComponentPool<T>();
            return pool->insert(entity, std::forward<Args>(args)...);
        }

        template<typename T, typename... Args>
        T& AddOrReplaceComponent(EntityID entity, Args&&... args)
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            std::shared_ptr<ComponentPool<T>> pool = GetComponentPool<T>();
            return pool->insert_or_replace(entity, std::forward<Args>(args)...);
        }

        template<typename T, typename... Args>
        T& ReplaceComponent(EntityID entity, Args&&... args)
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            std::shared_ptr<ComponentPool<T>> pool = GetComponentPool<T>();
            return pool->replace(entity, std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent(EntityID entity)
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            std::shared_ptr<ComponentPool<T>> pool = GetComponentPool<T>();
            return pool->get(entity);
        }
        template<typename T>
        const T& GetComponent(EntityID entity) const
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            std::shared_ptr<ComponentPool<T>> pool = GetComponentPool<T>();
            return pool->get(entity);
        }

        template<typename T>
        void RemoveComponent(EntityID entity)
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            std::shared_ptr<ComponentPool<T>> pool = GetComponentPool<T>();
            pool->tryDestroy(entity);
        }

        template<typename T>
        bool HasComponent(EntityID entity) const
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            std::shared_ptr<ComponentPool<T>> pool = GetComponentPool<T>();
            return pool->exists(entity);
        }

        template<typename T>
        std::shared_ptr<ComponentPool<T>> GetComponentPool() const
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            ComponentIndex poolIndex = mPoolIndices.at(Component<T>::Type);
            return std::dynamic_pointer_cast<ComponentPool<T>>(mComponentPools[poolIndex]);
        }

        template<typename... T>
        std::tuple<std::shared_ptr<ComponentPool<T>>...> GetComponentPools() const
        {
            return std::make_tuple<std::shared_ptr<ComponentPool<T>>...>(ToTupleElement<T>()...);
        }

        void EntityDestroyed(EntityID entity)
        {
            for (std::shared_ptr<IComponentPool> pool : mComponentPools)
                pool->tryDestroy(entity);
        };

        template<typename T>
        std::shared_ptr<ComponentPool<T>> ToTupleElement() const
        {
            if (!HasComponentPool<T>())
                RegisterType<T>();

            ComponentIndex poolIndex = mPoolIndices.at(Component<T>::Type);
            return std::dynamic_pointer_cast<ComponentPool<T>>(mComponentPools[poolIndex]);
        }

    private:
        using ComponentIndex = usize;

        mutable std::vector<std::shared_ptr<IComponentPool>> mComponentPools;
        mutable std::unordered_map<ComponentType, ComponentIndex> mPoolIndices;

        friend class Registry;
    };
}