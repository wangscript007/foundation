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

#include <catch2/catch.hpp>
#include <basecode/core/str.h>
#include <basecode/core/defer.h>
#include <basecode/core/buffer.h>
#include <basecode/core/hashtab.h>
#include <basecode/core/stopwatch.h>
#include <basecode/core/slice_utils.h>
#include "test.h"

using namespace basecode;

TEST_CASE("basecode::hashtab names") {
    hashtab_t<str::slice_t, baby_name_t> table{};
    hashtab::init(table, context::top()->alloc, 0.7f);
    defer(hashtab::free(table));

    //hashtab::reserve(table, 4661);

    auto buf = buffer::make();
    REQUIRE(OK(buffer::load((str_t) "../etc/ut.txt"_ss, buf)));
    defer(buffer::free(buf));

    array_t<name_record_t> records{};
    array::init(records);
    defer(array::free(records));

    buffer::each_line(
        buf,
        [&records](str::slice_t line) {
            auto& record = array::append(records);
            slice::to_fields(line, record.fields);
            return true;
        });

    stopwatch_t emplace_time{};
    stopwatch::start(emplace_time);

    for (const auto& rec : records) {
        auto name = hashtab::find(table, rec.fields[3]);
        if (name) continue;
        name = hashtab::emplace(table, rec.fields[3]);
        name->sex = rec.fields[1][0];
        std::memcpy(name->year, rec.fields[2].data, 4);
        name->year[4] = '\0';
        std::memcpy(name->state, rec.fields[0].data, 2);
        name->state[2] = '\0';
    }

    format::print("table.size = {}, table.capcaity = {}\n", table.size, table.capacity);
    stopwatch::stop(emplace_time);
    stopwatch::print_elapsed("total hashtab emplace time"_ss, 40,
                             stopwatch::elapsed(emplace_time));
}

TEST_CASE("basecode::hashtab payload with random string keys") {
    hashtab_t<str::slice_t, payload_t> table{};
    hashtab::init(table, context::top()->alloc, .65f);
    defer(hashtab::free(table));
    //hashtab::reserve(table, 1024);

    str_t str[4096];
    for (u32 i = 0; i < 4096; ++i) {
        str::init(str[i], context::top()->alloc);
        str::reserve(str[i], 32);
        str[i] = str::random(32);
    }

    stopwatch_t time{};
    stopwatch::start(time);

    for (u32 i = 0; i < 4096; ++i) {
        auto payload = hashtab::emplace(table, slice::make(str[i]));
        payload->ptr = {};
        payload->offset = i * 100;
    }

    stopwatch::stop(time);
    stopwatch::print_elapsed("hashtab payload + string key"_ss, 40, stopwatch::elapsed(time));

//    auto keys = hashtab::keys(table);
//    defer(array::free(keys));
//    for (auto key : keys) {
//        format::print("key = {}\n", *key);
//    }
}

TEST_CASE("basecode::hashtab payload with integer keys") {
    hashtab_t<u32, payload_t> table{};
    hashtab::init(table, context::top()->alloc, .65f);
    defer(hashtab::free(table));
    //hashtab::reserve(table, 1024);

    stopwatch_t time{};
    stopwatch::start(time);

    for (u32 i = 0; i < 4096; ++i) {
        auto payload = hashtab::emplace(table, i * 4096);
        payload->ptr = {};
        payload->offset = i * 100;
    }

    stopwatch::stop(time);
    stopwatch::print_elapsed("hashtab payload + int key"_ss, 40, stopwatch::elapsed(time));

//    auto keys = hashtab::keys(table);
//    defer(array::free(keys));
//    for (auto key : keys) {
//        format::print("key = {}\n", *key);
//    }
}

TEST_CASE("basecode::hashtab basics") {
    stopwatch_t time{};
    stopwatch::start(time);

    hashtab_t<s32, str::slice_t> table{};
    hashtab::init(table);
    defer(hashtab::free(table));

    const auto one = "one"_ss;
    const auto two = "two"_ss;
    const auto three = "three"_ss;
    const auto four = "four"_ss;
    const auto five = "five"_ss;
    const auto six = "six"_ss;
    const auto seven = "seven"_ss;

    hashtab::insert(table, 1, one);
    hashtab::insert(table, 2, two);
    hashtab::insert(table, 3, three);
    hashtab::insert(table, 4, four);
    hashtab::insert(table, 5, five);
    hashtab::insert(table, 6, six);
    hashtab::insert(table, 7, seven);
    REQUIRE(table.size == 7);
    REQUIRE(table.capacity == 32);

    str::slice_t* s;

    s = hashtab::find(table, 5);
    REQUIRE(s);
    REQUIRE(*s == five);

    s = hashtab::find(table, 1);
    REQUIRE(s);
    REQUIRE(*s == one);

    s = hashtab::find(table, 7);
    REQUIRE(s);
    REQUIRE(*s == seven);

    s = hashtab::find(table, 3);
    REQUIRE(s);
    REQUIRE(*s == three);

    s = hashtab::find(table, 6);
    REQUIRE(s);
    REQUIRE(*s == six);

    s = hashtab::find(table, 2);
    REQUIRE(s);
    REQUIRE(*s == two);

    s = hashtab::find(table, 4);
    REQUIRE(s);
    REQUIRE(*s == four);

    stopwatch::stop(time);
    stopwatch::print_elapsed("hashtab insert + find"_ss, 40, stopwatch::elapsed(time));
}
