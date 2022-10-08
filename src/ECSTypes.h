#pragma once

#include "Base.h"
#include "Reflection.h"

#include <string_view>

namespace LEnt {

    using EntityID = i16;
    constexpr EntityID MAX_ENTITIES = Limits::i16Max;
    constexpr EntityID NullEnt = -1;

    using ComponentType = std::string_view;

    template<typename T>
    struct Component
    {
        static constexpr ComponentType Type = TypeInfo<T>::Name;
    };

    namespace TypeUtils {

        template<usize I, typename T, typename TupleType>
        inline static constexpr usize IndexFunction()
        {
            LE_STATIC_ASSERT(I < std::tuple_size<TupleType>::value, "The element is not in the tuple");

            using IndexType = typename std::tuple_element<I, TupleType>::type;
            if constexpr (std::is_same<IndexType, T>::value) {
                return I;
            }
            else {
                return IndexFunction<I + 1, T, TupleType>();
            }
        }
    }

    template<typename... Types>
    struct TypeList
    {
        static constexpr usize Size = sizeof...(Types);

        using TupleType = std::tuple<Types...>;
        
        template<typename T>
        inline static constexpr bool Contains = std::disjunction<std::is_same<T, Types>...>::value;

        template<typename T>
        inline static constexpr usize Index = TypeUtils::IndexFunction<0, T, TupleType>();

        template<usize I>
        using Type = typename std::tuple_element<I, TupleType>::type;

    };
}