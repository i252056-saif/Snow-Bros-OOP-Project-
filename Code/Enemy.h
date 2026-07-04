#pragma once
#include <SFML/Graphics.hpp>
#include "HitBox.h"
#include "Constants.h"

class platform;
class projectile;

enum class snow_level { None, Half, Full };
enum class enemy_variant { Red, Green, Blue, Purple };

class enemy {
public:
    enemy(float x, float y, float w, float h,
        enemy_variant variant = enemy_variant::Red);
    virtual ~enemy();

    virtual void update(float dt, platform* platforms, int platCount,
        float playerX, float playerY) = 0;
    virtual void draw(sf::RenderTarget& target, bool showHitBox) const = 0;
    virtual int  get_score_value() const = 0;

    void apply_snow();
    snow_level get_snow_level() const 
    { return m_snow_level; }
    bool is_encased()    const 
    { return m_snow_level == snow_level::Full; }

    bool is_defeated() const 
    { return m_defeated; }
    bool is_alive()  const 
    { return !m_defeated; }
    void defeat() 
    { m_defeated = true; }

    const hitbox& get_hitbox()  const 
    { return m_hitbox; }
    sf::Vector2f  getPosition() const 
    { return m_shape.getPosition(); }
    int           hits_to_encase() const;

    projectile* pending_projectiles[8];
    int         pending_projectile_count = 0;

    enemy* pending_enemies[8];
    int         pending_enemy_count = 0;

    void clearPending() {
        pending_projectile_count = 0;
        pending_enemy_count = 0;
    }

protected:
    sf::RectangleShape m_shape;
    sf::Sprite  m_sprite;
    bool m_use_sprite = false;

    hitbox m_hitbox;
    enemy_variant m_variant;

    float m_vx = 0.f, m_vy = 0.f;
    float m_w, m_h;
    bool  m_defeated = false;
    bool  m_on_ground = false;

    snow_level m_snow_level = snow_level::None;
    int m_snow_hits = 0;
    float m_encase_timer = 0.f;  

    bool  apply_gravity_and_collide(float dt, platform* platforms, int platCount);
    sf::Color variantColor(sf::Color base) const;
    void  draw_snow(sf::RenderTarget& target) const;
    void  setup_sprite(sf::Texture& tex, float w, float h, float x, float y);

    // BUG FIX
    void  update_sprite_flip();
};

// botom
class botom : public enemy {
public:
    botom(float x, float y, enemy_variant variant = enemy_variant::Red);

    void update(float dt, platform* platforms, int platCount,
        float playerX, float playerY) override;
    void draw(sf::RenderTarget& target, bool showHitBox) const override;
    int  get_score_value() const override;

protected:
    float m_dir_timer = 0.f;
    float m_dir_timeout = 2.5f;

private:
    static sf::Texture s_tex_red;
    static bool   s_loaded;
};

// flyingfoogafoog
class flyingfoogafoog : public botom {
public:
    flyingfoogafoog(float x, float y, enemy_variant variant = enemy_variant::Red);

    void update(float dt, platform* platforms, int platCount,
        float playerX, float playerY) override;
    void draw(sf::RenderTarget& target, bool showHitBox) const override;
    int  get_score_value() const override;

protected:
    bool  m_flying = false;
    float m_fly_timer = 0.f;
    float m_fly_duration = 3.f;
    float m_ground_timer = 0.f;
    float m_ground_duration = 4.f;
    float m_fly_vx = 0.f, m_fly_vy = 0.f;

private:
    static sf::Texture s_tex_red;
    static sf::Texture s_tex_blue;
    static bool        s_loaded;
};

// tornado
class tornado : public flyingfoogafoog {
public:
    tornado(float x, float y, enemy_variant variant = enemy_variant::Red);

    void update(float dt, platform* platforms, int platCount,
        float playerX, float playerY) override;
    void draw(sf::RenderTarget& target, bool showHitBox) const override;
    int  get_score_value() const override;

private:
    float m_knife_timer = 0.f;
    float m_knife_timeout = 3.5f;

    static sf::Texture s_tex_red;
    static sf::Texture s_tex_blue;
    static bool        s_loaded;
};

// mogera_child
class mogera_child : public enemy {
public:
    mogera_child(float x, float y, float dirX);
    void update(float dt, platform* platforms, int platCount,
        float playerX, float playerY) override;
    void draw(sf::RenderTarget& target, bool showHitBox) const override;
    int  get_score_value() const override { return 300; }

private:
    static sf::Texture s_tex;
    static bool s_loaded;
};

// mogera (Boss)
class mogera : public enemy {
public:
    mogera(float x, float y);


    void update(float dt, platform* platforms, int platCount,
        float playerX, float playerY) override;
    void draw(sf::RenderTarget& target, bool showHitBox) const override;
    int  get_score_value() const override 
    { return MOGERA_SCORE; }

    int  getMaxHealth() const 
    { return m_max_health; }
    int  getCurrentHealth() const 
    { return m_health; }
    void takeDamage() 
    { if (m_health > 0) 
        --m_health; 
    if (m_health <= 0) 
        m_defeated = true; }

private:
    int m_health = 15;
    int m_max_health = 15;
    float m_spawn_timer = 0.f;
    float m_spawn_rate = 3.0f;

    static sf::Texture s_tex;
    static bool        s_loaded;
};

// gamaakichi_child
class gamaakichi_child : public enemy {
public:
    gamaakichi_child(float x, float y, float dirX);

    void update(float dt, platform* platforms, int platCount,
        float playerX, float playerY) override;
    void draw(sf::RenderTarget& target, bool showHitBox) const override;
    int  get_score_value() const override { return 400; }

private:
    float m_hop_timer = 0.f;
    float m_hop_interval = 1.2f;
};

//  gamaakichi (Bara Boss)
class gamakichi : public enemy {
public:
    gamakichi(float x, float y);

    void update(float dt, platform* platforms, int platCount,
        float playerX, float playerY) override;
    void draw(sf::RenderTarget& target, bool showHitBox) const override;
    int  get_score_value() const override 
    { return GAMAKICHI_SCORE; }

    int  getMaxHealth() const 
    { return m_max_health; }
    int  getCurrentHealth() const 
    { return m_health; }
    void takeDamage() {
        if (m_health > 0) 
            --m_health;
        if (m_health <= 0) 
            m_defeated = true;
        m_snow_level = snow_level::None;
        m_snow_hits = 0;
    }

private:
    int   m_health = 30;
    int   m_max_health = 30;

    float m_rocket_timer = 0.f;
    float m_spawn_timer = 0.f;
    float m_rocket_interval = 3.5f;
    float m_spawn_interval = 5.0f;
    int   m_rocket_count = 2;
    float m_drift_vx = 30.f;

    int  currentPhase() const;
    void fireVolley(float playerX, float playerY);
    void drawArms(sf::RenderTarget& target) const;

    static sf::Texture s_tex;
    static bool s_loaded;
};