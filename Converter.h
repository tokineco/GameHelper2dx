//
// Converter.h
//

#ifndef __Converter__
#define __Converter__

#include "cocos2d.h"

class Converter {

public:

    // "0xARGB"の文字列からアルファ付きのColor4Bを返す
    static cocos2d::Color4B fromARGB(std::string code);

    // 文字列の "true" or "false" を bool型の true or false に変換する
    // "1"もtrueとして扱う
    static bool stringToBool(std::string strBool);
    // 文字列の "true" or "false" を bool型の true or false に変換する
    // "1"もtrueとして扱う
    static bool stringToBool(std::string strBool, bool def);

};

#endif /* defined(__Converter__) */
