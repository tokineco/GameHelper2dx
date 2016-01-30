//
// SDKBOXHelper.cpp
//
#pragma execution_character_set("utf-8")

#include "SDKBOXHelper.h"

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
#include "PluginGoogleAnalytics/PluginGoogleAnalytics.h"
#endif

USING_NS_CC;

// GoogleAnalyticsの初期化
void SDKBOXHelper::initAnalytics() {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    sdkbox::PluginGoogleAnalytics::init();
    sdkbox::PluginGoogleAnalytics::startSession();
    CCLOG("initialized analytics.");
#endif
}

// GoogleAnalyticsにScreen送信
void SDKBOXHelper::sendAnalyticsScreen(std::string screenName) {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    sdkbox::PluginGoogleAnalytics::logScreen(screenName);
    sdkbox::PluginGoogleAnalytics::dispatchHits();
    CCLOG("send analytics : %s", screenName.c_str());
#endif
}

// GoogleAnalyticsにEvent送信(スクリーン名指定)
void SDKBOXHelper::sendAnalyticsEvent(std::string screenName, std::string eventCategory, std::string eventAction, std::string eventLabel /* = ""*/, int value /* = 0*/) {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    if (screenName != "") {
        sdkbox::PluginGoogleAnalytics::logScreen(screenName);
    }
    sdkbox::PluginGoogleAnalytics::logEvent(eventCategory, eventAction, eventLabel, value);
    sdkbox::PluginGoogleAnalytics::dispatchHits();
    CCLOG("send analytics : %s %s %s %s %d", screenName.c_str(), eventCategory.c_str(), eventAction.c_str(), eventLabel.c_str(), value);
#endif
}