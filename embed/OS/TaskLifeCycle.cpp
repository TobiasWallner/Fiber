#include "TaskLifeCycle.hpp"

OStream& operator<<(OStream& stream, TaskLifeCycle value){
    switch(value){
        case TaskLifeCycle::New         : return stream << FormatStr("New"          , 3);
        case TaskLifeCycle::Created     : return stream << FormatStr("Created"      , 7);
        case TaskLifeCycle::Waiting     : return stream << FormatStr("Waiting"      , 7);
        case TaskLifeCycle::Ready       : return stream << FormatStr("Ready"        , 5);
        case TaskLifeCycle::Running     : return stream << FormatStr("Running"      , 7);
        case TaskLifeCycle::Awaiting    : return stream << FormatStr("Awaiting"     , 8);
        case TaskLifeCycle::Yielding    : return stream << FormatStr("Yielding"     , 8);
        case TaskLifeCycle::Delaying    : return stream << FormatStr("Delaying"     , 8);
        case TaskLifeCycle::ReReady     : return stream << FormatStr("ReReady"      , 7);
        case TaskLifeCycle::Disabled    : return stream << FormatStr("Disabled"     , 8);
        case TaskLifeCycle::ExitSuccess : return stream << FormatStr("ExitSuccess"  , 11);
        case TaskLifeCycle::ExitFailure : return stream << FormatStr("ExitFailure"  , 11);
        case TaskLifeCycle::Died        : return stream << FormatStr("Died"         , 4);
        case TaskLifeCycle::Killed      : return stream << FormatStr("Killed"       , 6);
        default : return stream <<FormatStr("N/A", 3);
    }
}