// BGM: Win32-ogg, wav use SimpleAudioEngine. Other format use AudioEngine.
// SE: Win32-wav use SimpleAudioEngine. Other format use AudioEngine.

#pragma execution_character_set("utf-8")

#include "SimpleAudioEngine.h"              // Windows-wav
#include "audio/include/AudioEngine.h"      // iOS, Android
#include "json/rapidjson.h"
#include "json/document.h"

#include "AudioManager.h"

USING_NS_CC;
using namespace experimental;

AudioManager* AudioManager::_instance = NULL;

// コンストラクタ
AudioManager::AudioManager()
    : _audioListFile("")
    , _bgmVolume(0.5f)
    , _seVolume(0.6f)
{
    // チャンク配列の初期化
    for(int i=0; i < sizeof(_chunk) / sizeof(_chunk[0]); i++) {
        _chunk[i] = AudioEngine::INVALID_AUDIO_ID;
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


// オーディオ管理ファイルを読み込む
bool AudioManager::readAudioListFile(const std::string fileName) {

    // Resourceの中にあるファイルを読み込む
    // ファイル読み込み
    std::string strData = FileUtils::getInstance()->getStringFromFile(fileName);

    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseDefaultFlags>(strData.c_str());

    if (doc.HasParseError()) {
        // 解析エラー
        log("JSON error : %u", doc.GetParseError());
        return false;
    }

    if (doc.IsObject()) {

        log("%s", strData.c_str());

        // 初期化
        _bgmList.clear();
        _seList.clear();

        // BGM
        rapidjson::Value& bgms = doc["BGM"];

        // キーと値をリストに登録する
        for (rapidjson::Value::ConstMemberIterator it = bgms.MemberBegin(); it != bgms.MemberEnd(); it++) {
            std::string key = it->name.GetString();
            const rapidjson::Value& value = it->value;
            if (value.GetType() == rapidjson::kStringType) {
                _bgmList[key] = value.GetString();
            }
        }

        // SE
        rapidjson::Value& ses = doc["SE"];
        // キーと値をリストに登録する
        for (rapidjson::Value::ConstMemberIterator it = ses.MemberBegin(); it != ses.MemberEnd(); it++) {
            std::string key = it->name.GetString();
            const rapidjson::Value& value = it->value;
            if (value.GetType() == rapidjson::kStringType) {
                _seList[key] = value.GetString();
            }
        }

        // 現在のファイルをセット
        _audioListFile = fileName;

        return true;
    }
    return false;
}


// 端末ごとに読み込む拡張子を変えて、そのファイル名を返す
std::string AudioManager::getFileName(std::string baseName, AudioType type) {

    auto platform = Application::getInstance()->getTargetPlatform();

    std::string ext = ".wav";               // 拡張子

    // オーディオ管理ファイルを使う場合、キーからファイル名を取得する
    if (_audioListFile != "") {
        if (type == AudioType::BGM) {
            if (_bgmList.count(baseName) != 0) {
                baseName = _bgmList[baseName];
            }
        } else if (type == AudioType::SE) {
            if (_seList.count(baseName) != 0) {
                baseName = _seList[baseName];
            }
        }
    }

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
    log("file not found %s.", baseName.c_str());
    return baseName;

}


// 拡張子を取得する
std::string AudioManager::getExtension(const std::string fileName) {
    auto chars = strchr(fileName.c_str(), '.');
    if (chars != nullptr) {
        return StringUtils::toString(chars);
    }

    return "";
}

// AudioEngine全てのキャッシュを削除する
void AudioManager::releaseAll() {
    AudioEngine::uncacheAll();
}


//===================
// BGM
//===================

// BGMのPreLoad
void AudioManager::preloadBgm(const std::string baseName) {

    std::string fileName = getFileName(baseName, AudioType::BGM);
    if (fileName == "") {
        return;
    }

    // Windowsで.wav, .oggならSimpleAudioEngineを使用する
    auto isWav = (getExtension(fileName).compare(".wav") || getExtension(fileName).compare(".ogg")) == 0 ? 1 : 0;

    Application::Platform platform = Application::getInstance()->getTargetPlatform();
    if (platform == Application::Platform::OS_WINDOWS && isWav == 1) {
        CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(fileName.c_str());
    } else {
        // 音量0で再生し、キャッシュさせる
        playBgm(fileName, false);
        stopBgm();
    }
}

// BGMの再生
int AudioManager::playBgm(const std::string baseName, float fadeTime /* =0*/, bool roop /* = true*/) {

    int soundId = AudioEngine::INVALID_AUDIO_ID;

    std::string fileName = getFileName(baseName, AudioType::BGM);
    if (fileName == "") {
        return soundId;
    }

    // Windowsで.wav, .oggならSimpleAudioEngineを使用する
    auto isWav = (getExtension(fileName).compare(".wav") == 0 || getExtension(fileName).compare(".ogg") == 0) ? 1 : 0;

    Application::Platform platform = Application::getInstance()->getTargetPlatform();
    if (platform == Application::Platform::OS_WINDOWS && isWav == 1) {
        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(fileName.c_str(), roop);
    } else {
        if (_bgmFileName == baseName && AudioEngine::getState(_bgmId) == AudioEngine::AudioState::PLAYING) {
            // 前回と同じファイル名で、再生中の場合は無視する
            return _bgmId;
        }
        // 前回のBGMを停止
        stopBgm();

        _bgmId = AudioEngine::play2d(fileName, roop, _bgmVolume);
        _bgmFileName = baseName;
    }
    return _bgmId;
}

// BGMを停止する
void AudioManager::stopBgm(float fadeTime /*= 0*/, bool release /* = true */) {

    // Windows版wav用にこれも実行しておく
    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    AudioEngine::stop(_bgmId);

    // キャッシュ解放
    if (release == true) {
        releaseBgm();
    }
}

// BGMのキャシュを解放する
void AudioManager::releaseBgm() {
    AudioEngine::uncache(_bgmFileName);
    _bgmId = AudioEngine::INVALID_AUDIO_ID;
    _bgmFileName = "";
}

//===================
// SE
//===================

// 効果音のPreLoad
void AudioManager::preloadSe(const std::string baseName) {

    std::string fileName = getFileName(baseName, AudioType::SE);
    if (fileName == "") {
        return;
    }

    // Windowsで.wavならSimpleAudioEngineを使用する
    auto isWav = getExtension(fileName).compare(".wav") == 0 ? 1 : 0;

    Application::Platform platform = Application::getInstance()->getTargetPlatform();
    if (platform == Application::Platform::OS_WINDOWS && isWav == 1) {
        CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(fileName.c_str());
    } else {
        // 音量0で再生し、キャッシュさせる
        playSe(fileName, false, 0);
    }
}

// 効果音を再生する
int AudioManager::playSe(const std::string baseName, int chunkNo, bool roop, float volume) {
    
    int soundId = AudioEngine::INVALID_AUDIO_ID;
    bool chunkFlag = false;
    
    std::string fileName = getFileName(baseName, AudioType::SE);
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
    auto isWav = getExtension(fileName).compare(".wav") == 0 ? 1 : 0;

    Application::Platform platform = Application::getInstance()->getTargetPlatform();
    if (platform == Application::Platform::OS_WINDOWS && isWav == 1) {
        soundId = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(fileName.c_str(), roop);
    } else {
        soundId = AudioEngine::play2d(fileName, roop, volume);
    }
    
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

// 効果音のキャッシュを解放する
void AudioManager::releaseSe(const std::string baseName) {

    std::string fileName = getFileName(baseName, AudioType::SE);
    if (fileName == "") {
        return;
    }

    // Windowsで.wavならSimpleAudioEngineを使用する
    auto isWav = getExtension(fileName).compare(".wav") == 0 ? 1 : 0;

    Application::Platform platform = Application::getInstance()->getTargetPlatform();
    if (platform == Application::Platform::OS_WINDOWS && isWav == 1) {
        CocosDenshion::SimpleAudioEngine::getInstance()->unloadEffect(fileName.c_str());
    } else {
        // iOSでの解放
        AudioEngine::uncache(fileName);
    }
}
