#pragma once

#include "Writeable.hpp"

namespace embed
{
    class iPin : public iWritable<bool>{};

    template<class Pin>
    concept cPin = cWritable<Pin, bool>;
} // namespace embed
