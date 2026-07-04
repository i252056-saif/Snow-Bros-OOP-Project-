#include "PowerUp.h"
#include <cmath>

power_up::power_up(power_up_type type, float x, float y, sf::Color colour)
    : m_type(type), m_hitbox(x - 10.f, y - 10.f, 20.f, 20.f), m_baseY(y)
{
    m_shape.setRadius(10.f);
    m_shape.setOrigin(10.f, 10.f);
    m_shape.setPosition(x, y);
    m_shape.setFillColor(colour);
    m_shape.setOutlineColor(sf::Color::White);
    m_shape.setOutlineThickness(2.f);
}

void power_up::update(float dt) {
    m_bobTimer += dt * 2.f;
    float newY = m_baseY + sinf(m_bobTimer) * 4.f;
    m_shape.setPosition(m_shape.getPosition().x, newY);
    m_hitbox.setPosition(m_shape.getPosition().x - 10.f, newY - 10.f);
}

void power_up::draw(sf::RenderTarget& target, bool showHitBox) const {
    if (m_collected) return;
    target.draw(m_shape);
    if (showHitBox)
        m_hitbox.debugDraw(target, sf::Color::Yellow);
}

speed_boost_powerup::speed_boost_powerup(float x, float y)
    : power_up(power_up_type::speed_boost, x, y, sf::Color(255, 200, 0)) {}

snowball_powerup::snowball_powerup(float x, float y)
    : power_up(power_up_type::snowball_power, x, y, sf::Color(150, 220, 255)) {}

distance_powerup::distance_powerup(float x, float y)
    : power_up(power_up_type::distance_increase, x, y, sf::Color(100, 255, 150)) {}

balloon_powerup::balloon_powerup(float x, float y)
    : power_up(power_up_type::balloon_mode, x, y, sf::Color(255, 100, 200)) {}

extra_life_powerup::extra_life_powerup(float x, float y)
    : power_up(power_up_type::extra_life, x, y, sf::Color(255, 50, 50)) {}
