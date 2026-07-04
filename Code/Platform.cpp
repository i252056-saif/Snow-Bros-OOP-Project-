#include "Platform.h"

platform::platform() {}

platform::platform(float x, float y, float w, float h, sf::Color colour)
    : m_hitbox(x, y, w, h)
{
    m_shape.setPosition(x, y);
    m_shape.setSize({w, h});
    m_shape.setFillColor(colour);
    m_shape.setOutlineColor(sf::Color(50, 35, 20));
    m_shape.setOutlineThickness(2.f);
}

void platform::draw(sf::RenderTarget& target, bool showHitBox) const {
    target.draw(m_shape);
    if (showHitBox)
        m_hitbox.debugDraw(target, sf::Color::Blue);
}
