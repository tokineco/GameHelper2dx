// BGM: Win32-ogg, wav use SimpleAudioEngine. Other format use AudioEngine.
// SE : Win32-wav use SimpleAudioEngine. Other format use AudioEngine.

#pragma execution_character_set("utf-8")

#include "SimpleAudioEngine.h"              // Windows-ogg, wav
#include "audio/include/AudioEngine.h"      // iOS, Android
#include "json/rapidjson.h"
#include "json/document.h"

#include "AudioManager.h"

USING_NS_CC;
using namespace experimental;

AudioManager* AudioManager::_instance = nullptr;
cocos2d::Scheduler* AudioManager::_scheduler = nullptr;

// コンストラクタ
AudioManager::AudioManager()
    : _audioListFile("")
    , _bgmVolume(0.5f)
    , _seVolume(0.6f)
    , _fadeCondition(FadeType::NONE)
    , _bgmFadeVolumeNow(0)
    , _bgmFadeVolumeFrom(0)
    , _bgmFadeVolumeTo(0)
    , _bgmFadeTime(0)
    , _stopBgmReleaseFlg(true)
{
    // チャンク配列の初期化
    for(int i=0; i < sizeof(_chunk) / sizeof(_chunk[0]); i++) {
        _chunk[i] = AudioEngine::INVALID_AUDIO_ID;
    }
}

// デストラクタ
AudioManager::~AudioManager() {
    CC_SAFE_RELEASE_NULL(_scheduler);
}

// 初期化
AudioManager* AudioManager::getInstance() {

    if (_instance == nullptr) {
        _instance = new AudioManager();

        // スケジューラをインスタンスに追加し、updateを呼び出せるようにする
        _scheduler = Director::getInstance()->getScheduler();
        _scheduler->retain();
        _scheduler->scheduleUpdate(_instance, 0, false);
	}

	return _instance;
}

