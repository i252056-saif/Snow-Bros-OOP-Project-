#include "Player.h"
#include "Platform.h"
#include "Projectile.h"
#include "Constants.h"

Player::Player()
    : m_playerIndex(0),
    m_hitbox(0, 0, PLAYER_WIDTH, PLAYER_HEIGHT),
    m_color(sf::Color(80, 160, 220))
{
    m_shape.setSize({ PLAYER_WIDTH, PLAYER_HEIGHT });
    m_shape.setOrigin(PLAYER_WIDTH / 2.f, PLAYER_HEIGHT / 2.f);
    m_shape.setFillColor(m_color);
    m_shape.setOutlineColor(sf::Color::White);
    m_shape.setOutlineThickness(2.f);
    setupControls();
}

Player::Player(float x, float y, int playerIndex)
    : m_playerIndex(playerIndex),
    m_hitbox(x - PLAYER_WIDTH / 2.f, y - PLAYER_HEIGHT / 2.f,
        PLAYER_WIDTH, PLAYER_HEIGHT),
    m_color(playerIndex == 0 ? sf::Color(80, 160, 220)
        : sf::Color(220, 140, 60))
{
    m_shape.setSize({ PLAYER_WIDTH, PLAYER_HEIGHT });
    m_shape.setOrigin(PLAYER_WIDTH / 2.f, PLAYER_HEIGHT / 2.f);
    m_shape.setPosition(x, y);
    m_shape.setFillColor(m_color);
    m_shape.setOutlineColor(sf::Color::White);
    m_shape.setOutlineThickness(2.f);
    setupControls();
}

// loadSprite
void Player::loadSprite(const char* path) {
    if (!m_texture.loadFromFile(path)) return;  
    m_texture.setSmooth(true);

    auto sz = m_texture.getSize();
    m_sprite.setTexture(m_texture);              // sprite points to this object's m_texture
    m_sprite.setOrigin(sz.x / 2.f, sz.y / 2.f);
    m_sprite.setScale(PLAYER_WIDTH / (float)sz.x,
        PLAYER_HEIGHT / (float)sz.y);
    m_sprite.setPosition(m_shape.getPosition());
    m_use_sprite = true;
}

// setupControls
void Player::setupControls() {
    if (m_playerIndex == 0)
        m_controls = { sf::Keyboard::A, sf::Keyboard::D,
                       sf::Keyboard::W, sf::Keyboard::Space };
    else
        m_controls = { sf::Keyboard::Left, sf::Keyboard::Right,
                       sf::Keyboard::Up,sf::Keyboard::L };
}

//  handleInput
void Player::handleInput(float dt) {
    (void)dt;
    m_vx = 0.f;
    float speed = PLAYER_SPEED * (m_speedBoostTimer > 0.f ? SPEED_BOOST_MULT : 1.f);

    if (sf::Keyboard::isKeyPressed(m_controls.left)) 
    { 
        m_vx = -speed; 
        m_facingRight = false;
    }
    if (sf::Keyboard::isKeyPressed(m_controls.right)) 
    { 
        m_vx = speed;
        m_facingRight = true; 
    }

    if (m_balloonTimer > 0.f) {
        if (sf::Keyboard::isKeyPressed(m_controls.jump)) 
            m_vy = BALLOON_FLOAT_VEL;
    }
    else {
        if (sf::Keyboard::isKeyPressed(m_controls.jump) && m_on_ground) {
            m_vy = JUMP_VELOCITY;
            m_on_ground = false;
        }
    }
}

//  update
void Player::update(float dt, platform* platforms, int platCount) {
    if (m_throwCooldown > 0.f) m_throwCooldown -= dt;
    if (m_invincibleTimer > 0.f) m_invincibleTimer -= dt;
    if (m_speedBoostTimer > 0.f) m_speedBoostTimer -= dt;
    if (m_balloonTimer > 0.f) m_balloonTimer -= dt;

    apply_gravity_and_collide(dt, platforms, platCount);

    // Flip sprite based on facing direction
    if (m_use_sprite) {
        auto sz = m_texture.getSize();
        float scaleX = (PLAYER_WIDTH / (float)sz.x) * (m_facingRight ? 1.f : -1.f);
        m_sprite.setScale(scaleX, PLAYER_HEIGHT / (float)sz.y);
    }

    // Blink alpha when invincible
    sf::Uint8 alpha = (m_invincibleTimer > 0.f)
        ? (static_cast<int>(m_invincibleTimer * 8) % 2 == 0 ? 255 : 80)
        : 255;
    m_shape.setFillColor({ m_color.r, m_color.g, m_color.b, alpha });
    if (m_use_sprite)
        m_sprite.setColor(sf::Color(255, 255, 255, alpha));
}

