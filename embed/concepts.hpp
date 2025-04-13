#pragma once

#include <type_traits>

template<class T>
concept CStringView = requires(T t) {
    t.data();
    t.size();
};