#pragma once

#include <fiber/OStream/OStream.hpp>

namespace fiber{

    enum class Exit {
        Success,   ///< Task completed successfully; destroy it
        Failure    ///< Task failed; notify or destroy
    };

    inline OStream& operator<<(OStream& stream, Exit value){
        switch(value){
            case Exit::Success: return stream << FormatStr("Success", 7);
            case Exit::Failure: return stream << FormatStr("Failure", 7);
            default : return stream << FormatStr("N/A", 3);
        };
    }

}