// 削除する際に使用
void AudioManager::deleteInstance() {

    if (_instance != nullptr) {
		delete _instance;
	}

    _instance = nullptr;
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
std::string AudioManager::getFileName(AudioType type, std::string baseName) {

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

            // mp3> ogg > wav
            if (FileUtils::getInstance()->isFileExist(baseName + ".mp3")) {
                ext = ".mp3";
            }
            else if (FileUtils::getInstance()->isFileExist(baseName + ".ogg")) {
                ext = ".ogg";
            }
            break;
        case cocos2d::ApplicationProtocol::Platform::OS_ANDROID:

            // m4a > ogg > mp3 > wav
            if (FileUtils::getInstance()->isFileExist(baseName + ".m4a")) {
                ext = ".m4a";
            } else if (FileUtils::getInstance()->isFileExist(baseName + ".ogg")) {
                ext = ".ogg";
            } else if (FileUtils::getInstance()->isFileExist(baseName + ".mp3")) {
                ext = ".mp3";
            }
            break;
        case cocos2d::ApplicationProtocol::Platform::OS_IPHONE:
        case cocos2d::ApplicationProtocol::Platform::OS_IPAD:

            // m4a > caf > mp3 > wav
            if (FileUtils::getInstance()->isFileExist(baseName + ".m4a")) {
                ext = ".m4a";
            } else if (FileUtils::getInstance()->isFileExist(baseName + ".caf")) {
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

// SimpleEngineを使うかどうか
bool AudioManager::isSimpleAudioEngine(AudioType type, const std::string fileName) {

    bool isWav = false;

    auto fu = FileUtils::getInstance();

    if (type == AudioType::BGM) {
        isWav = (fu->getFileExtension(fileName).compare(".wav") || fu->getFileExtension(fileName).compare(".ogg")) == 0 ? 1 : 0;
    } else {
        isWav = (fu->getFileExtension(fileName).compare(".wav") == 0) ? 1 : 0;
    }

    Application::Platform platform = Application::getInstance()->getTargetPlatform();
    if (platform == Application::Platform::OS_WINDOWS && isWav == 1) {
        return true;
    }

    return false;
}

// AudioEngine全てのキャッシュを削除する
void AudioManager::releaseAll() {
    AudioEngine::uncacheAll();
}

// 毎フレーム実行
void AudioManager::update(float dt) {

    // フェードイン、アウトを実行する
    switch (_fadeCondition) {
        case FadeType::FADE_IN:
        case FadeType::FADE_IN_RESUME:
            // 0除算回避
            if (_bgmFadeTime == 0) {
                _bgmFadeTime = 0.01f;
            }
            // dt時間後の増分ボリュームを求める。 _bgmVolume:_bgmFadeTime = dV : dt
            _bgmFadeVolumeNow += (dt * (_bgmFadeVolumeTo - _bgmFadeVolumeFrom)) / _bgmFadeTime;

            if (_bgmFadeVolumeNow >= _bgmFadeVolumeTo) {
                _bgmFadeVolumeNow = _bgmFadeVolumeTo;
                _bgmFadeVolumeFrom = _bgmFadeVolumeTo;
                _fadeCondition = FadeType::NONE;
            }

            this->setBgmVolume(_bgmFadeVolumeNow, false);
            break;
        case FadeType::FADE_OUT:
        case FadeType::FADE_OUT_PAUSE:
            // 0除算回避
            if (_bgmFadeTime == 0) {
                _bgmFadeTime = 0.01f;
            }
            // dt時間後の減分ボリュームを求める。 _bgmVolume:_bgmFadeTime = dV : dt
            _bgmFadeVolumeNow += (dt * (_bgmFadeVolumeTo - _bgmFadeVolumeFrom)) / _bgmFadeTime;

            if (_bgmFadeVolumeNow <= _bgmFadeVolumeTo) {
                _bgmFadeVolumeNow = _bgmFadeVolumeTo;
                _bgmFadeVolumeFrom = _bgmFadeVolumeTo;

                if (_fadeCondition == FadeType::FADE_OUT) {
                    // stopBgmを実行
                    stopBgm(0, _stopBgmReleaseFlg);
                } else if (_fadeCondition == FadeType::FADE_OUT_PAUSE) {
                    // pauseBgmを実行
                    pauseBgm(0);
                }

                _fadeCondition = FadeType::NONE;
            }

            this->setBgmVolume(_bgmFadeVolumeNow, false);
            break;
        default:
            break;
    }
}


// BGMとSEの音量の初期化
void AudioManager::initVolume(float bgmVolume, float seVolume) {
    _bgmVolume = bgmVolume;
    _seVolume = seVolume;
}

//===================
// BGM
//===================

// BGMのPreLoad
void AudioManager::preloadBgm(const std::string baseName) {

    std::string fileName = getFileName(AudioType::BGM, baseName);
    if (fileName == "") {
        return;
    }

    if (isSimpleAudioEngine(AudioType::BGM, fileName)) {
        CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(fileName.c_str());
        // iOSでなぜかBGMが再生されてしまうのを防ぐため、stopしておく
        CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    } else {
        AudioEngine::preload(fileName);
    }

}

// BGMの再生
int AudioManager::playBgm(const std::string baseName, float fadeTime /* =0*/, bool roop /* = true*/) {
    return playBgm(baseName, fadeTime, roop, _bgmVolume);
}
// BGMの再生
int AudioManager::playBgm(const std::string baseName, float fadeTime, bool roop, float volume) {

    int soundId = AudioEngine::INVALID_AUDIO_ID;

    std::string fileName = getFileName(AudioType::BGM, baseName);
    if (fileName == "") {
        return soundId;
    }

    // 前回のBGMを停止
    stopBgm();

    // フェード指定の場合
    if (fadeTime != 0) {
        _fadeCondition = FadeType::FADE_IN;
        _bgmFadeVolumeNow = 0;
        _bgmFadeVolumeFrom = 0;
        _bgmFadeTime = fadeTime;
    } else {
        _fadeCondition = FadeType::NONE;
        _bgmFadeVolumeNow = volume;
    }
    _bgmFadeVolumeTo = volume;


    if (isSimpleAudioEngine(AudioType::BGM, fileName)) {
        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(fileName.c_str(), roop);
    } else {
        if (_bgmFileName == baseName && AudioEngine::getState(_bgmId) == AudioEngine::AudioState::PLAYING) {
            // 前回と同じファイル名で、再生中の場合は無視する
            return _bgmId;
        }

        _bgmId = AudioEngine::play2d(fileName, roop, volume);
        _bgmFileName = baseName;
    }
    return _bgmId;
}

// BGMを一時停止する
void AudioManager::pauseBgm(float fadeTime /*= 0*/) {

    _bgmFadeVolumeTo = 0;

    if (fadeTime != 0) {
        // フェード指定の場合
        _fadeCondition = FadeType::FADE_OUT_PAUSE;
        _bgmFadeVolumeNow = _bgmVolume;
        _bgmFadeVolumeFrom = _bgmVolume;
        _bgmFadeTime = fadeTime;
    } else {
        // フェードなしの場合
        _fadeCondition = FadeType::NONE;
        _bgmFadeVolumeNow = 0;

        pauseBgmEngine();
    }
    
}

// pauseBgmの実行(fadeなし、またはupdateによるフェード後に実行される)
void AudioManager::pauseBgmEngine() {

    // Windows版wav用にこれも実行しておく
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    AudioEngine::pause(_bgmId);
}

// BGMをリジューム再生する
void AudioManager::resumeBgm(float fadeTime /*=0*/) {

    // フェード指定の場合
    if (fadeTime != 0) {
        _fadeCondition = FadeType::FADE_IN_RESUME;
        _bgmFadeVolumeNow = 0;
        _bgmFadeVolumeFrom = 0;
        _bgmFadeTime = fadeTime;
    } else {
        _fadeCondition = FadeType::NONE;
        _bgmFadeVolumeNow = _bgmVolume;
    }
    _bgmFadeVolumeTo = _bgmVolume;

    // Windows版wav用にこれも実行しておく
    CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    AudioEngine::resume(_bgmId);
}

// BGMを停止する
void AudioManager::stopBgm(float fadeTime /*= 0*/, bool release /* = true */) {

    _bgmFadeVolumeTo = 0;

    if (fadeTime != 0) {
        // フェード指定の場合
        _fadeCondition = FadeType::FADE_OUT;
        _bgmFadeVolumeNow = _bgmVolume;
        _bgmFadeVolumeFrom = _bgmVolume;
        _bgmFadeTime = fadeTime;
        _stopBgmReleaseFlg = release;
    } else {
        // フェードなしの場合
        _fadeCondition = FadeType::NONE;
        _bgmFadeVolumeNow = 0;

        stopBgmEngine(release);
    }
}

// stopBgmの実行(fadeなし、またはupdateによるフェード後に実行される)
void AudioManager::stopBgmEngine(bool release /* = true */) {

    // Windows版wav用にこれも実行しておく
    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic(release);
    AudioEngine::stop(_bgmId);

    // キャッシュ解放
    if (release) {
        releaseBgm();
    }

    _bgmId = AudioEngine::INVALID_AUDIO_ID;
    _bgmFileName = "";

}

// BGMが再生されているかどうか
bool AudioManager::isPlayingBgm() {

    std::string fileName = getFileName(AudioType::BGM, _bgmFileName);

    if (isSimpleAudioEngine(AudioType::BGM, fileName)) {
        return CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
    } else {
        AudioEngine::AudioState state = AudioEngine::getState(_bgmId);
        if (state == AudioEngine::AudioState::PLAYING) {
            return true;
        }
    }

    return false;
}

// BGMの音量を変更する
void AudioManager::setBgmVolume(float volume, bool save /* = true */) {

    // 変数保持フラグがonの場合は変数を切り替える
    if (save) {
        _bgmVolume = volume;
    }

    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(volume);
    AudioEngine::setVolume(_bgmId, volume);
}

// BGMの音量を取得する
float AudioManager::getBgmVolume() {
    return _bgmVolume;
}

// BGMのキャシュを解放する
void AudioManager::releaseBgm() {
    AudioEngine::uncache(_bgmFileName);
}

//===================
// SE
//===================

// 効果音のPreLoad
void AudioManager::preloadSe(const std::string baseName) {

    std::string fileName = getFileName(AudioType::SE, baseName);
    if (fileName == "") {
        return;
    }

    if (isSimpleAudioEngine(AudioType::SE, fileName)) {
        CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(fileName.c_str());
    } else {
        AudioEngine::preload(fileName);
    }
}

// 効果音を再生する
int AudioManager::playSe(const std::string baseName, int chunkNo) {
    return this->playSe(baseName, chunkNo, false, _seVolume);
}
// 効果音を再生する
int AudioManager::playSe(const std::string baseName, int chunkNo, bool roop, float volume) {

    int soundId = AudioEngine::INVALID_AUDIO_ID;
    bool chunkFlag = false;

    std::string fileName = getFileName(AudioType::SE, baseName);
    if (fileName == "") {
        return soundId;
    }

    // チャンクが指定されていたら
    if (chunkNo >= 0 && chunkNo < sizeof(_chunk) / sizeof(_chunk[0])) {
        chunkFlag = true;

        // 指定チャンクの再生中の音を停止
        this->stopSe(_chunk[chunkNo]);
    }

    if (isSimpleAudioEngine(AudioType::SE, fileName)) {
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
// 効果音を再生する
int AudioManager::playSe(const std::string baseName, bool roop, float volume) {

    return this->playSe(baseName, -1, roop, volume);
}
// 効果音を再生する
int AudioManager::playSe(const std::string baseName, bool roop /* = false */) {
    return this->playSe(baseName, roop, _seVolume);
}

// 効果音を停止する
void AudioManager::stopSe(int soundId) {

    // Windows版wav用にこれも実行しておく
    CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(soundId);

    AudioEngine::stop(soundId);
}

// 効果音の音量を変更する
void AudioManager::setSeVolume(float volume) {
    _seVolume = volume;
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(_seVolume);
    //AudioEngine::setVolume(soundId, _seVolume);
}

// 効果音の音量を取得する
float AudioManager::getSeVolume() {
    return _seVolume;
}

// 効果音のキャッシュを解放する
void AudioManager::releaseSe(const std::string baseName) {

    std::string fileName = getFileName(AudioType::SE, baseName);
    if (fileName == "") {
        return;
    }

    if (isSimpleAudioEngine(AudioType::SE, fileName)) {
        CocosDenshion::SimpleAudioEngine::getInstance()->unloadEffect(fileName.c_str());
    } else {
        AudioEngine::uncache(fileName);
    }
}


// AudioEngineを解放する
void AudioManager::endAudioEngine() {
    AudioEngine::end();
    CocosDenshion::SimpleAudioEngine::getInstance()->end();
}