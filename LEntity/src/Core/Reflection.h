#pragma once

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#    define LE_FUNC_SIG __PRETTY_FUNCTION__
#    define LE_FUNC_SIG_PREFIX '='
#    define LE_FUNC_SIG_SUFFIX ']'
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#    define LE_FUNC_SIG __PRETTY_FUNCTION__
#    define LE_FUNC_SIG_PREFIX '='
#    define LE_FUNC_SIG_SUFFIX ']'
#elif (defined(__FUNCSIG__) || (_MSC_VER))
#    define LE_FUNC_SIG __FUNCSIG__
#    define LE_FUNC_SIG_PREFIX '<'
#    define LE_FUNC_SIG_SUFFIX '>'
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#   define LE_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#   define LE_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#   define LE_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#   define LE_FUNC_SIG __func__
#else
#   define LE_FUNC_SIG "LE_FUNC_SIG unknown!"
#endif

namespace LEnt {

    namespace Reflection {
#if defined LE_FUNC_SIG_PREFIX
        template<typename Type>
        [[nodiscard]] constexpr auto GetLongName() noexcept
        {
            std::string_view pretty_function{ LE_FUNC_SIG };
            auto first = pretty_function.find_first_not_of(' ', pretty_function.find_first_of(LE_FUNC_SIG_PREFIX) + 1);
            auto value = pretty_function.substr(first, pretty_function.find_last_of(LE_FUNC_SIG_SUFFIX) - first);
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
#if defined LE_FUNC_SIG_PREFIX
        inline static constexpr auto Name = Reflection::GetName<T>();
#endif
    };
}
