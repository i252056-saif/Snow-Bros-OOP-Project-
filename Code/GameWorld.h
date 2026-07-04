#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"
#include "Player.h"
#include "Enemy.h"
#include "Projectile.h"
#include "PowerUp.h"
#include "Platform.h"
#include "LevelConfig.h"
#include "HUD.h"
#include "Database.h"

enum class game_state {
    MainMenu,
    Login,
    Login2,          // Player 2 login (multiplayer)
    Register,
    CharacterSelect,
    Playing,
    Paused,
    LevelComplete,
    GameOver,
    Leaderboard,
    Shop
};

/*
 class GameWorld
  brief Manages all gameplay for one level.
  Supports 1 or 2 players. Uses Factory pattern (make_enemy) and raw arrays.
 */
class GameWorld {
public:
    /*Construct world. Pass empty string for username2 in single player. */
    GameWorld(sf::Font& font,
              const char* username,
              const char* username2,
              database& db,
              bool multiplayer,
              int charSel1,
              int charSel2);
    ~GameWorld();

    void load_level (int level_number);
    void handle_event (const sf::Event& event);
    void update (float dt);
    void draw (sf::RenderTarget& target);

    bool is_level_complete() const 
    { return m_level_complete; }
    bool is_game_over() const 
    { return m_game_over; }
    bool is_paused() const 
    { return m_paused; }
    bool is_boss_level() const 
    { return m_is_boss_level; }
    bool request_shop() const 
    { return m_open_shop; }
    void clear_shop_request()      
    { m_open_shop = false; }
    int  get_shop_requester()const 
    { return m_shop_requester; }
    int  get_current_level() const 
    { return m_current_level; }

    /* Returns player by index (0 = P1, 1 = P2). */
    Player& get_player(int idx = 0)       
    { return m_players[idx < m_player_count ? idx : 0]; }
    const Player& get_player(int idx = 0) const 
    { return m_players[idx < m_player_count ? idx : 0]; }

private:
    sf::Font&  m_font;
    char m_username[MAX_STR];
    char m_username2[MAX_STR];
    database&  m_db;
    bool m_multiplayer;

    Player m_players[MAX_PLAYERS];
    int m_player_count = 0;

    enemy* m_enemies [MAX_ENEMIES];      
    int m_enemy_count = 0;
    projectile* m_projectiles[MAX_PROJECTILES]; 
    int m_proj_count  = 0;
    power_up* m_powerups [MAX_POWERUPS];     
    int m_pu_count    = 0;
    platform m_platforms [MAX_PLATFORMS];    
    int m_plat_count  = 0;

    hud   m_hud;
    int   m_current_level = 1;
    bool  m_level_complete = false;
    bool  m_game_over = false;
    bool  m_paused = false;
    bool  m_open_shop  = false;
    bool  m_show_hit_boxes = false;
    bool  m_is_boss_level  = false;
    bool  m_is_bonus_level = false;
    int   m_shop_requester = 0;

    sf::Color  
        m_bg_color = sf::Color(20, 20, 60);

    // Background image for gameplay view
    sf::Texture m_bg_texture;
    sf::Sprite  m_bg_sprite;
    bool m_bg_loaded = false;

    mogera* m_boss = nullptr;
    gamakichi* m_gamakichi_boss = nullptr;

    enemy* make_enemy(const enemy_spawn& spawn);

    void resolve_snowball_vs_enemies();
    void resolve_rolling_vs_enemies();
    void resolve_enemy_vs_player();
    void resolve_power_up_vs_player();
    void resolve_knife_vs_player();
    void resolve_explosion_vs_player();
    void harvest_rocket_explosions();
    void harvest_pending_spawns();

    void cleanup_defeated();
    void check_level_complete();
    void spawn_power_up(float x, float y);

    void add_projectile(projectile* p);
    void add_enemy (enemy* e);
    void add_power_up(power_up* p);

    void clear_all();
};
