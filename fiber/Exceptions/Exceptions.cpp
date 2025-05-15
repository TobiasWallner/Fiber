#include <fiber/Exceptions/Exceptions.hpp>
#include <fiber/OStream/OStream.hpp>
#include <fiber/OStream/ansi.hpp>

namespace fiber{
    
    // -----------------------------------------------------------------------------------------------
    //                                    Exception
    // -----------------------------------------------------------------------------------------------

    const char* Exception::what() const noexcept {return this->_what;};

    void Exception::print(OStream& stream) const {
        stream << '[' << fiber::ansi::red << this->type() << fiber::ansi::reset << "]: " << this->what() << fiber::endl;
    };
    
    OStream& operator<<(OStream& stream, const Exception& e) {
        e.print(stream);
        return stream;
    }


    // -----------------------------------------------------------------------------------------------
    //                                    AssertionFailure
    // -----------------------------------------------------------------------------------------------

    void AssertionFailure::print(OStream& stream) const {
        if(this->condition){
            stream << '[' << fiber::ansi::bright_red << fiber::ansi::bold << this->type() << fiber::ansi::reset << "]: " << this->condition << fiber::endl;
            stream << "    message: " << fiber::ansi::yellow << this->what() << fiber::ansi::reset << fiber::newl;
        }else{
            stream << '[' << fiber::ansi::bright_red << this->type() << fiber::ansi::reset << "]: " << this->what() << fiber::endl;
        }
        stream << "    in: " << fiber::ansi::magenta << this->function_signature << fiber::ansi::reset << fiber::endl;
    };

    void AssertionFailureCritical::print(OStream& stream) const {
        if(this->condition){
            stream << '[' << fiber::ansi::bright_red << fiber::ansi::bold << this->type() << fiber::ansi::reset << "]: " << this->condition << fiber::endl;
            stream << "    message: " << fiber::ansi::yellow << this->what() << fiber::ansi::reset << fiber::newl;
        }else{
            stream << '[' << fiber::ansi::bright_red << this->type() << fiber::ansi::reset << "]: " << this->what() << fiber::endl;
        }
        stream << "    in: " << fiber::ansi::magenta << this->function_signature << fiber::ansi::reset << fiber::endl;
    };

    void AssertionFailureO1::print(OStream& stream) const {
        if(this->condition){
            stream << '[' << fiber::ansi::bright_yellow << fiber::ansi::bold << this->type() << fiber::ansi::reset << "]: " << this->condition << fiber::endl;
            stream << "    message: " << fiber::ansi::yellow << this->what() << fiber::ansi::reset << fiber::newl;
        }else{
            stream << '[' << fiber::ansi::red << this->type() << fiber::ansi::reset << "]: " << this->what() << fiber::endl;
        }
        stream << "    in: " << fiber::ansi::magenta << this->function_signature << fiber::ansi::reset << fiber::endl;
    };

    void AssertionFailureFull::print(OStream& stream) const {
        if(this->condition){
            stream << '[' << fiber::ansi::bright_cyan << fiber::ansi::bold << this->type() << fiber::ansi::reset << "]: " << this->condition << fiber::endl;
            stream << "    message: " << fiber::ansi::yellow << this->what() << fiber::ansi::reset << fiber::newl;
        }else{
            stream << '[' << fiber::ansi::red << this->type() << fiber::ansi::reset << "]: " << this->what() << fiber::endl;
        }
        stream << "    in: " << fiber::ansi::magenta << this->function_signature << fiber::ansi::reset << fiber::endl;
    };
    
    // -----------------------------------------------------------------------------------------------
    //                                    Allocation Failure
    // -----------------------------------------------------------------------------------------------

    void AllocationFailure::print(OStream& stream) const {
        stream << '[' << fiber::ansi::bright_red << fiber::ansi::bold << this->type() << fiber::ansi::reset << "]: Failed to allocate: " << this->to_allocate << " byte. S: Provide more memory, prevent fragmentation, use differnet allocation strategies." << fiber::newl;
        stream << "    buffer size: " << this->buffer_size << " byte" << fiber::newl;
        stream << "    largest free region: " << this->largest_free << " byte" << fiber::newl;
        stream << "    free/allocated regions: " << this->nfree << '/' << this->nalloc << fiber::endl;
    }

}// namespace fiber
