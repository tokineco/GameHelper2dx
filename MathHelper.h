//
// MathHelper.h
//

#ifndef __MathHelper__
#define __MathHelper__

#include "cocos2d.h"

class MathHelper {

public:

    // min以上max以下のint値をランダムに返す
    static int getRandom(int minValue, int maxValue);

};

#endif /* defined(__MathHelper__) */
