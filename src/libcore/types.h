// ----------------------------------------------------------------------------
// ____                               _
// |  _\                             | |
// | |_)| __ _ ___  ___  ___ ___   __| | ___ TM
// |  _< / _` / __|/ _ \/ __/ _ \ / _` |/ _ \
// | |_)| (_| \__ \  __/ (_| (_) | (_| |  __/
// |____/\__,_|___/\___|\___\___/ \__,_|\___|
//
//      F O U N D A T I O N   P R O J E C T
//
// Copyright (C) 2020 Jeff Panici
// All rights reserved.
//
// This software source file is licensed under the terms of MIT license.
// For details, please read the LICENSE file.
//
// ----------------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <cstddef>

#if defined(__GNUC__)
#   define force_inline inline __attribute__((always_inline, unused))
#   define never_inline inline __attribute__((noinline, unused))
#   ifndef likely
#       define likely(x) __builtin_expect(!!(x), 1)
#   endif
#   ifndef unlikely
#       define unlikely(x) __builtin_expect(!!(x), 0)
#   endif
#elif defined(_MSC_VER)
#   define force_inline __forceinline
#   define never_inline __declspec(noinline)
#   ifndef likely
#       define likely(x) x
#   endif
#   ifndef unlikely
#       define unlikely(x) x
#   endif
#else
#   define force_inline inline
#   define never_inline
#   ifndef likely
#       define likely(x) x
#   endif
#   ifndef unlikely
#       define unlikely(x) x
#   endif
#endif

#define OK(x) (0 == (u32) x)
#define SAFE_SCOPE(x) do { x } while(false)

namespace basecode {
    using u0    = void;
    using u8    = uint8_t;
    using u16   = uint16_t;
    using u32   = uint32_t;
    using u64   = uint64_t;
    using s8    = char;
    using s16   = int16_t;
    using s32   = int32_t;
    using s64   = int64_t;
    using b8    = bool;
    using f32   = float;
    using f64   = double;
    using s128  = __int128_t;
    using u128  = __uint128_t;
    using usize = std::size_t;

    static inline u32 next_power_of_two(u32 n) {
        --n;
        n |= n >> (u32) 1;
        n |= n >> (u32) 2;
        n |= n >> (u32) 4;
        n |= n >> (u32) 8;
        n |= n >> (u32) 16;
        ++n;
        return n;
    }

    static inline u64 align(u64 value, u64 align) {
        const u64 mod = value % align;
        if (mod) value += (align - mod);
        return value;
    }

    static inline bool is_power_of_two(int64_t x) {
        if (x <= 0)
            return false;
        return !(x & (x-1));
    }
}
