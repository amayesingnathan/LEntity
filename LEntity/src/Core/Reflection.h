#pragma once

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#    define LE_FUNC_SIGNATURE __PRETTY_FUNCTION__
#    define LE_FUNC_SIGNATURE_PREFIX '='
#    define LE_FUNC_SIGNATURE_SUFFIX ']'
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#    define LE_FUNC_SIGNATURE __PRETTY_FUNCTION__
#    define LE_FUNC_SIGNATURE_PREFIX '='
#    define LE_FUNC_SIGNATURE_SUFFIX ']'
#elif (defined(__FUNCSIG__) || (_MSC_VER))
#    define LE_FUNC_SIGNATURE __FUNCSIG__
#    define LE_FUNC_SIGNATURE_PREFIX '<'
#    define LE_FUNC_SIGNATURE_SUFFIX '>'
#endif

namespace LEnt {

    namespace Reflection {
#if defined LE_FUNC_SIGNATURE_PREFIX
        template<typename Type>
        [[nodiscard]] constexpr auto GetLongName() noexcept
        {
            std::string_view pretty_function{ LE_FUNC_SIGNATURE };
            auto first = pretty_function.find_first_not_of(' ', pretty_function.find_first_of(LE_FUNC_SIGNATURE_PREFIX) + 1);
            auto value = pretty_function.substr(first, pretty_function.find_last_of(LE_FUNC_SIGNATURE_SUFFIX) - first);
            return value;
        }

        template<typename Type>
        [[nodiscard]] constexpr auto GetName() noexcept
        {
            std::string_view long_name = GetLongName<Type>();
            auto first = long_name.find_last_of("::");
            if (first == std::string_view::npos)
                first = long_name.find_last_of(' ');
            else
                first++;
            if (first == std::string_view::npos)
                return long_name;
            return long_name.substr(first, long_name.length() - first);
        }
#endif
    }

    template<typename T>
    struct TypeInfo
    {
#if defined LE_FUNC_SIGNATURE_PREFIX
        inline static constexpr auto Name = Reflection::GetName<T>();
#endif
    };
}
