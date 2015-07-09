// SE only.
// Windows-wav use SimpleAudioEngine. Other format use AudioEngine.

#pragma execution_character_set("utf-8")

#include "SimpleAudioEngine.h"              // Windows-wav
#include "audio/include/AudioEngine.h"      // iOS, Android
#include "AudioManager.h"

USING_NS_CC;
using namespace experimental;

AudioManager* AudioManager::_instance = NULL;

// コンストラクタ
AudioManager::AudioManager()
    :_seVolume(0.6f)
{
    // チャンク配列の初期化
    for(int i=0; i < sizeof(_chunk) / sizeof(_chunk[0]); i++) {
        _chunk[i] = -1;
    }
}

// デストラクタ
AudioManager::~AudioManager() {

}

// 初期化
AudioManager* AudioManager::getInstance() {
	
	if (_instance == NULL) {
        _instance = new AudioManager();
	}

	return _instance;
}

// 削除する際に使用
void AudioManager::deleteInstance() {

	if (_instance != NULL) {
		delete _instance;
	}

	_instance = NULL;
}


// 端末ごとに読み込む拡張子を変えて、そのファイル名を返す
std::string AudioManager::getFileName(const std::string baseName) {

    auto platform = Application::getInstance()->getTargetPlatform();

    std::string ext = ".wav";               // 拡張子

    // すでに拡張子(.～)が含まれているならそのまま返す
    if (baseName.find_last_of(".") != std::string::npos) {
        if (FileUtils::getInstance()->isFileExist(baseName)) {
            return baseName;
        } else {
            return "";
        }
    }

    switch (platform) {
        case cocos2d::ApplicationProtocol::Platform::OS_WINDOWS:
        case cocos2d::ApplicationProtocol::Platform::OS_ANDROID:

            // ogg > mp3 > wav
            if (FileUtils::getInstance()->isFileExist(baseName + ".ogg")) {
                ext = ".ogg";
            } else if (FileUtils::getInstance()->isFileExist(baseName + ".mp3")) {
                ext = ".mp3";
            }
            break;
        case cocos2d::ApplicationProtocol::Platform::OS_IPHONE:
        case cocos2d::ApplicationProtocol::Platform::OS_IPAD:

            // caf > mp3 > wav
            if (FileUtils::getInstance()->isFileExist(baseName + ".caf")) {
                ext = ".caf";
            } else if (FileUtils::getInstance()->isFileExist(baseName + ".mp3")) {
                ext = ".mp3";
            }
            break;
        default:
            break;
    }

    if (FileUtils::getInstance()->isFileExist(baseName + ext)) {
        return baseName + ext;
    }

    // それでも見つからなければ空文字を返して、その先でエラーとする
    log("file not found %s.", baseName);
    return baseName;

}


// 拡張子を取得する
const char* AudioManager::getExtension(const std::string fileName) {
    return strchr(fileName.c_str(), '.');
}


// 効果音のPreLoad
void AudioManager::preloadSe(const std::string baseName) {

    std::string fileName = getFileName(baseName);
    if (fileName == "") {
        return;
    }

    // Windowsで.wavならSimpleAudioEngineを使用する
    auto isWav = _stricmp(getExtension(fileName), ".wav") == 0 ? 1 : 0;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 && isWav == 1)
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(fileName.c_str());
#else
    // 音量0で再生し、キャッシュさせる
    playSe(fileName, false, 0);
#endif
}

// 効果音のキャッシュを解放する
void AudioManager::releaseSe(const std::string baseName) {

    std::string fileName = getFileName(baseName);
    if (fileName == "") {
        return;
    }

    // Windowsで.wavならSimpleAudioEngineを使用する
    auto isWav = _stricmp(getExtension(fileName), ".wav") == 0 ? 1 : 0;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 && isWav == 1)
    CocosDenshion::SimpleAudioEngine::getInstance()->unloadEffect(fileName.c_str());
#else
    // iOSでの解放
    AudioEngine::uncache(fileName);
#endif
}

// 効果音を再生する
int AudioManager::playSe(const std::string baseName, int chunkNo, bool roop, float volume) {
    
    int soundId = -1;
    bool chunkFlag = false;
    
    std::string fileName = getFileName(baseName);
    if (fileName == "") {
        return soundId;
    }
    
    // チャンクが指定されていたら
    if (chunkNo >= 0 && chunkNo < sizeof(_chunk) / sizeof(_chunk[0])) {
        chunkFlag = true;
        
        // 指定チャンクの再生中の音を停止
        this->stopSe(_chunk[chunkNo]);
    }

    // Windowsで.wavならSimpleAudioEngineを使用する
    auto isWav = _stricmp(getExtension(fileName), ".wav") == 0 ? 1 : 0;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 && isWav == 1)
        soundId = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(fileName.c_str(), roop);
#else
        soundId = AudioEngine::play2d(fileName, roop, volume);
#endif
    
    if (chunkFlag) {
        // チャンクにSoundIdを登録
        _chunk[chunkNo] = soundId;
    }

    return soundId;
    
}

int AudioManager::playSe(const std::string baseName, bool roop, float volume) {
    
    return this->playSe(baseName, -1, roop, volume);
}

int AudioManager::playSe(const std::string baseName, bool roop) {

    return this->playSe(baseName, roop, _seVolume);

}

// チャンク指定で再生(0～7)
int AudioManager::playSe(const std::string fileName, int chunkNo) {
    return this->playSe(fileName, chunkNo, false, _seVolume);
}

// 効果音を停止する
void AudioManager::stopSe(int soundId) {

    // Windows版wav用にこれも実行しておく
    CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(soundId);
    
    AudioEngine::stop(soundId);

}
