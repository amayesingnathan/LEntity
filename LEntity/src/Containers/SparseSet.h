#pragma once

#include "Core/ECSTypes.h"

namespace LEnt {

    template<typename T>
    class SparseSet
    {
    private:
        static constexpr usize ElementsPerPage = 4096;
        static constexpr usize PageSize = ElementsPerPage * sizeof(T);

    public:
        SparseSet() = default;
        ~SparseSet()
        {
            clear();
        }

        void clear()
        {
            LE_PROFILE_FUNCTION();

            for (auto& [index, page] : mPages)
                ::operator delete(page);

            mPages.clear();
        }

        void reset()
        {
            LE_PROFILE_FUNCTION();

            for (auto& [pageIndex, memBlock] : mPages)
                memset(memBlock, 0xff, PageSize);
        }

    public:
        T& operator[](usize index) { return GetElement(index); }
        T& operator[](EntityID index) { return GetElement((usize)index); }
        const T& operator[](usize index) const { return GetElement(index); }
        const T& operator[](EntityID index) const { return GetElement((usize)index); }

    private:
        T& GetElement(usize index) const
        {
            LE_PROFILE_FUNCTION();

            usize pageIndex = index / ElementsPerPage;
            usize localIndex = index - (ElementsPerPage * pageIndex);

            if (pageIndex >= mPages.size())
                AddPage(pageIndex);

            T* page = mPages[pageIndex];
            LE_ASSERT(page, "Page is not valid memory!");

            T* target = page + localIndex;
            LE_ASSERT(target, "Located index is not valid memory!");

            return *target;
        }

        void AddPage(usize pageIndex) const
        {
            LE_PROFILE_FUNCTION();

            auto result = mPages.try_emplace(pageIndex, nullptr);
            LE_ASSERT(result.second, "Failed to add new page");

            result.first->second = (T*)::operator new(PageSize);
            memset(result.first->second, 0xff, PageSize);
        }

    private:
        using PageMap = std::unordered_map<usize, T*>;
        mutable PageMap mPages;
    };
}