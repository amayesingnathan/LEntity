#pragma once

#include <memory>

#include <cstdint>
#include <cassert>

namespace LEnt {

#define LE_ASSERT(condition) assert(condition);
#define LE_STATIC_ASSERT(...) static_assert(__VA_ARGS__);

	using i8 = signed char;
	using i16 = short;
	using i32 = int;
	using i64 = long long;

	using u8 = unsigned char;
	using u16 = unsigned short;
	using u32 = unsigned int;
	using u64 = unsigned long long;

	using usize = size_t;

	using f32 = float;
	using f64 = double;

	struct Limits
	{
		static constexpr i32 i8Min = -127;
		static constexpr i32 i8Max = -127;
		static constexpr i32 i16Min = -32768;
		static constexpr i32 i16Max = 32767;
		static constexpr i32 u8Max = 255;
		static constexpr i32 u16Max = 65535;
	};

	template<typename T>
	using Single = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr inline Single<T> CreateSingle(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
}