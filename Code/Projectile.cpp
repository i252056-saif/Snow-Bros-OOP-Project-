#include "Projectile.h"
#include "Platform.h"
#include "Constants.h"
#include <cmath>

// Base
projectile::projectile(float x, float y, float vx, float vy,
                       projectile_type type, float radius)
    : m_type(type), m_vx(vx), m_vy(vy), m_radius(radius),
      m_hitbox(x - radius, y - radius, radius * 2.f, radius * 2.f)
{
    m_shape.setRadius(radius);
    m_shape.setOrigin(radius, radius);
    m_shape.setPosition(x, y);
    m_shape.setOutlineThickness(1.5f);

    if (type == projectile_type::snowball) {
        m_shape.setFillColor(sf::Color(200, 230, 255));
        m_shape.setOutlineColor(sf::Color(100, 160, 220));
    } else if (type == projectile_type::encased_enemy) {
        m_shape.setFillColor(sf::Color(180, 210, 240));
        m_shape.setOutlineColor(sf::Color(80, 130, 200));
    } else if (type == projectile_type::rocket) {
        m_shape.setFillColor(sf::Color(220, 100, 30));
        m_shape.setOutlineColor(sf::Color(255, 60, 0));
    } else if (type == projectile_type::explosion) {
        m_shape.setFillColor(sf::Color(255, 180, 0, 200));
        m_shape.setOutlineColor(sf::Color(255, 80, 0, 220));
    } else {
        // knife
        m_shape.setFillColor(sf::Color(220, 180, 50));
        m_shape.setOutlineColor(sf::Color(180, 130, 20));
    }
}

void projectile::draw(sf::RenderTarget& target, bool showHitBox) const {
    if (m_expired) return;
    target.draw(m_shape);
    if (showHitBox)
        m_hitbox.debugDraw(target, sf::Color::Yellow);
}

//snowball
snowball::snowball(float x, float y, float dirX)
    : projectile(x, y, dirX * SNOWBALL_SPEED, 0.f, projectile_type::snowball, 8.f) {}

void snowball::update(float dt, platform*, int) {
    float nx = m_shape.getPosition().x + m_vx * dt;
    float ny = m_shape.getPosition().y;
    // Screen wrap
    if (nx < -m_radius)                  nx = WINDOW_WIDTH  + m_radius;
    if (nx > WINDOW_WIDTH  + m_radius)   nx = -m_radius;
    m_shape.setPosition(nx, ny);
    m_hitbox.setPosition(nx - m_radius, ny - m_radius);
}

//rolling_snowball
rolling_snowball::rolling_snowball(float x, float y, float dirX)
    : projectile(x, y, dirX * ROLL_SPEED, 0.f, projectile_type::encased_enemy, 14.f) {}

void rolling_snowball::update(float dt, platform* platforms, int platCount) {
    m_vy += GRAVITY * dt;
    float nx = m_shape.getPosition().x + m_vx * dt;
    float ny = m_shape.getPosition().y + m_vy * dt;

    for (int i = 0; i < platCount; ++i) {
        float pl = platforms[i].get_left();
        float pt = platforms[i].get_top();
        float pw = platforms[i].get_width();

        if (nx + m_radius > pl && nx - m_radius < pl + pw) {
            float prevBot = m_shape.getPosition().y + m_radius;
            if (prevBot <= pt + 4.f && ny + m_radius >= pt) {
                ny = pt - m_radius;
                m_vy = 0.f;
            }
        }
    }

    // Wall bounce
    if (nx < m_radius || nx > WINDOW_WIDTH - m_radius) m_vx = -m_vx;
    // Floor
    if (ny + m_radius > WINDOW_HEIGHT - 10.f) {
        ny = WINDOW_HEIGHT - 10.f - m_radius;
        m_vy = 0.f;
    }
    // Grow slightly with chain
    float newR = 14.f + m_chain_count * 2.f;
    if (newR > 26.f) newR = 26.f;
    if (newR != m_radius) {
        m_radius = newR;
        m_shape.setRadius(m_radius);
        m_shape.setOrigin(m_radius, m_radius);
        m_hitbox.setSize(m_radius * 2.f, m_radius * 2.f);
    }

    m_shape.setPosition(nx, ny);
    m_hitbox.setPosition(nx - m_radius, ny - m_radius);
}

// knife
knife::knife(float x, float y, float targetX, float targetY)
    : projectile(x, y, 0, 0, projectile_type::knife, 6.f)
{
    float dx = targetX - x, dy = targetY - y;
    float len = sqrtf(dx * dx + dy * dy);
    if (len > 0.f) { dx /= len; dy /= len; }
    m_vx = dx * 250.f;
    m_vy = dy * 250.f;
}

void knife::update(float dt, platform*, int) {
    float nx = m_shape.getPosition().x + m_vx * dt;
    float ny = m_shape.getPosition().y + m_vy * dt;
    m_shape.setPosition(nx, ny);
    m_hitbox.setPosition(nx - m_radius, ny - m_radius);
    if (nx < -20 || nx > WINDOW_WIDTH + 20 || ny < -20 || ny > WINDOW_HEIGHT + 20)
        m_expired = true;
}

