#include <embed/OStream/OStream.hpp>
#include <embed/OS/CoSignal.hpp>

namespace embed
{
    
    embed::OStream& operator << (embed::OStream& stream, embed::CoSignal::Type type){
        switch(type){
            case embed::CoSignal::Type::None : return stream << "None";
            case embed::CoSignal::Type::Await : return stream << "Await";
            case embed::CoSignal::Type::NextCycle : return stream << "NextCycle";
            case embed::CoSignal::Type::ImplicitDelay : return stream << "ImplicitDelay";
            case embed::CoSignal::Type::ExplicitDelay : return stream << "ExplicitDelay";
            default : return stream << "N/A";
        };
    }
    
} // namespace embed