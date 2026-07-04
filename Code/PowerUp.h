#pragma once
#include <SFML/Graphics.hpp>
#include "HitBox.h"

enum class power_up_type {
    speed_boost,
    snowball_power,
    distance_increase,
    balloon_mode,
    extra_life
};

/*
 class power_up
  Abstract base for all collectible power ups.
  Concrete subclasses set their own colour in the constructor.
 */
class power_up {
public:
    power_up(power_up_type type, float x, float y, sf::Color colour);
    virtual ~power_up() = default;

    virtual void update(float dt);
    virtual void draw(sf::RenderTarget& target, bool show_hitbox) const;

    power_up_type   get_type()      const { return m_type; }
    const hitbox& get_hitbox()    const { return m_hitbox; }
    bool          isCollected()  const { return m_collected; }
    void          collect()            { m_collected = true; }

protected:
    power_up_type   m_type;
    sf::CircleShape m_shape;
    hitbox          m_hitbox;
    bool            m_collected = false;
    float           m_bobTimer  = 0.f;
    float           m_baseY;
};

class speed_boost_powerup    : public power_up {
public: speed_boost_powerup(float x, float y);
};
class snowball_powerup      : public power_up {
public: snowball_powerup(float x, float y);
};
class distance_powerup      : public power_up {
public: distance_powerup(float x, float y);
};
class balloon_powerup       : public power_up {
public: balloon_powerup(float x, float y);
};
class extra_life_powerup     : public power_up {
public: extra_life_powerup(float x, float y);
};
