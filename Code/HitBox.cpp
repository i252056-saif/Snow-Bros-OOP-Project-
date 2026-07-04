#include "HitBox.h"

hitbox::hitbox() : m_x(0), m_y(0), m_w(0), m_h(0) {}
hitbox::hitbox(float x, float y, float w, float h) : m_x(x), m_y(y), m_w(w), m_h(h) {}

void hitbox::setPosition(float x, float y) { m_x = x; m_y = y; }
void hitbox::setSize(float w, float h) { m_w = w; m_h = h; }

sf::FloatRect hitbox::getRect() const {
    return sf::FloatRect(m_x, m_y, m_w, m_h);
}

bool hitbox::intersects(const hitbox& o) const {
    return !(m_x + m_w <= o.m_x || o.m_x + o.m_w <= m_x ||
             m_y + m_h <= o.m_y || o.m_y + o.m_h <= m_y);
}

void hitbox::debugDraw(sf::RenderTarget& target, sf::Color colour) const {
    sf::RectangleShape r;
    r.setPosition(m_x, m_y);
    r.setSize({m_w, m_h});
    r.setFillColor(sf::Color::Red);
    r.setOutlineColor(colour);
    r.setOutlineThickness(1.5f);
    target.draw(r);
}
