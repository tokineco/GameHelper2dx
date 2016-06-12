# GameHelper2dx
helper and functions for cocos2d-x v3.8 over.

sorry. comment language is japanese.

---
### 概要
Cocos2d-x(v3.8以降)用のライブラリです。  
MIT Licenseのもと、自由に使用できます。

* AudioManager : Audio系ライブラリ
* Converter : 文字列からの型変換、操作等
* MathHelper : ランダム値取得等 
* SDKBOXHelper : SDKBOXを使用した場合のプラットフォーム吸収ヘルパー。

>※ SDKBOXHelperに関しては、Windows環境でエラーが出ないようにしたものであり、iOSやAndroidでSDKBOXプラグインが含まれていない場合はエラーとなります。基本的にプロジェクトに含めないようにし、参考程度にしてください。

使用したいヘッダファイルのみをインポートするか、全てのライブラリ(SDKBOXを除く)を使いたい場合は"GameHelper2dx.h"をインポートしてください。
  
    #import "lib/GameHelper2dx/GameHelper2dx.h"

個別指定の場合は、

    #import "lib/GameHelper2dx/AudioManager.h"

---
### AudioManager
AudioManagerでは、一部のプラットフォーム・形式を除いてCocos2d-xのAudioEngineを使用しています。  
**BGMのフェードや区間ループ、SEのチャンク指定再生にも対応しています。**

シンプルな使い方

    // 1.5秒のフェードインをしながらtitleを再生
    AudioManager::getInstance()->playBgm("bgm/title", 1.5f);

拡張子は不要です(Cocos2dxが対応する拡張子が存在すれば再生します)  

また、事前にjson形式のaudio定義ファイルを読み込むことによって、再生キー名、ファイルパスを定義することができ、ファイル名の仕様変更等に対応できます。さらに、BGMの区間ループにも対応可能となります。

定義ファイルの構造と読み込み方法

[Resources/data/audio.json]

    {
        "BGM":{
            "title":"bgm/title",
            "game_happy":["bgm/game01", 20.129, 79.990]
        }
        ,
        "SE":
        {
            "ui_ok":"sounds/ui_ok",
            "ui_cancel":"sounds/ui_cancel"
        }
    }

読み込みコード(AppDelegate.cpp等)

    // オーディオ定義ファイルの読み込み  
    AudioManager::getInstance()->readAudioListFile("data/audio.json");

BGMのvalueを配列指定の場合、[ファイルパス, ループ開始位置(秒), ループ終端位置(秒)] で区間ループに対応します。

定義ファイルを読み込んでいない場合、オーディオ再生に指定するファイル名はファイルパスを指定してください。

#### 実装

*BGMの再生*

    // 1.5秒のフェードインをしながらtitleを再生
    AudioManager::getInstance()->playBgm("title", 1.5f);

*BGMの停止*

    // 1.5秒のフェードアウトをしながら停止
    AudioManager::getInstance()->stopBgm(1.5f);

*BGMの一時停止*

    // 1.5秒のフェードアウトを一時停止
    AudioManager::getInstance()->pauseBgm(1.5f);

*BGMのリジューム再生*

    // 1.5秒のフェードインをしながら再開
    AudioManager::getInstance()->resumeBgm(1.5f);

*SEの再生*

    // OK音を再生
    AudioManager::getInstance()->playSe("ui_ok");
    // OK音をチャンクを指定して再生(チャンクは0～7まで使用できます)
    AudioManager::getInstance()->playSe("ui_ok", 1);

---
### Converter, MathHelper
利用頻度が高そうな文字列操作、取得関数等を入れています。  
オマケ程度にどうぞ。

---
朱鷺裕二 @ cretia studio  
http://studio.cretia.net
