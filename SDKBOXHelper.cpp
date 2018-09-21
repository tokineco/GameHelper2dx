/****************************************************************************
Copyright (c) 2016 Yuji Toki(tokineco)
- MIT license

- In-App Purchase
- Google Analytics

* 使用しない場合はプロジェクトから外してください
****************************************************************************/

#pragma execution_character_set("utf-8")

#include "SDKBOXHelper.h"

USING_NS_CC;


//==========================
// Google Analytics
//==========================

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


//==========================
// In-App Purchase
//==========================

// iAPの初期化
void SDKBOXHelper::initIAP() {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    
#if COCOS2D_DEBUG == 1
    sdkbox::IAP::setDebug(true);
#endif

    sdkbox::IAP::init();
    CCLOG("initialized iAP.");
#endif
}

// iAPの最新データを取得する
void SDKBOXHelper::refreshIAP() {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    sdkbox::IAP::refresh();
#endif
}

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
// iAPのリスナーセット(Mobileのみ)
void SDKBOXHelper::setIAPListener(sdkbox::IAPListener * listener) {
    sdkbox::IAP::setListener(listener);
}

// iAPのリスナー削除(Mobileのみ)
void SDKBOXHelper::removeIAPListener() {
    sdkbox::IAP::removeListener();
}
#endif

// iAPの課金リクエスト
void SDKBOXHelper::purchaseIAP(const std::string name) {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    sdkbox::IAP::purchase(name);
#endif
}

// iAPのリストア処理
void SDKBOXHelper::restoreIAP() {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    sdkbox::IAP::restore();
#endif
}
