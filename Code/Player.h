#pragma once
#include <SFML/Graphics.hpp>
#include "HitBox.h"
#include "PowerUp.h"
#include "Constants.h"

class platform;
class projectile;

/*
 class Player
  Playable character. Encapsulates all state privately.
  Composition: has a hitbox, has-a Controls struct.
  Supports two player through playerIndex.
 */
class Player {
public:
    Player();
    Player(float x, float y, int playerIndex = 0);

    void handleInput(float dt);
    void update(float dt, platform* platforms, int platCount);
    void draw(sf::RenderTarget& target, bool showHitBox) const;

    projectile* tryThrow();

    void takeDamage();
    void applyPowerUp(power_up_type type);

    void setColor(sf::Color c) 
    { m_color = c; m_shape.setFillColor(c); }

    void loadSprite(const char* path);

    // Getters
    const hitbox& get_hitbox() const 
    { return m_hitbox; }
    //  getter:
    sf::Keyboard::Key getThrowKey() const 
    { return m_controls.throwKey; }
    sf::Vector2f  getPosition() const 
    { return m_shape.getPosition(); }
    int getLives() const 
    { return m_lives; }
    int getGems() const 
    { return m_gems; }
    int getScore()const 
    { return m_score; }
    bool is_alive() const 
    { return m_lives > 0; }
    bool isInvincible()  const 
    { return m_invincibleTimer > 0.f; }
    bool isBalloonMode() const 
    { return m_balloonTimer > 0.f; }
    int getIndex() const 
    { return m_playerIndex; }

    // Setter for save / load
    void setLives(int l) 
    { m_lives = l; }
    void setGems(int g) 
    { m_gems = g; }
    void setScore(int s) 
    { m_score = s; }

    void addScore(int pts) 
    { m_score += pts; }
    void addGems(int g) 
    { m_gems += g; }
    void addLife() 
    { ++m_lives; }

private:
    sf::RectangleShape m_shape;
    hitbox  m_hitbox;
    sf::Color m_color;

    
    sf::Texture  m_texture;
    sf::Sprite m_sprite;
    bool m_use_sprite = false;

    int m_playerIndex;
    float m_vx = 0.f, m_vy = 0.f;
    bool  m_on_ground = false;
    bool  m_facingRight = true;

    int m_lives = PLAYER_START_LIVES;
    int m_gems = 0;
    int m_score = 0;

    float m_throwCooldown = 0.f;
    float m_invincibleTimer = 0.f;
    float m_speedBoostTimer = 0.f;
    float m_balloonTimer = 0.f;
    bool m_snowballPower = false;
    bool m_distanceIncrease = false;

    struct Controls {
        sf::Keyboard::Key left, right, jump, throwKey;
    } m_controls;

    void setupControls();
    void apply_gravity_and_collide(float dt, platform* platforms, int platCount);
};