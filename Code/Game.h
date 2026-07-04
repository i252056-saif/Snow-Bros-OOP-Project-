#pragma once
#include <SFML/Graphics.hpp>
#include "Database.h"
#include "GameWorld.h"
#include "AudioManager.h"
#include "Constants.h"


 //class game
 // brief Top-level application. 
 // window, font, database, AudioManager,
 // background texture, character textures, and GameWorld.
 // Drives the game_state state machine. Supports 1 or 2-player modes.
class game {
public:
    game();
    ~game();
    void run();

private:
    sf::RenderWindow m_window;
    sf::Font         m_font;
    database         m_db;
    AudioManager     m_audio;
    GameWorld*       m_world = nullptr;

    // Background
    sf::Texture        m_bg_texture;
    sf::Sprite         m_bg_sprite;          // stretch to window
    sf::RectangleShape m_bg_overlay;         // transparent  menus

    // Character (Nick=0, Tom=1, Nova=2)
    sf::Texture m_char_tex[3];
    sf::Sprite  m_char_sprite[3];
    bool        m_chars_loaded = false;

    // Game state
    game_state m_state = game_state::MainMenu;
    char m_username[MAX_STR];
    char m_username2[MAX_STR];
    int m_current_level = 1;
    bool m_guest_mode = false;
    bool m_multiplayer = false;
    MusicTrack m_last_track = MusicTrack::None;

    //Per state event handler
    void update_main_menu (const sf::Event& e);
    void update_login (const sf::Event& e);
    void update_login2 (const sf::Event& e);
    void update_register (const sf::Event& e);
    void update_char_select (const sf::Event& e);
    void update_playing (const sf::Event& e);
    void update_game_over (const sf::Event& e);
    void update_leaderboard (const sf::Event& e);
    void update_shop (const sf::Event& e);

    // Per state draw
    void draw_background(float alpha = 0.55f);
    void draw_main_menu();
    void draw_login();
    void draw_login2();
    void draw_register();
    void draw_char_select();
    void draw_game_over();
    void draw_leaderboard();
    void draw_shop();

    // Audio helper
    void update_music();       

    // UI helpers
    sf::Text make_text(const char* str, unsigned size,
                       sf::Color col, float x, float y,
                       bool centered = false);
    void draw_panel(float x, float y, float w, float h);
    void draw_field(const char* label, const char* value,
                    bool active, bool mask,
                    float x, float y);

    // login / register form
    struct InputState {
        char username[MAX_STR];
        char password[MAX_STR];
        char email[MAX_STR];
        char message[MAX_STR];
        int  active_field = 0;

        void clear() {
            for (int i = 0; i < MAX_STR; ++i) {
                username[i] = 0; 
                password[i] = 0;
                email[i] = 0; 
                message[i]= 0;
            }
            active_field = 0;
        }
    } m_input, m_input2;

    void input_append    (char* field, char c) const;
    void input_backspace (char* field) const;

    int  m_menu_sel  = 0;
    int  m_char_sel  = 0;
    int  m_char_sel2 = 0;
    int  m_char_step = 0;
    int  m_shop_sel  = 0;
    int  m_shop_player = 0;

    void load_assets();
    void start_new_game();
    void advance_level();
    void destroy_world();
};
