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
    const char* getExtension(const std::string fileName);
    
    // SE音量
    CC_SYNTHESIZE(float, _seVolume, SeVolume);
    
public:

	~AudioManager();
	static AudioManager* getInstance();
	static void deleteInstance();
	
    void preloadSe(const std::string fileName);
    void releaseSe(const std::string fileName);
    
    int playSe(const std::string fileName, int chunkNo);
    int playSe(const std::string fileName, bool roop = false);
    int playSe(const std::string fileName, int chunkNo, bool roop, float volume);
    int playSe(const std::string fileName, bool roop, float volume);
    void stopSe(int soundId);

};

#endif /* defined(__AudioManager__) */