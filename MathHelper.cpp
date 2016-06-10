/****************************************************************************
Copyright (c) 2016 Yuji Toki(tokineco)
- MIT license
****************************************************************************/

#pragma execution_character_set("utf-8")

#include "MathHelper.h"

USING_NS_CC;

// min以上max以下のint値をランダムに返す
int MathHelper::getRandom(int minValue, int maxValue) {

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> num(minValue, maxValue);

    return num(mt);
}
