#pragma once
#include <SFML/Graphics.hpp>

/*
class hitbox
  Rectangular collision box for any game entity.
  Separated from the visual sprite for clean OOP design.
 */
class hitbox {
public:
    hitbox();
    hitbox(float x, float y, float w, float h);

    void  setPosition(float x, float y);
    void setSize(float w, float h);
    sf::FloatRect getRect() const;
    bool intersects(const hitbox& other) const;
    void debugDraw(sf::RenderTarget& target, sf::Color colour) const;

    float get_left() const 
    { return m_x; }
    float get_top()const 
    { return m_y; }
    float get_width()  const 
    { return m_w; }
    float get_height() const 
    { return m_h; }

private:
    float m_x = 0, m_y = 0, m_w = 0, m_h = 0;
};
