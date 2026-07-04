#pragma once
#include <SFML/Graphics.hpp>
#include "HitBox.h"
#include "Constants.h"

class platform;

enum class projectile_type { snowball, encased_enemy, knife, rocket, explosion };

/*
 class projectile
  Abstract base for all projectiles.
  Managed by raw pointer array in GameWorld.
 */
class projectile {
public:
    projectile(float x, float y, float vx, float vy,
               projectile_type type, float radius);
    virtual ~projectile() = default;

    virtual void update(float dt, platform* platforms, int platCount) = 0;
    virtual void draw(sf::RenderTarget& target, bool showHitBox) const;

    projectile_type  get_type() const 
    { return m_type; }
    const hitbox&   get_hitbox() const 
    { return m_hitbox; }
    bool is_expired()const 
    { return m_expired; }
    void  expire() 
    { m_expired = true; }
    int  get_chain_count() const 
    { return m_chain_count; }
    void increment_chain()      
    { ++m_chain_count; }

    // Explosion pending spawn: rocket on impact
    projectile* pending_explosion = nullptr;

protected:
    sf::CircleShape m_shape;
    hitbox  m_hitbox;
    projectile_type  m_type;
    float m_vx, m_vy;
    float m_radius;
    bool m_expired = false;
    int m_chain_count = 0;
};

//snowball
class snowball : public projectile {
public:
    snowball(float x, float y, float dirX);
    void update(float dt, platform* platforms, int platCount) override;
};

//rolling_snowball
class rolling_snowball : public projectile {
public:
    rolling_snowball(float x, float y, float dir_x);
    void update(float dt, platform* platforms, int plat_count) override;
};

//knife
class knife : public projectile {
public:
    knife(float x, float y, float targetX, float target_y);
    void update(float dt, platform* platforms, int plat_count) override;
};

//rocket
/*
 class rocket
  Fired by Gamakichi. Travel toward a target
  and explode on platform/floor impact or after a max lifetime.
 */
class rocket : public projectile {
public:
    /*
     param x,y     Spawn position
     param vx,vy   Initial velocity components
     */
    rocket(float x, float y, float vx, float vy);
    void update(float dt, platform* platforms, int platCount) override;
    void draw(sf::RenderTarget& target, bool showHitBox) const override;

private:
    float m_life = 4.f;   // max seconds before auto explode
    void  explode(float x, float y);
};

//explosion
/*
 class explosion
  Short time areaof effect blast or explode.
  Damages the player if they overlap during the active frames.
 */
class explosion : public projectile {
public:
    explosion(float x, float y);
    void update(float dt, platform* platforms, int platCount) override;
    void draw(sf::RenderTarget& target, bool showHitBox) const override;

    /* True while the blast can hurt the player*/
    bool is_dangerous() const { return m_damageTimer > 0.f; }

private:
    float m_totalLife = 0.55f;  // total display time
    float m_damageTimer = 0.25f;  // window where it deals damage
    float m_elapsed = 0.f;
};
