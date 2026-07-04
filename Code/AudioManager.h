#pragma once
#include <SFML/Audio.hpp>

/*
 class AudioManager
  Handles background music switching.
 
 Music tracks:
  theme_menu.ogg       main menu, login, register, leaderboard screens
  theme_gameplay.ogg   standard levels 1-4, 6-9
  level_complete.ogg   character select, level complete, shop, game over
  boss_music.ogg       boss levels 5 and 10
 
  Uses sf::Music 
  Volume is fixed at 60% for music.
 */

enum class MusicTrack {
    None,
    Menu,       // snow_bros_theme_01  (theme_menu.ogg)
    Gameplay,   // snow_bros_theme_02  (theme_gameplay.ogg)
    LevelEvent, // snow_bros_level     (level_complete.ogg)
    Boss        // final_boss          (boss_music.ogg)
};

class AudioManager {
public:
    AudioManager();

    //Play the requested track
    void play(MusicTrack track);

    //Stop all music immediately.
    void stop();

    void setVolume(float vol);
    float getVolume() const { return m_volume; }

private:
    sf::Music m_music;
    MusicTrack m_current = MusicTrack::None;
    float      m_volume  = 60.f;

    const char* trackPath(MusicTrack t) const;
};
