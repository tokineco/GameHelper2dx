/****************************************************************************
 Copyright (c) 2016 Yuji Toki(tokineco)
 - MIT license

 - In-App Purchase

 * 使用しない場合はプロジェクトから外してください
****************************************************************************/

#ifndef __SDKBOXHelper__
#define __SDKBOXHelper__

#include "cocos2d.h"

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
#include "PluginIAP/PluginIAP.h"
#endif

class SDKBOXHelper {

public:
    
    // iAPの初期化処理
    static void initIAP();

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    // iAPのリスナーセット(Mobileのみ)
    static void setIAPListener(sdkbox::IAPListener * listener);
    //iAPのリスナー削除(Mobileのみ)
    static void removeIAPListener();
#endif

    // iAPの最新データを取得する
    static void refreshIAP();
    // iAPの課金リクエスト
    static void purchaseIAP(const std::string name);
    // iAPのリストア処理
    static void restoreIAP();

};

#endif /* defined(__SDKBOXHelper__) */
