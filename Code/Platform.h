#pragma once
#include <SFML/Graphics.hpp>
#include "HitBox.h"

/*
 class Platform
  Static rectangular platform.
 */
class platform {
public:
    platform();
    platform(float x, float y, float w, float h,
             sf::Color colour = sf::Color(80, 60, 40));

    void draw(sf::RenderTarget& target, bool showHitBox) const;

    const hitbox& get_hitbox() const 
    { return m_hitbox; }
    float get_left() const 
    { return m_hitbox.get_left(); }
    float get_top() const 
    { return m_hitbox.get_top(); }
    float get_width()  const 
    { return m_hitbox.get_width(); }
    float get_height() const 
    { return m_hitbox.get_height(); }

private:
    sf::RectangleShape m_shape;
    hitbox m_hitbox;
};
