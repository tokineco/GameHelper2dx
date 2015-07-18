// BGM: Win32-ogg, wav use SimpleAudioEngine. Other format use AudioEngine.
// SE: Win32-wav use SimpleAudioEngine. Other format use AudioEngine.

#ifndef __AudioManager__
#define __AudioManager__

#include "cocos2d.h"

class AudioManager {

public:
    
    // SE用チャンク
    int _chunk[8];
    
private:
	
	AudioManager();
	static AudioManager* _instance;

    // 環境に応じて拡張子付きファイル名に変換する
    std::string getFileName(const std::string baseName);
    // 拡張子を取得する
    std::string getExtension(const std::string fileName);
    
    // BGMは1種類のみ
    int _bgmId = -1;
    // 同じファイル名の場合は無視
    std::string _bgmFileName = "";

    // BGM音量
    CC_SYNTHESIZE(float, _bgmVolume, BgmVolume);
    // SE音量
    CC_SYNTHESIZE(float, _seVolume, SeVolume);
    
public:

	~AudioManager();
	static AudioManager* getInstance();
	static void deleteInstance();
	
    void preloadBgm(const std::string baseName);
    void releaseBgm(const std::string baseName);
    int playBgm(const std::string baseName, float fadeTime = 0, bool roop = true);
    int playBgm(const std::string baseName, float fadeTime, bool roop, float volume);
    void stopBgm(float fadeTime = 0);

    void preloadSe(const std::string baseName);
    void releaseSe(const std::string baseName);
    int playSe(const std::string baseName, int chunkNo);
    int playSe(const std::string baseName, bool roop = false);
    int playSe(const std::string baseName, int chunkNo, bool roop, float volume);
    int playSe(const std::string baseName, bool roop, float volume);
    void stopSe(int soundId);

};

#endif /* defined(__AudioManager__) */