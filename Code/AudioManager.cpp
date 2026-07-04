#include "AudioManager.h"

AudioManager::AudioManager() {
    m_music.setLoop(true);
    m_music.setVolume(m_volume);
}

const char* AudioManager::trackPath(MusicTrack t) const {
    switch (t) {
        case MusicTrack::Menu: return "assets/sounds/theme_menu.ogg";
        case MusicTrack::Gameplay: return "assets/sounds/theme_gameplay.ogg";
        case MusicTrack::LevelEvent: return "assets/sounds/level_complete.ogg";
        case MusicTrack::Boss:  return "assets/sounds/boss_music.ogg";
        default: return "";
    }
}

void AudioManager::play(MusicTrack track) {
    if (track == m_current &&
        m_music.getStatus() == sf::Music::Playing) return;

    if (track == MusicTrack::None) { stop(); return; }

    m_current = track;
    const char* path = trackPath(track);
    if (path[0] == '\0') return;

    m_music.stop();
    if (m_music.openFromFile(path)) {
        m_music.setLoop(true);
        m_music.setVolume(m_volume);
        m_music.play();
    }
}

void AudioManager::stop() {
    m_music.stop();
    m_current = MusicTrack::None;
}

void AudioManager::setVolume(float vol) {
    m_volume = vol;
    m_music.setVolume(vol);
}
