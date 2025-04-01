#pragma once

#include <cstddef>
#include <type_traits>

namespace embed{

    template<template<class> class A>
    concept CAllocator = requires(A<int> a, std::size_t n, int* p) {
        { a.allocate(n) } -> std::same_as<int*>;
        { a.deallocate(p, n) };
    };


}// embed
