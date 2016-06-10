/****************************************************************************
Copyright (c) 2016 Yuji Toki(tokineco)
- MIT license
****************************************************************************/

#ifndef __MathHelper__
#define __MathHelper__

#include "cocos2d.h"

class MathHelper {

public:

    // min以上max以下のint値をランダムに返す
    static int getRandom(int minValue, int maxValue);

};

#endif /* defined(__MathHelper__) */
