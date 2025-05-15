#include <fiber/OStream/OStream.hpp>
#include <fiber/OS/CoSignal.hpp>

namespace fiber
{
    
    fiber::OStream& operator << (fiber::OStream& stream, fiber::CoSignal::Type type){
        switch(type){
            case fiber::CoSignal::Type::None : return stream << "None";
            case fiber::CoSignal::Type::Await : return stream << "Await";
            case fiber::CoSignal::Type::NextCycle : return stream << "NextCycle";
            case fiber::CoSignal::Type::ImplicitDelay : return stream << "ImplicitDelay";
            case fiber::CoSignal::Type::ExplicitDelay : return stream << "ExplicitDelay";
            default : return stream << "N/A";
        };
    }
    
} // namespace fiber