// rocket

rocket::rocket(float x, float y, float vx, float vy)
    : projectile(x, y, vx, vy, projectile_type::rocket, 9.f) {}

void rocket::explode(float x, float y) {
    pending_explosion = new explosion(x, y);
    m_expired = true;
}

void rocket::update(float dt, platform* platforms, int platCount) {
    if (m_expired) return;

    m_life -= dt;
    if (m_life <= 0.f) {
        explode(m_shape.getPosition().x, m_shape.getPosition().y);
        return;
    }

    // Gravity pulls rockets down
    m_vy += GRAVITY * 0.55f * dt;

    float nx = m_shape.getPosition().x + m_vx * dt;
    float ny = m_shape.getPosition().y + m_vy * dt;

    // explode on platform collision
    for (int i = 0; i < platCount; ++i) {
        float pl = platforms[i].get_left();
        float pt = platforms[i].get_top();
        float pw = platforms[i].get_width();
        float ph = platforms[i].get_height();

        float left  = nx - m_radius;
        float right = nx + m_radius;
        float bot   = ny + m_radius;
        float prevBot = m_shape.getPosition().y + m_radius;

        if (right > pl && left < pl + pw) {
            if (prevBot <= pt + 4.f && bot >= pt) {
                explode(nx, pt);
                return;
            }
        }
        // Side walls of platform
        float top = ny - m_radius;
        if (bot > pt + 4.f && top < pt + ph) {
            if ((left < pl + pw && left - m_vx * dt >= pl + pw - 2.f) ||
                (right > pl    && right - m_vx * dt <= pl + 2.f)) {
                explode(nx, ny);
                return;
            }
        }
    }

    // explode on floor
    if (ny + m_radius > WINDOW_HEIGHT - 10.f) {
        explode(nx, WINDOW_HEIGHT - 10.f);
        return;
    }

    // Expire if it flies screen horizontally
    if (nx < -40.f || nx > WINDOW_WIDTH + 40.f) {
        m_expired = true;
        return;
    }

    m_shape.setPosition(nx, ny);
    m_hitbox.setPosition(nx - m_radius, ny - m_radius);
}

void rocket::draw(sf::RenderTarget& target, bool showHitBox) const {
    if (m_expired) return;
    // Draw rocket body
    target.draw(m_shape);

    // Draw a small  trail
    auto pos = m_shape.getPosition();
    float speed = sqrtf(m_vx * m_vx + m_vy * m_vy);
    if (speed > 0.f) {
        float tx = -m_vx / speed * 14.f;
        float ty = -m_vy / speed * 14.f;
        sf::Vertex trail[2] = {
            {sf::Vector2f(pos.x,        pos.y       ), sf::Color(255, 160, 0, 200)},
            {sf::Vector2f(pos.x + tx,   pos.y + ty  ), sf::Color(255, 80,  0, 0  )}
        };
        target.draw(trail, 2, sf::Lines);
    }

    if (showHitBox) m_hitbox.debugDraw(target, sf::Color::Yellow);
}

// explosion

explosion::explosion(float x, float y)
    : projectile(x, y, 0.f, 0.f, projectile_type::explosion, 40.f)
{
    m_shape.setFillColor(sf::Color(255, 180, 0, 200));
    m_shape.setOutlineColor(sf::Color(255, 60, 0, 220));
    m_shape.setOutlineThickness(3.f);
}

void explosion::update(float dt, platform*, int) {
    m_elapsed     += dt;
    m_damageTimer -= dt;

    if (m_elapsed >= m_totalLife) {
        m_expired = true;
        return;
    }

    // Expand and fade 
    float progress = m_elapsed / m_totalLife;          // 0 → 1
    float newR     = 40.f + progress * 30.f;           // 40 → 70
    sf::Uint8 alpha = static_cast<sf::Uint8>((1.f - progress) * 200.f);

    m_radius = newR;
    m_shape.setRadius(newR);
    m_shape.setOrigin(newR, newR);
    m_hitbox.setSize(newR * 2.f, newR * 2.f);

    auto pos = m_shape.getPosition();
    m_hitbox.setPosition(pos.x - newR, pos.y - newR);

    m_shape.setFillColor(sf::Color(255, 140 + (sf::Uint8)(progress * 60), 0, alpha));
    m_shape.setOutlineColor(sf::Color(255, 60, 0, (sf::Uint8)(alpha * 0.8f)));
}

void explosion::draw(sf::RenderTarget& target, bool showHitBox) const {
    if (m_expired) return;
    target.draw(m_shape);

    // Inner bright core
    auto pos = m_shape.getPosition();
    sf::CircleShape core(m_radius * 0.45f);
    core.setOrigin(m_radius * 0.45f, m_radius * 0.45f);
    core.setPosition(pos);
    auto fc = m_shape.getFillColor();
    core.setFillColor(sf::Color(255, 255, 200, (sf::Uint8)(fc.a * 1.4f > 255 ? 255 : fc.a * 1.4f)));
    target.draw(core);

    if (showHitBox) m_hitbox.debugDraw(target, sf::Color(255, 100, 0));
}
