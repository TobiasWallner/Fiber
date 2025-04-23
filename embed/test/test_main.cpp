
// std
#include <iostream>

// embed
#include "test.hpp"
#include <embed/OStream/OStream.hpp>

class StdOut : public embed::OStream{
    public:
    inline void put(char c) override{std::cout.put(c);}
    inline void flush() override{std::cout.flush();}
    inline void write(const char* str, size_t len) override{std::cout.write(str, len);}
};

class StdErr : public embed::OStream{
    public:
    inline void put(char c) override{std::cerr.put(c);}
    inline void flush() override{std::cerr.flush();}
    inline void write(const char* str, size_t len) override{std::cerr.write(str, len);}
};

int main(){
    // redirect embed output streams
    StdOut cout;
    StdErr cerr;
    embed::cout = cout;
    embed::cerr = cerr;

    return test_all();
    
}