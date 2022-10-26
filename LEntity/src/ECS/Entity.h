#include "Registry.h"

namespace LEnt {

    class Entity
    {
    public:
        Entity() = default;
        Entity(Registry& registry, bool create = false)
            : mID(NullEnt), mRegistry(&registry)
        {
            if (create)
                mID = mRegistry->create();
        }

        Entity(EntityID id, Registry& registry)
            : mID(id), mRegistry(&registry) {}

        template<typename T, typename... Args>
        T& addComponent(Args&&... args) { return mRegistry->emplace<T>(mID, std::forward<Args>(args)...); }

        template<typename T, typename... Args>
        T& replaceComponent(Args&&... args) { return mRegistry->emplace<T>(mID, std::forward<Args>(args)...); }

        template<typename T, typename... Args>
        T& addOrReplaceComponent(Args&&... args) { return mRegistry->emplace_or_replace<T>(mID, std::forward<Args>(args)...); }

        template<typename T>
        void removeComponent() { return mRegistry->erase<T>(mID); }

        template<typename T>
        T& getComponent() { return mRegistry->get<T>(mID); }
        template<typename T>
        const T& getComponent() const { return mRegistry->get<T>(mID); }

        template<typename T>
        bool hasComponent() const { mRegistry->all_of<T>(mID); }

        void destroy() { mRegistry->destroy(mID); }

    public:
        operator bool() const { return mID != NullEnt && mRegistry; }

        bool operator==(const Entity& other) const
        {
            return mID == other.mID && mRegistry == other.mRegistry;
        }
        bool operator!=(const Entity& other) const
        {
            return !(*this == other);
        }

    private:
        EntityID mID = NullEnt;
        Registry* mRegistry = nullptr;
    };
}