//  apply_gravity_and_collide
void Player::apply_gravity_and_collide(float dt, platform* platforms, int platCount) {
    if (m_balloonTimer <= 0.f) m_vy += GRAVITY * dt;

    float nx = m_shape.getPosition().x + m_vx * dt;
    float ny = m_shape.getPosition().y + m_vy * dt;

    m_on_ground = false;
    for (int i = 0; i < platCount; ++i) {
        float pl = platforms[i].get_left();
        float pt = platforms[i].get_top();
        float pw = platforms[i].get_width();

        float left = nx - PLAYER_WIDTH / 2.f;
        float right = nx + PLAYER_WIDTH / 2.f;
        float bot = ny + PLAYER_HEIGHT / 2.f;
        float prevBot = m_shape.getPosition().y + PLAYER_HEIGHT / 2.f;

        if (right > pl && left < pl + pw) {
            if (prevBot <= pt + 6.f && bot >= pt) {
                ny = pt - PLAYER_HEIGHT / 2.f;
                m_vy = 0.f;
                m_on_ground = true;
            }
        }
    }

    // Horizontal screen wrap
    if (nx < -PLAYER_WIDTH / 2.f)               nx = WINDOW_WIDTH + PLAYER_WIDTH / 2.f;
    if (nx > WINDOW_WIDTH + PLAYER_WIDTH / 2.f) nx = -PLAYER_WIDTH / 2.f;

    // Floor
    if (ny + PLAYER_HEIGHT / 2.f > WINDOW_HEIGHT - 10.f) {
        ny = WINDOW_HEIGHT - 10.f - PLAYER_HEIGHT / 2.f;
        m_vy = 0.f;
        m_on_ground = true;
    }

    m_shape.setPosition(nx, ny);
    m_sprite.setPosition(nx, ny); 
    m_hitbox.setPosition(nx - PLAYER_WIDTH / 2.f, ny - PLAYER_HEIGHT / 2.f);
}

//  tryThrow
projectile* Player::tryThrow() {
    if (m_throwCooldown > 0.f) return nullptr;
    m_throwCooldown = 0.35f;
    auto  pos = m_shape.getPosition();
    float dirX = m_facingRight ? 1.f : -1.f;
    return new snowball(pos.x + dirX * (PLAYER_WIDTH / 2.f + 4.f), pos.y, dirX);
}

// takeDamage
void Player::takeDamage() {
    if (m_invincibleTimer > 0.f) return;
    --m_lives;
    m_invincibleTimer = INVINCIBLE_DURATION;
}

//  applyPowerUp
void Player::applyPowerUp(power_up_type type) {
    switch (type) {
    case power_up_type::speed_boost: m_speedBoostTimer = SPEED_BOOST_TIME; break;
    case power_up_type::snowball_power: m_snowballPower = true;  break;
    case power_up_type::distance_increase:  m_distanceIncrease = true;  break;
    case power_up_type::balloon_mode: m_balloonTimer = BALLOON_TIME; m_vy = 0.f; break;
    case power_up_type::extra_life: ++m_lives; break;
    }
}

// draw
void Player::draw(sf::RenderTarget& target, bool showHitBox) const {
    if (m_use_sprite)
        target.draw(m_sprite);
    else
        target.draw(m_shape);

    auto pos = m_shape.getPosition();

    // Eye dot always drawn on top of sprite for visual clarity
    sf::CircleShape eye(4.f);
    eye.setFillColor(sf::Color::White);
    eye.setOrigin(4.f, 4.f);
    eye.setPosition(pos.x + (m_facingRight ? 6.f : -6.f), pos.y - 8.f);
    target.draw(eye);

    // P2 badge
    if (m_playerIndex == 1) {
        sf::CircleShape badge(7.f);
        badge.setFillColor(sf::Color(220, 80, 80, 200));
        badge.setOrigin(7.f, 7.f);
        badge.setPosition(pos.x, pos.y - PLAYER_HEIGHT / 2.f - 8.f);
        target.draw(badge);
    }

    // Balloon visual
    if (m_balloonTimer > 0.f) {
        sf::CircleShape balloon(12.f);
        balloon.setFillColor(sf::Color(255, 100, 200, 160));
        balloon.setOrigin(12.f, 12.f);
        balloon.setPosition(pos.x, pos.y - PLAYER_HEIGHT / 2.f - 14.f);
        target.draw(balloon);
    }

    if (showHitBox) m_hitbox.debugDraw(target, sf::Color::Green);
}