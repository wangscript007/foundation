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

#include "intern.h"
#include "memory/bump_system.h"
#include "memory/page_system.h"

namespace basecode::intern {
    static inline u32 index_size(u32);
    static b8 find_bucket(const u32* ids, u32 size, u32& bucket_index);
    static b8 find_key(const intern_t&, u64 hash, const string::slice_t& key, u32& bucket_index);

    static b8 find_key(
            const intern_t& pool,
            u64 hash,
            const string::slice_t& key,
            u32& bucket_index) {
        for (u32 i = bucket_index; i < pool.capacity; ++i) {
            if (pool.hashes[i] == 0) break;
            if (pool.hashes[i] == hash && pool.slices[i] == key) {
                bucket_index = i;
                return true;
            }
        }
        for (u32 i = 0; i < bucket_index; ++i) {
            if (pool.hashes[i] == 0) break;
            if (pool.hashes[i] == hash && pool.slices[i] == key) {
                bucket_index = i;
                return true;
            }
        }
        return false;
    }

    static inline u32 index_size(u32 capacity) {
        return (capacity * sizeof(u32))
               + alignof(u64) + (capacity * sizeof(u64))
               + alignof(string::slice_t) + (capacity * sizeof(string::slice_t));
    }

    static b8 find_bucket(const u32* ids, u32 capacity, u32& bucket_index) {
        for(u32 i = bucket_index; i < capacity; ++i) {
            if (ids[i] == 0) {
                bucket_index = i;
                return true;
            }
        }
        for(u32 i = 0; i < bucket_index; ++i) {
            if (ids[i] == 0) {
                bucket_index = i;
                return true;
            }
        }
        return false;
    }

    u0 free(intern_t& pool) {
        memory::system::free(pool.bump_alloc);
        memory::system::free(pool.page_alloc);
        memory::free(pool.alloc, pool.index);
        pool.index = {};
        pool.size = pool.capacity = {};
    }

    u0 reset(intern_t& pool) {
        memory::page::reset(pool.page_alloc);
        memory::bump::reset(pool.bump_alloc);
        pool.id = 1;
        pool.size = {};
        std::memset(pool.index, 0, index_size(pool.capacity));
    }

    intern_t make(alloc_t* alloc) {
        intern_t pool{};
        init(pool, alloc);
        return pool;
    }

    result_t get(intern_t& pool, u32 id) {
        for (u32 i = 0; i < pool.capacity; ++i) {
            if (pool.ids[i] == id) {
                return result_t{
                    .hash = pool.hashes[i],
                    .slice = pool.slices[i],
                    .id = pool.ids[i],
                    .status = status_t::ok,
                    .new_value = false
                };
            }
        }
        return result_t{
            .status = status_t::not_found
        };
    }

    u0 init(intern_t& pool, alloc_t* alloc, u32 num_pages) {
        pool.id = 1;
        pool.alloc = alloc;

        page_config_t page_config{};
        page_config.backing = pool.alloc;
        page_config.page_size = memory::system::os_page_size() * num_pages;
        pool.page_alloc = memory::system::make(alloc_type_t::page, &page_config);

        bump_config_t bump_config{};
        bump_config.type = bump_type_t::allocator;
        bump_config.backing.alloc = pool.page_alloc;
        pool.bump_alloc = memory::system::make(alloc_type_t::bump, &bump_config);
    }

    static status_t rehash(intern_t& pool, u32 new_capacity) {
        new_capacity = std::max<u32>(new_capacity, 16);

        auto new_data = (u8*) memory::alloc(pool.alloc, index_size(new_capacity), alignof(u32));
        u32 adjust{};
        auto ids    = (u32*)               new_data;
        auto hashes = (u64*)               memory::system::align_forward(ids    + new_capacity, alignof(u64), adjust);
        auto slices = (string::slice_t*)   memory::system::align_forward(hashes + new_capacity, alignof(string::slice_t), adjust);
        std::memset(ids, 0, new_capacity * sizeof(u32));

        for (u32 i = 0; i < pool.capacity; ++i) {
            if (pool.hashes[i] == 0)
                continue;

            u32 bucket_index = pool.hashes[i] % new_capacity;
            if (!find_bucket(ids, new_capacity, bucket_index))
                return status_t::no_bucket;

            ids[bucket_index] = pool.ids[i];
            hashes[bucket_index] = pool.hashes[i];
            slices[bucket_index] = pool.slices[i];
        }
        memory::free(pool.alloc, pool.index);
        pool.index = new_data;
        pool.ids = ids;
        pool.hashes = hashes;
        pool.slices = slices;
        pool.capacity = new_capacity;

        return status_t::ok;
    }

    result_t intern(intern_t& pool, string::slice_t value) {
        if (!pool.index || pool.size * 3 > pool.capacity * 2) {
            auto status = rehash(pool, pool.size * 3);
            if (!OK(status))
                return result_t{.status = status, .new_value = false};
        }

        u64 hash = hashing::hash64(value);
        u32 bucket_index = hash % pool.capacity;

        if (find_key(pool, hash, value, bucket_index)) {
            return result_t{
                .hash = pool.hashes[bucket_index],
                .slice = pool.slices[bucket_index],
                .id = pool.ids[bucket_index],
                .status = status_t::ok,
                .new_value = false
            };
        }

        if (!find_bucket(pool.ids, pool.capacity, bucket_index))
            return result_t{.status = status_t::no_bucket, .new_value = false};

        auto data_ptr = (u8*) memory::alloc(pool.bump_alloc, value.length + 1);
        std::memcpy(data_ptr, value.data, value.length);
        data_ptr[value.length] = '\0';

        pool.ids[bucket_index] = pool.id++;
        pool.hashes[bucket_index] = hash;

        auto& slice = pool.slices[bucket_index];
        slice.data = data_ptr;
        slice.length = value.length;

        ++pool.size;

        return result_t{
            .hash = pool.hashes[bucket_index],
            .slice = pool.slices[bucket_index],
            .id = pool.ids[bucket_index],
            .status = status_t::ok,
            .new_value = true
        };
    }
}
