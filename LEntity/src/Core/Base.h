#pragma once

#include "lepch.h"

#include "Profiling.h"

#define LE_ASSERT(x, ...) assert(x)
#define LE_STATIC_ASSERT(...) static_assert(__VA_ARGS__)
