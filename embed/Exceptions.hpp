#pragma once

// source: Exceptions.cpp

#include <exception>


namespace embed{

    class RawStringException : public std::exception{

        const char* _msg = nullptr;
    
    public:
        explicit RawStringException(const char* msg) noexcept : _msg(msg){}
    
        const char* what() const noexcept override;
    };

}
