//
// SDKBOXHelper.h
//
#ifndef __SDKBOXHelper__
#define __SDKBOXHelper__

#include "cocos2d.h"

class SDKBOXHelper {

public:

    // GoogleAnalyticsの初期化処理
    static void initAnalytics();
    // GoogleAnalyticsにScreen送信
    static void sendAnalyticsScreen(std::string screenName);
    // GoogleAnalyticsにEvent送信(スクリーン名指定)
    static void sendAnalyticsEvent(std::string screenName, std::string eventCategory, std::string eventAction, std::string eventLabel = "", int value = 0);
};

#endif /* defined(__SDKBOXHelper__) */
