#include "embed/Exceptions.hpp"

namespace embed{


    const char* RawStringException::what() const noexcept {
        return this->_msg;
    }

}