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

#include <basecode/core/types.h>
#include <basecode/core/array.h>
#include <basecode/core/slice.h>
#include <basecode/core/hashtab.h>
#include <basecode/core/assoc_array.h>

namespace basecode {
    struct intern_t final {
        alloc_t*                        alloc;
        u32*                            ids;
        u64*                            hashes;
        assoc_array_t<u32>              buf;
        u32                             size;
        u32                             capacity;
        f32                             load_factor;
    };
    static_assert(sizeof(intern_t) <= 88, "intern_t is now larger than 88 bytes!");

    namespace intern {
        enum class status_t : u8 {
            ok,
            no_bucket,
            not_found,
        };

        struct result_t final {
            u64                         hash;
            str::slice_t                slice;
            u32                         id;
            status_t                    status;
            b8                          new_value;
        };

        u0 free(intern_t& pool);

        u0 reset(intern_t& pool);

        result_t get(intern_t& pool, u32 id);

        str::slice_t status_name(status_t status);

        intern_t make(alloc_t* alloc = context::top()->alloc);

        result_t intern(intern_t& pool, const str::slice_t& value);

        u0 reserve(intern_t& pool, u32 key_capacity, u32 value_capacity);

        u0 init(intern_t& pool, alloc_t* alloc = context::top()->alloc, f32 load_factor = .5f);
    }
}

