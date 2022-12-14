#pragma once

#include "ComponentPool.h"

namespace LEnt {

	// Using a view when changes were made to the registry after instantiation of the view is undefined behaviour. 
	template<typename... T>
	class ComponentView
	{
	public:
		using ComponentList = TypeList<T...>;
		using ComponentTuple  = typename ComponentList::TupleType;
		using PoolTuple = std::tuple<std::shared_ptr<ComponentPool<T>>...>;

	public:
		ComponentView(const PoolTuple& pools)
			: mPools(pools)
		{
			BuildEntityList();
		}

		void each(std::function<void(EntityID, T&...)> function)
		{
			LE_PROFILE_FUNCTION();

			for (EntityID entity : mValidEntities)
			{
				// Create tuple of components and unpack into function
				function(entity, std::get<T&>(GenTuple(entity))...);
			}
		}

		template<typename U>
		U& get(EntityID entity)
		{
			LE_PROFILE_FUNCTION();

			LE_STATIC_ASSERT(ComponentList::Contains<U>, "Template argument is not a component type in this view!");
			LE_ASSERT(Contains(entity), "View does not contain this entity!");

			std::shared_ptr<ComponentPool<U>> pool = std::get<ComponentList::Index<U>>(mPools);
			return pool->get(entity);
		}

		usize size() const { return mValidEntities.size(); }

		auto begin() { return mValidEntities.begin(); }
		auto begin() const { return mValidEntities.cbegin(); }
		auto end() { return mValidEntities.end(); }
		auto end() const { return mValidEntities.cend(); }

	private:
		bool Contains(EntityID entity) const
		{
			LE_PROFILE_FUNCTION();

			return std::find(mValidEntities.begin(), mValidEntities.end(), entity) != mValidEntities.end();
		}

		void BuildEntityList()
		{
			LE_PROFILE_FUNCTION();

			mValidEntities.clear();
			std::shared_ptr<IComponentPool> smallestPool = GetSmallest();

			for (EntityID entity : *smallestPool)
			{
				if (ValidEntity(entity))
					mValidEntities.push_back(entity);
			}
		}

		std::shared_ptr<IComponentPool> GetSmallestImpl(std::shared_ptr<ComponentPool<T>>... pools)
		{
			LE_PROFILE_FUNCTION();

			usize finalPoolSize = -1;
			std::shared_ptr<IComponentPool> pool = nullptr;
			([&, this]
			{
				usize poolSize = pools->size();
				if (poolSize < finalPoolSize)
				{
					pool = pools;
					finalPoolSize = poolSize;
				}
			} (), ...);

			return pool;
		}
		std::shared_ptr<IComponentPool> GetSmallest()
		{
			LE_PROFILE_FUNCTION();

			return std::apply(&ComponentView<T...>::GetSmallestImpl, std::tuple_cat(std::make_tuple(this), mPools));
		}

		bool ValidEntityImpl(std::shared_ptr<ComponentPool<T>>... pools)
		{
			LE_PROFILE_FUNCTION();

			bool exists = true;
			([&, this]
			{
				if (!pools->exists(mCheckEntity))
					exists = false;
			} (), ...);

			return exists;
		}
		bool ValidEntity(EntityID entity)
		{
			LE_PROFILE_FUNCTION();

			mCheckEntity = entity;
			return std::apply(&ComponentView<T...>::ValidEntityImpl, std::tuple_cat(std::make_tuple(this), mPools));
		}

		template<usize index>
		auto& ToTupleElement(EntityID entity)
		{
			LE_PROFILE_FUNCTION();

			using ElementType = typename ComponentList::Type<index>;
			std::shared_ptr<ComponentPool<ElementType>> pool = std::get<index>(mPools);
			return pool->get(entity);
		}

		template <usize... Is>
		auto GenTupleImpl(EntityID entity, std::index_sequence<Is...>) 
		{
			LE_PROFILE_FUNCTION();

			return std::tie(ToTupleElement<Is>(entity)...); 
		}

		auto GenTuple(EntityID entity) 
		{
			LE_PROFILE_FUNCTION();

			return GenTupleImpl(entity, std::make_index_sequence<ComponentList::Size>{}); 
		}

	private:
		PoolTuple mPools;
		std::vector<EntityID> mValidEntities;
		EntityID mCheckEntity;
	};

}