#pragma once

#include "Core/ECSTypes.h"

namespace LEnt {

    template<typename PackedSet>
    class PackedSetIterator
    {
    public:
        using TypeVal = typename PackedSet::TypeVal;
        using TypeRef = typename PackedSet::TypeRef;
        using TypePtr = typename PackedSet::TypePtr;

        using PagePtr = typename PackedSet::Page*;

    private:
        static constexpr usize PageLen = PackedSet::ElementsPerPage;

    public:
        PackedSetIterator(TypePtr ptr, PagePtr page)
            : mPtr(ptr), mPage(page) {}

        PackedSetIterator& operator++()
        {
            usize traversed = mPtr - mPage->memBlock;
            if (traversed < PageLen - 1)
            {
                mPtr++;
                return *this;
            }

            mPage++;
            mPtr = mPage->memBlock;
            return *this;
        }
        PackedSetIterator operator++(int)
        {
            PackedSetIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        PackedSetIterator& operator--()
        {
            usize traversed = mPtr - mPage->memBlock;
            if (traversed > 0)
            {
                mPtr--;
                return *this;
            }

            mPage--;
            mPtr = mPage->memBlock + (PageLen - 1);
            return *this;
        }
        PackedSetIterator operator--(int)
        {
            PackedSetIterator tmp = *this;
            --(*this);
            return tmp;
        }

        PackedSetIterator& operator+=(usize offset)
        {
            usize traversed = mPtr - mPage->memBlock;
            while (traversed + offset > PageLen)
            {
                mPage++;
                offset -= PageLen;
            }

            mPtr = mPage->memBlock + traversed + offset;
            return *this;
        }
        PackedSetIterator operator+(usize offset) const
        {
            PackedSetIterator it = *this;
            it += offset;
            returh it;
        }
        PackedSetIterator operator-=(usize offset)
        {
            usize traversed = mPtr - mPage->memBlock;
            while (traversed > offset)
            {
                mPage--;
                offset -= PageLen;
            }

            mPtr = mPage->memBlock + traversed - offset;
            return *this;
        }
        PackedSetIterator operator-(usize offset) const
        {
            PackedSetIterator it = *this;
            it -= offset;
            return it;
        }

        usize operator-(const PackedSetIterator& rhs) const
        {
            LE_ASSERT(mPage >= rhs.mPage);
            if (mPage == rhs.mPage)
                LE_ASSERT(mPtr >= rhs.mPtr);

            usize distance = 0;
            distance += (mPage - rhs.mPage) * PageLen;
            distance += (mPtr - rhs.mPtr);
            return distance;
        }

        TypeRef operator[](usize index)
        {
            PackedSetIterator it = *this;
            it += index;
            return *(it.mPtr);
        }

        TypePtr operator->()
        {
            return mPtr;
        }

        TypeRef operator*()
        {
            return *mPtr;
        }

        bool operator==(const PackedSetIterator& rhs) const
        {
            return mPtr == rhs.mPtr;
        }
        bool operator!=(const PackedSetIterator& rhs) const
        {
            return !(*this == rhs);
        }

        operator TypePtr() { return mPtr; }

    private:
        PackedSetIterator& Null()
        {
            mPtr = nullptr;
            mPage = nullptr;
            return *this;
        }

    private:
        TypePtr mPtr;
        PagePtr mPage;
    };

    template<typename T>
    class PackedSet
    {
    public:
        using SetType = PackedSet<T>;
        using Iterator = PackedSetIterator<SetType>;
        using TypeVal = T;
        using TypeRef = T&;
        using TypePtr = T*;

        struct Page
        {
            TypePtr memBlock = nullptr;
            usize used = 0;
        };

    public:
        static constexpr usize PageSize = 1024;
        static constexpr usize ElementsPerPage = PageSize / sizeof(TypeVal);

    public:
        PackedSet() = default;
        ~PackedSet()
        {
            clear();
        }

    public:
        template<typename... Args>
        TypeRef emplace_back(Args&&... args)
        {
            if (mSize == 0)
                AddPage();

            usize pageIndex = mSize / ElementsPerPage;
            LE_ASSERT(pageIndex <= mPages.size(), "No page for this index!");

            Page& page = mPages[pageIndex];
            LE_ASSERT(page.memBlock, "Page is not valid memory!");
            LE_ASSERT(page.used < ElementsPerPage, "Page has no room for new entries!");

            TypePtr target = page.memBlock + page.used;
            new (target) T(std::forward<Args>(args)...);

            mSize++;
            page.used++;

            if (page.used == ElementsPerPage)
                AddPage();

            return *target;
        }

        void push_back(const TypeRef val) { emplace_back(val); }

        void pop_back()
        {
            usize pageIndex = (mSize - 1) / ElementsPerPage;

            Page& page = mPages[pageIndex];
            LE_ASSERT(page.memBlock, "Page is not valid memory!");
            LE_ASSERT(page.used > 0, "Page has no entries to pop!");

            mSize--;
            page.used--;

            TypePtr target = page.memBlock + page.used;
            target->~T();
        }

        void clear()
        {
            for (Page& page : mPages)
                ::operator delete(page.memBlock);

            mPages.clear();
            mSize = 0;
        }

        usize size() const { return mSize; }

        TypeRef operator[](usize index) { return GetElement(index); }
        TypeRef operator[](i32 index) { return GetElement((usize)index); }
        const TypeRef operator[](usize index) const { return GetElement(index); }
        const TypeRef operator[](i32 index) const { return GetElement((usize)index); }

        TypeRef back() { return GetElement(mSize - 1); }
        const TypeRef back() const { return GetElement(mSize - 1); }

        Iterator begin()
        {
            if (mSize == 0)
                return end();

            Page& firstPage = mPages[0];
            return Iterator(firstPage.memBlock, &firstPage);
        }
        const Iterator begin() const
        {
            if (mSize == 0)
                return end();

            Page& firstPage = mPages[0];
            return Iterator(firstPage.memBlock, &firstPage);
        }
        Iterator end()
        {
            Page& endPage = mPages.back();
            return Iterator(endPage.memBlock + endPage.used, &endPage);
        }
        const Iterator end() const
        {
            const Page& endPage = mPages.back();
            return Iterator(endPage.memBlock + endPage.used, &endPage);
        }

        Iterator find(Iterator begin, Iterator end, const TypeRef val)
        {
            for (Iterator it = begin; it != end; ++it)
            {
                if (*it == val)
                    return it;
            }
            return end;
        }
        Iterator find(const TypeRef val) { return find(begin(), end(), val); }

    private:
        void AddPage()
        {
            Page& newPage = mPages.emplace_back();
            newPage.memBlock = (TypePtr)::operator new(PageSize);
        }

        TypeRef GetElement(usize index) const
        {
            usize pageIndex = index / ElementsPerPage;
            usize localIndex = index - (ElementsPerPage * pageIndex);

            LE_ASSERT(pageIndex < mPages.size(), "No page for this index!");

            const Page& page = mPages[pageIndex];
            LE_ASSERT(page.memBlock, "Page is not valid memory!");

            TypePtr target = page.memBlock + localIndex;
            LE_ASSERT(target, "Located index is not valid memory!");

            return *target;
        }

    private:
        std::vector<Page> mPages;
        usize mSize = 0;
    };
}