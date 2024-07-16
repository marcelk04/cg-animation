#ifndef SOUND_PLAYER_HPP
#define SOUND_PLAYER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <thread>
#include <atomic>

class SoundPlayer {
public:
    SoundPlayer();
    ~SoundPlayer();

    bool init();
    void playSound(const std::string& file);
    void stopSound();
    void cleanup();

private:
    void playSoundInternal(const std::string& file);

    bool initialized;
    std::atomic<bool> playing;
    std::thread playbackThread;
};

#endif // SOUND_PLAYER_HPP
