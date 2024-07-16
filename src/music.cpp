#include "music.hpp"
#include <iostream>
#include "framework/common.hpp"
SoundPlayer::SoundPlayer() : initialized(false), playing(false) {}

SoundPlayer::~SoundPlayer() {
    stopSound();
    cleanup();
}

bool SoundPlayer::init() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    initialized = true;
    return true;
}

void SoundPlayer::playSound(const std::string& file) {
    if (!initialized) {
        std::cerr << "SoundPlayer is not initialized!" << std::endl;
        return;
    }

    if (playing) {
        stopSound();
    }

    playing = true;
    playbackThread = std::thread(&SoundPlayer::playSoundInternal, this, Common::absolutePath(file));
}

void SoundPlayer::stopSound() {
    if (playing) {
        playing = false;
        if (playbackThread.joinable()) {
            playbackThread.join();
        }
    }
}

void SoundPlayer::playSoundInternal(const std::string& file) {
    Mix_Chunk* sound = Mix_LoadWAV(file.c_str());
    if (sound == nullptr) {
        std::cerr << "Failed to load sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return;
    }

    Mix_PlayChannel(-1, sound, 0);
    while (playing && Mix_Playing(-1) != 0) {
        SDL_Delay(100);
    }

    Mix_HaltChannel(-1);
    Mix_FreeChunk(sound);
}

void SoundPlayer::cleanup() {
    if (initialized) {
        Mix_Quit();
        SDL_Quit();
        initialized = false;
    }
}
