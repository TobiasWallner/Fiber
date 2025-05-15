#include <fiber/OStream/OStream.hpp>
#include "OStream_test.hpp"

namespace fiber
{
    
        
    void OutputStream_test_string_formating(OStream& stream){
        stream << "test string formating" << newl;
        stream << "---------------------" << newl;
        stream << "'" << FormatStr("String", 6) << "', Expected: 'String'" << newl;
        stream << "'" << FormatStr("String") << "', Expected: 'String'" << newl;
        stream << "'" << FormatStr("String").mwidth(10) << "', Expected: '    String'" << newl;
        stream << "'" << FormatStr("String").mwidth(10).left() << "', Expected: 'String    '" << newl;
        stream << "'" << FormatStr("String").mwidth(10).fill('.') << "', Expected: '....String'" << newl;
        stream << "'" << FormatStr("String").mwidth(10).left().fill('.') << "', Expected: 'String....'" << newl;
        
        FormatStr lformat = FormatStr().mwidth(64).fill('.').left();
        FormatStr rformat = FormatStr().mwidth(8).fill('.');
        stream << lformat("Chapter 1 The Beginning") << rformat("1") << endl;
        stream << lformat("Chapter 2 The Action") << rformat("22") << endl;
        stream << lformat("Chapter 3 The End") << rformat("333") << endl;
        stream << "Expected: " << newl;
        stream << "Chapter 1 The Beginning................................................1" << newl;
        stream << "Chapter 2 The Action..................................................22" << newl;
        stream << "Chapter 3 The End....................................................333" << newl;
        stream << "---" << newl;
        stream << "Alignment [" << FormatStr("Left").mwidth(10).left() << "]" << endl;
        stream << "Alignment [" << FormatStr("Right").mwidth(10).right() << "]" << endl;
        stream << "Alignment [" << FormatStr("Center").mwidth(10).center() << "]" << endl;
        stream << "Expected: " << newl;
        stream << "Alignment [Left      ]" << newl;
        stream << "Alignment [     Right]" << newl;
        stream << "Alignment [  Center  ]" << newl;
        stream << "---" << newl;
    }

    void OutputStream_test_bool_formating(OStream& stream){
        stream << "test boolean formating" << newl;
        stream << "----------------------" << newl;
        stream << "'" << true << "', Expected: 'true'" << newl;
        stream << "'" << false << "', Expected: 'false'" << newl;
        stream << "'" << FormatBool(true).num() << "', Expected: '1'" << newl;
        stream << "'" << FormatBool(false).num() << "', Expected: '0'" << newl;
        stream << "'" << FormatBool(true).text() << "', Expected: 'true'" << newl;
        stream << "'" << FormatBool(false).text() << "', Expected: 'false'" << newl;
        stream << "'" << FormatBool(true).mwidth(6) << "', Expected: '  true'" << newl;
        stream << "'" << FormatBool(false).mwidth(6) << "', Expected: ' false'" << newl;
        stream << "'" << FormatBool(true).num().mwidth(6) << "', Expected: '     1'" << newl;
        stream << "'" << FormatBool(false).num().mwidth(6) << "', Expected: '     0'" << newl;
        stream << "'" << FormatBool(true).mwidth(6).left() << "', Expected: 'true  '" << newl;
        stream << "'" << FormatBool(false).mwidth(6).left() << "', Expected: 'false '" << newl;
        stream << "'" << FormatBool(true).num().mwidth(6).left() << "', Expected: '1     '" << newl;
        stream << "'" << FormatBool(false).num().mwidth(6).left() << "', Expected: '0     '" << newl;
        stream << "'" << FormatBool(true).mwidth(6).center() << "', Expected: ' true '" << newl;
        stream << "'" << FormatBool(false).mwidth(6).center() << "', Expected: ' false'" << newl;
        stream << "'" << FormatBool(true).num().mwidth(6).center() << "', Expected: '   1  '" << newl;
        stream << "'" << FormatBool(false).num().mwidth(6).center() << "', Expected: '   0  '" << newl;
    }


