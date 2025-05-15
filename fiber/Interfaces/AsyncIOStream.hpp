#pragma once

#include "AsyncInputStream.hpp"
#include "AsyncOutputStream.hpp"

namespace fiber
{

    class iAsyncIOStream : public iAsyncInputStream, iAsyncOutputStream{};

    template<class IOStream>
    class cAsyncIOStream = cAsyncInputStream<IOStream> && cAsyncOutputStream<IOStream>;

} // namespace fiber
