#pragma once

#include "Writeable.hpp"

namespace fiber
{
    class iPin : public iWritable<bool>{};

    template<class Pin>
    concept cPin = cWritable<Pin, bool>;
} // namespace fiber
