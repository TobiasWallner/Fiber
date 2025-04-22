
// std
#include <iostream>

// embed
#include <embed/OStream/OStream.hpp>
#include <embed/Containers/tests/ArrayList_test.hpp>
#include <embed/Containers/tests/DualArrayList_test.hpp>


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

    
    std::cout << "test start: " << std::endl;
    try{
        embed::ArrayList_test();
        embed::DualArrayList_test();

    }catch(embed::Exception& e){
        embed::cout << e << embed::endl;
    }

    return 0;
}