    void OutputStream_test_integer_formating(OStream& stream){
        stream << "test integer formating" << newl;
        stream << "----------------------" << newl;
        stream << "'" << 42L << "', Expected: '42'" << newl;
        stream << "'" << FormatInt(42).fsign() << "', Expected: '+42'" << newl;
        stream << "'" << -42 << "', Expected: '-42'" << newl;
        stream << "'" << 42UL << "', Expected: '42'" << newl;
        stream << "'" << 0ULL << "', Expected: '0'" << newl;
        stream << "'" << FormatInt(32'156'584LL).thousands('.') << "', Expected: '32.156.584'" << newl;
        stream << "'" << FormatInt(32'156'584L).thousands() << "', Expected: '32,156,584'" << newl;
        stream << "'" << FormatInt(42U).mwidth(6) << "', Expected: '    42'" << newl;
        stream << "'" << FormatInt(42).mwidth(6).fsign() << "', Expected: '   +42'" << newl;
        stream << "'" << FormatInt(-42).mwidth(6) << "', Expected: '   -42'" << newl;
        stream << "'" << FormatInt(42).mwidth(6).fsign().pad_sign() << "', Expected: '+   42'" << newl;
        stream << "'" << FormatInt(-42).mwidth(6).pad_sign() << "', Expected: '-   42'" << newl;
        stream << "'" << FormatInt((uint8_t)42).mwidth(6).fsign().pad_sign().fill('0') << "', Expected: '+00042'" << newl;
        stream << "'" << FormatInt((int16_t)-42).mwidth(6).pad_sign().fill('0') << "', Expected: '-00042'" << newl;
        stream << "'" << FormatInt((uint32_t)42).mwidth(6).fsign().pad_sign().fill('0').left() << "', Expected: '+42000'" << newl;
        stream << "'" << FormatInt((int64_t)-42).mwidth(6).pad_sign().fill('0').left() << "', Expected: '-42000'" << newl;
        stream << "'" << FormatInt((int32_t)42).mwidth(6).left() << "', Expected: '42    '" << newl;
        stream << "'" << FormatInt((int8_t)-42).mwidth(6).left() << "', Expected: '-42   '" << newl;
        stream << "'" << FormatInt((int32_t)42).mwidth(6).center() << "', Expected: '  42  '" << newl;
        stream << "'" << FormatInt((int32_t)42).mwidth(6).center().fsign() << "', Expected: '  +42 '" << newl;
        stream << "'" << FormatInt((int8_t)-42).mwidth(6).center() << "', Expected: '  -42 '" << newl;
    }

    void OutputStream_test_float_formating(OStream& stream){
        stream << "test float formating" << newl;
        stream << "----------------------" << newl;
        stream << "'" << 3.536f << "', Expected: '3.536'" << newl;
        stream << "'" << 3.536 << "', Expected: '3.536'" << newl;
        stream << "'" << 3.536l << "', Expected: '3.536'" << newl;
        stream << "'" << 2.0 << "', Expected: '2'" << newl;
        
        stream << "'" << FormatFloat(3.536f).fexp() << "', Expected: '3.536e0'" << newl;
        stream << "'" << FormatFloat(2.0).fdeci() << "', Expected: '2.000'" << newl;
        stream << "'" << FormatFloat(2.0).fcomma() << "', Expected: '2.'" << newl;
        
        stream << "science formating:" << newl;
        stream << "'" << FormatFloat(1.23e0).sci() << "', Expected: '1.23'" << newl;
        stream << "'" << FormatFloat(1.23e1).sci() << "', Expected: '1.23e1'" << newl;
        stream << "'" << FormatFloat(1.23e2).sci() << "', Expected: '1.23e2'" << newl;
        stream << "'" << FormatFloat(1.23e3).sci() << "', Expected: '1.23e3'" << newl;
        stream << "'" << FormatFloat(23165546.231564).sci() << "', Expected: '2.317e7'" << newl;
        
        stream << "engineering formating:" << newl;
        stream << "'" << FormatFloat(1.23e0).eng() << "', Expected: '1.23'" << newl;
        stream << "'" << FormatFloat(1.23e1).eng() << "', Expected: '12.3'" << newl;
        stream << "'" << FormatFloat(1.23e2).eng() << "', Expected: '123'" << newl;
        stream << "'" << FormatFloat(1.23e3).eng() << "', Expected: '1.23e3'" << newl;
        stream << "'" << FormatFloat(23165546.231564).eng() << "', Expected: '23.166e6'" << newl;

        stream << "full formating:" << newl;
        stream << "'" << FormatFloat(1.23e0).full() << "', Expected: '1.23'" << newl;
        stream << "'" << FormatFloat(1.23e1).full() << "', Expected: '12.3'" << newl;
        stream << "'" << FormatFloat(1.23e2).full() << "', Expected: '123'" << newl;
        stream << "'" << FormatFloat(1.23e3).full() << "', Expected: '1230'" << newl;
        stream << "'" << FormatFloat(23165546.231564).full() << "', Expected: '23165546.232'" << newl;
        stream << "'" << FormatFloat(23165546.231564).full().thousands().decimals(5) << "', Expected: '23,165,546.231,56'" << newl;

        stream << "special formating:" << newl;
        float nan_val = std::nanf("");                      // NaN
        float pos_inf = std::numeric_limits<float>::infinity(); // +Infinity
        float neg_inf = -std::numeric_limits<float>::infinity(); // -Infinity
        stream << "'" << nan_val << "', Expected: 'nan'" << newl;
        stream << "'" << pos_inf << "', Expected: 'inf'" << newl;
        stream << "'" << FormatFloat(pos_inf).fsign() << "', Expected: '+inf'" << newl;
        stream << "'" << neg_inf << "', Expected: '-inf'" << newl;

    }

    void OutputStream_test(OStream& stream){
        stream << OutputStream_test_string_formating << newl;
        stream << OutputStream_test_bool_formating << newl;
        stream << OutputStream_test_integer_formating << newl;
        stream << OutputStream_test_float_formating << newl;
        
        stream.flush();
    }

} // namespace fiber
