#include <embed/Exceptions/Exceptions.hpp>
#include <embed/OStream/OStream.hpp>
#include <embed/OStream/ansi.hpp>

namespace embed{
    
    // -----------------------------------------------------------------------------------------------
    //                                    Exception
    // -----------------------------------------------------------------------------------------------

    const char* Exception::what() const noexcept {return this->_what;};

    void Exception::print(OStream& stream) const {
        stream << '[' << embed::ansi::red << this->type() << embed::ansi::reset << "]: " << this->what() << embed::endl;
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
            stream << '[' << embed::ansi::bright_red << embed::ansi::bold << this->type() << embed::ansi::reset << "]: " << this->condition << embed::endl;
            stream << "    message: " << embed::ansi::yellow << this->what() << embed::ansi::reset << embed::newl;
        }else{
            stream << '[' << embed::ansi::bright_red << this->type() << embed::ansi::reset << "]: " << this->what() << embed::endl;
        }
        stream << "    in: " << embed::ansi::magenta << this->function_signature << embed::ansi::reset << embed::endl;
    };

    void AssertionFailureCritical::print(OStream& stream) const {
        if(this->condition){
            stream << '[' << embed::ansi::bright_red << embed::ansi::bold << this->type() << embed::ansi::reset << "]: " << this->condition << embed::endl;
            stream << "    message: " << embed::ansi::yellow << this->what() << embed::ansi::reset << embed::newl;
        }else{
            stream << '[' << embed::ansi::bright_red << this->type() << embed::ansi::reset << "]: " << this->what() << embed::endl;
        }
        stream << "    in: " << embed::ansi::magenta << this->function_signature << embed::ansi::reset << embed::endl;
    };

    void AssertionFailureO1::print(OStream& stream) const {
        if(this->condition){
            stream << '[' << embed::ansi::bright_yellow << embed::ansi::bold << this->type() << embed::ansi::reset << "]: " << this->condition << embed::endl;
            stream << "    message: " << embed::ansi::yellow << this->what() << embed::ansi::reset << embed::newl;
        }else{
            stream << '[' << embed::ansi::red << this->type() << embed::ansi::reset << "]: " << this->what() << embed::endl;
        }
        stream << "    in: " << embed::ansi::magenta << this->function_signature << embed::ansi::reset << embed::endl;
    };

    void AssertionFailureFull::print(OStream& stream) const {
        if(this->condition){
            stream << '[' << embed::ansi::bright_cyan << embed::ansi::bold << this->type() << embed::ansi::reset << "]: " << this->condition << embed::endl;
            stream << "    message: " << embed::ansi::yellow << this->what() << embed::ansi::reset << embed::newl;
        }else{
            stream << '[' << embed::ansi::red << this->type() << embed::ansi::reset << "]: " << this->what() << embed::endl;
        }
        stream << "    in: " << embed::ansi::magenta << this->function_signature << embed::ansi::reset << embed::endl;
    };
    
    // -----------------------------------------------------------------------------------------------
    //                                    Allocation Failure
    // -----------------------------------------------------------------------------------------------

    void AllocationFailure::print(OStream& stream) const {
        stream << '[' << embed::ansi::bright_red << embed::ansi::bold << this->type() << embed::ansi::reset << "]: Failed to allocate: " << this->to_allocate << " byte. S: Provide more memory, prevent fragmentation, use differnet allocation strategies." << embed::newl;
        stream << "    buffer size: " << this->buffer_size << " byte" << embed::newl;
        stream << "    largest free region: " << this->largest_free << " byte" << embed::newl;
        stream << "    free/allocated regions: " << this->nfree << '/' << this->nalloc << embed::endl;
    }

}// namespace embed
