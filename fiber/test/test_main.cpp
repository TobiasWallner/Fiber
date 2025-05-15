
// std
#include <iostream>

// fiber
#include "test.hpp"
#include <fiber/OStream/OStream.hpp>

class StdOut : public fiber::OStream{
    public:
    inline void put(char c) final {std::cout.put(c);}
    inline void flush() final {std::cout.flush();}
    inline void write(const char* str, size_t len) final {std::cout.write(str, len);}
};

class StdErr : public fiber::OStream{
    public:
    inline void put(char c) final {std::cerr.put(c);}
    inline void flush() final {std::cerr.flush();}
    inline void write(const char* str, size_t len) final {std::cerr.write(str, len);}
};

int main(){
    // redirect fiber output streams
    StdOut cout;
    StdErr cerr;
    fiber::cout = cout;
    fiber::cerr = cerr;

    return test_all();
    
}