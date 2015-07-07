/****************************************************************************
Copyright (c) 2015 Hiroshi Fukuda.
****************************************************************************/

//
// Converter.cpp
//

#pragma execution_character_set("utf-8")

#include "Converter.h"

USING_NS_CC;

// "0xARGB"の文字列からアルファ付きのColor4Bを返す
cocos2d::Color4B Converter::fromARGB(std::string code) {

    // 0xARGBコードが見つかったら
    if (code.find("0x") == 0 && code.length() == 10) {
        
        try {
            // A
            int a = (int)strtol(code.substr(2, 2).c_str(), nullptr, 16);
            // R
            int r = (int)strtol(code.substr(4, 2).c_str(), nullptr, 16);
            // G
            int g = (int)strtol(code.substr(6, 2).c_str(), nullptr, 16);
            // B
            int b = (int)strtol(code.substr(8, 2).c_str(), nullptr, 16);


            return cocos2d::Color4B(r, g, b, a);

        } catch (...) {
            // Error
            log("illegal color code : %s", code.c_str());
        }
    } else {
        // Error
        log("not supeert format : %s", code.c_str());
    }

    return cocos2d::Color4B::BLACK;
}

// 文字列の "true" or "false" を bool型の true or false に変換する
// "1"もtrueとして扱う
bool Converter::stringToBool(std::string strBool, bool def) {

    if (strBool == "true" || strBool == "1") {
        return true;
    }

    return def;
}

bool Converter::stringToBool(std::string strBool) {
    return stringToBool(strBool, false);
}
