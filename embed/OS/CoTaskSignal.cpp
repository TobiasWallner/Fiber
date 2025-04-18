#include <embed/OStream/OStream.hpp>
#include <embed/OS/CoTaskSignal.hpp>

namespace embed
{
    
    embed::OStream& operator << (embed::OStream& stream, embed::CoTaskSignal::Type type){
        switch(type){
            case embed::CoTaskSignal::Type::None : return stream << "None";
            case embed::CoTaskSignal::Type::Await : return stream << "Await";
            case embed::CoTaskSignal::Type::NextCycle : return stream << "NextCycle";
            case embed::CoTaskSignal::Type::ImplicitDelay : return stream << "ImplicitDelay";
            case embed::CoTaskSignal::Type::ExplicitDelay : return stream << "ExplicitDelay";
            default : return stream << "N/A";
        };
    }
    
} // namespace embed