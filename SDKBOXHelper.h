//
// SDKBOXHelper.h
//
#ifndef __SDKBOXHelper__
#define __SDKBOXHelper__

#include "cocos2d.h"

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
#include "PluginGoogleAnalytics/PluginGoogleAnalytics.h"
#include "PluginIAP/PluginIAP.h"
#endif

class SDKBOXHelper {

public:

    // GoogleAnalyticsの初期化処理
    static void initAnalytics();
    // GoogleAnalyticsにScreen送信
    static void sendAnalyticsScreen(std::string screenName);
    // GoogleAnalyticsにEvent送信(スクリーン名指定)
    static void sendAnalyticsEvent(std::string screenName, std::string eventCategory, std::string eventAction, std::string eventLabel = "", int value = 0);
    
    // iAPの初期化処理
    static void initIAP();
    // iAPのリスナーセット
    static void setIAPListener(sdkbox::IAPListener * listener);
    
};

#endif /* defined(__SDKBOXHelper__) */
