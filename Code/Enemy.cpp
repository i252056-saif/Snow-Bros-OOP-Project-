#include "Enemy.h"
#include "Platform.h"
#include "Projectile.h"
#include "Constants.h"
#include <cmath>
#include <cstdlib>

//static definitions

sf::Texture botom::s_tex_red;
bool botom::s_loaded = false;

sf::Texture flyingfoogafoog::s_tex_red;
sf::Texture flyingfoogafoog::s_tex_blue;
bool flyingfoogafoog::s_loaded = false;

sf::Texture tornado::s_tex_red;
sf::Texture tornado::s_tex_blue;
bool tornado::s_loaded = false;

sf::Texture mogera_child::s_tex;
bool mogera_child::s_loaded = false;

sf::Texture mogera::s_tex;
bool mogera::s_loaded = false;

sf::Texture gamakichi::s_tex;
bool gamakichi::s_loaded = false;

//enemy base

enemy::enemy(float x, float y, float w, float h, enemy_variant variant)
    : m_variant(variant), m_w(w), m_h(h)
{
    m_shape.setSize({ w, h });
    m_shape.setOrigin(w / 2.f, h / 2.f);
    m_shape.setPosition(x, y);
    m_hitbox = hitbox(x - w / 2.f, y - h / 2.f, w, h);
    for (int i = 0; i < 8; ++i) 
    { pending_projectiles[i] = nullptr; 
    pending_enemies[i] = nullptr; }
}

enemy::~enemy() {}

void enemy::setup_sprite(sf::Texture& tex, float w, float h, float x, float y) {
    auto sz = tex.getSize();
    m_sprite.setTexture(tex);
    m_sprite.setOrigin(sz.x / 2.f, sz.y / 2.f);
    m_sprite.setScale(w / (float)sz.x, h / (float)sz.y);
    m_sprite.setPosition(x, y);
    m_use_sprite = true;
}

// tornado tuffaan etc 
void enemy::update_sprite_flip() {
    if (!m_use_sprite) return;
    const sf::Texture* tex = m_sprite.getTexture();
    if (!tex) return;
    auto sz = tex->getSize();
    if (sz.x == 0 || sz.y == 0) return;
    float scaleX = (m_w / (float)sz.x) * (m_vx < 0.f ? -1.f : 1.f);
    m_sprite.setScale(scaleX, m_h / (float)sz.y);
}

bool enemy::apply_gravity_and_collide(float dt, platform* platforms, int platCount) {
    m_vy += GRAVITY * dt;
    float nx = m_shape.getPosition().x + m_vx * dt;
    float ny = m_shape.getPosition().y + m_vy * dt;
    bool onGround = false;

    for (int i = 0; i < platCount; ++i) {
        float pl = platforms[i].get_left();
        float pt = platforms[i].get_top();
        float pw = platforms[i].get_width();
        float ph = platforms[i].get_height();

        float left = nx - m_w / 2.f;
        float right = nx + m_w / 2.f;
        float bot = ny + m_h / 2.f;
        float prevBot = m_shape.getPosition().y + m_h / 2.f;

        if (right > pl && left < pl + pw) {
            if (prevBot <= pt + 4.f && bot >= pt) {
                ny = pt - m_h / 2.f;
                m_vy = 0.f;
                onGround = true;
            }
        }
        // Horizontal wall takkar
        float top = ny - m_h / 2.f;
        if (bot > pt + 4.f && top < pt + ph) {
            if (left < pl + pw && left - m_vx * dt >= pl + pw - 2.f) m_vx = -m_vx;
            if (right > pl && right - m_vx * dt <= pl + 2.f)      m_vx = -m_vx;
        }
    }

    // Screen ke andar
    if (nx - m_w / 2.f < 0.f) 
    { nx = m_w / 2.f;                 
    m_vx = fabsf(m_vx); }
    if (nx + m_w / 2.f > WINDOW_WIDTH) 
    { nx = WINDOW_WIDTH - m_w / 2.f;  
    m_vx = -fabsf(m_vx); }
    if (ny + m_h / 2.f > WINDOW_HEIGHT - 10.f) 
    { ny = WINDOW_HEIGHT - 10.f - m_h / 2.f; 
    m_vy = 0.f; 
    onGround = true; }

    m_shape.setPosition(nx, ny);
    m_sprite.setPosition(nx, ny);
    m_hitbox.setPosition(nx - m_w / 2.f, ny - m_h / 2.f);
    return onGround;
}

void enemy::apply_snow() {
    if (m_snow_level == snow_level::Full) return;
    ++m_snow_hits;
    int needed = hits_to_encase();
    if (m_snow_hits >= needed) {
        m_snow_level = snow_level::Full;
        m_vx = 0.f; m_vy = 0.f;
        m_encase_timer = ENCASE_TIMEOUT;
    }
    else if (m_snow_hits >= (needed / 2 + 1)) {
        m_snow_level = snow_level::Half;
    }
}

int enemy::hits_to_encase() const {
    switch (m_variant) {
    case enemy_variant::Green:  return 3;
    case enemy_variant::Blue:  return 4;
    case enemy_variant::Purple: return 5;
    default: return 2;
    }
}

sf::Color enemy::variantColor(sf::Color base) const {
    switch (m_variant) {
    case enemy_variant::Green:return sf::Color(60, 200, 80);
    case enemy_variant::Blue: return sf::Color(80, 130, 230);
    case enemy_variant::Purple: return sf::Color(180, 60, 220);
    default:  return base;
    }
}

void enemy::draw_snow(sf::RenderTarget& target) const {
    if (m_snow_level == snow_level::None) return;
    auto pos = m_shape.getPosition();
    float r = (m_snow_level == snow_level::Full) ? m_w * 0.6f : m_w * 0.35f;
    sf::CircleShape snow;
    snow.setRadius(r);
    snow.setOrigin(r, r);
    snow.setPosition(pos);
    snow.setFillColor(m_snow_level == snow_level::Full
        ? sf::Color(200, 230, 255, 200)
        : sf::Color(200, 230, 255, 120));
    snow.setOutlineColor(sf::Color(150, 190, 230, 180));
    snow.setOutlineThickness(1.5f);
    target.draw(snow);
}

// botom

botom::botom(float x, float y, enemy_variant variant)
    : enemy(x, y, 28.f, 32.f, variant)
{
    m_shape.setFillColor(variantColor(sf::Color(220, 100, 60)));
    m_vx = BOTOM_SPEED * (rand() % 2 == 0 ? 1.f : -1.f);

    if (!s_loaded) {
        s_loaded = s_tex_red.loadFromFile("assets/images/FlyingFoogaFoog_Red.png");
        if (s_loaded) s_tex_red.setSmooth(true);
    }
    if (s_loaded) {
        setup_sprite(s_tex_red, m_w, m_h, x, y);
        switch (m_variant) {
        case enemy_variant::Green:  m_sprite.setColor(sf::Color(60, 200, 80));  break;
        case enemy_variant::Blue:  m_sprite.setColor(sf::Color(80, 130, 230)); break;
        case enemy_variant::Purple: m_sprite.setColor(sf::Color(180, 60, 220)); break;
        default: break;
        }
    }
}

void botom::update(float dt, platform* platforms, int platCount, float, float) {
    if (m_snow_level == snow_level::Full) {
        m_encase_timer -= dt;  
        if (m_encase_timer <= 0.f) {
            m_snow_level = snow_level::None; m_snow_hits = 0;
            m_vx = BOTOM_SPEED * (rand() % 2 == 0 ? 1.f : -1.f);
        }
        apply_gravity_and_collide(dt, platforms, platCount);
        return;
    }

    float speedMult = (m_variant == enemy_variant::Green) ? 1.25f :
        (m_variant == enemy_variant::Blue) ? 1.50f :
        (m_variant == enemy_variant::Purple) ? 1.75f : 1.f;
    float sign = m_vx >= 0.f ? 1.f : -1.f;
    m_vx = sign * BOTOM_SPEED * speedMult;

    m_dir_timer += dt;
    if (m_dir_timer >= m_dir_timeout) {
        m_dir_timer = 0.f;
        m_dir_timeout = 1.5f + (float)(rand() % 300) / 100.f;
        if (rand() % 3 == 0) m_vx = -m_vx;
    }
    if (m_snow_level == snow_level::Half) m_vx *= 0.5f;

    update_sprite_flip();

    m_on_ground = apply_gravity_and_collide(dt, platforms, platCount);
}

void botom::draw(sf::RenderTarget& target, bool showHitBox) const {
    if (m_use_sprite) target.draw(m_sprite);
    else              target.draw(m_shape);
    draw_snow(target);
    if (showHitBox) m_hitbox.debugDraw(target, sf::Color::Red);
}

int botom::get_score_value() const {
    return BOTOM_SCORE_MIN + rand() % (BOTOM_SCORE_MAX - BOTOM_SCORE_MIN + 1);
}

// flyingfoogafoog

flyingfoogafoog::flyingfoogafoog(float x, float y, enemy_variant variant)
    : botom(x, y, variant)
{
    m_w = 28.f; m_h = 28.f;
    m_shape.setSize({ m_w, m_h });
    m_shape.setOrigin(m_w / 2.f, m_h / 2.f);
    m_shape.setFillColor(variantColor(sf::Color(120, 80, 200)));
    m_hitbox = hitbox(x - 14.f, y - 14.f, 28.f, 28.f);

    if (!s_loaded) {
        bool okR = s_tex_red.loadFromFile("assets/images/FlyingFoogaFoog_Red.png");
        bool okB = s_tex_blue.loadFromFile("assets/images/FlyingFoogaFoog_Blue.png");
        if (okR) s_tex_red.setSmooth(true);
        if (okB) s_tex_blue.setSmooth(true);
        s_loaded = okR;
    }
    if (s_loaded) {
        sf::Texture& tex = (m_variant == enemy_variant::Blue) ? s_tex_blue : s_tex_red;
        setup_sprite(tex, m_w, m_h, x, y);
        switch (m_variant) {
        case enemy_variant::Green:  m_sprite.setColor(sf::Color(100, 220, 120)); break;
        case enemy_variant::Purple: m_sprite.setColor(sf::Color(180, 80, 220)); break;
        default: break;
        }
    }
}

void flyingfoogafoog::update(float dt, platform* platforms, int platCount,
    float playerX, float playerY)
{
    if (m_snow_level == snow_level::Full) {
        m_encase_timer -= dt;  
        if (m_encase_timer <= 0.f) { m_snow_level = snow_level::None; m_snow_hits = 0; m_flying = false; }
        apply_gravity_and_collide(dt, platforms, platCount);
        return;
    }

    if (!m_flying) {
        botom::update(dt, platforms, platCount, playerX, playerY);
        m_ground_timer += dt;
        if (m_ground_timer >= m_ground_duration) {
            m_ground_timer = 0.f; m_flying = true; m_fly_timer = 0.f;
            float angles[8] = { 0,45,90,135,180,225,270,315 };
            float rad = angles[rand() % 8] * 3.14159f / 180.f;
            float spd = FOOGA_SPEED * 1.2f;
            m_fly_vx = cosf(rad) * spd;
            m_fly_vy = sinf(rad) * spd;
        }
    }
    else {
        m_fly_timer += dt;
        float nx = m_shape.getPosition().x + m_fly_vx * dt;
        float ny = m_shape.getPosition().y + m_fly_vy * dt;
        if (nx < 20.f || nx > WINDOW_WIDTH - 20.f) { m_fly_vx = -m_fly_vx; nx = m_shape.getPosition().x; }
        if (ny < 20.f || ny > WINDOW_HEIGHT - 80.f) { m_fly_vy = -m_fly_vy; ny = m_shape.getPosition().y; }
        m_shape.setPosition(nx, ny);
        m_sprite.setPosition(nx, ny);
        m_hitbox.setPosition(nx - 14.f, ny - 14.f);
        if (m_fly_timer >= m_fly_duration) { m_fly_timer = 0.f; m_flying = false; m_vy = 0.f; }

        
        update_sprite_flip();
    }
}

void flyingfoogafoog::draw(sf::RenderTarget& target, bool showHitBox) const {
    if (m_use_sprite) target.draw(m_sprite);
    else              target.draw(m_shape);

    if (m_flying) {
        auto pos = m_shape.getPosition();
        sf::CircleShape w(5.f);
        w.setFillColor(sf::Color(180, 120, 255, 180));
        w.setOrigin(5.f, 5.f);
        w.setPosition(pos.x - 18.f, pos.y); target.draw(w);
        w.setPosition(pos.x + 18.f, pos.y); target.draw(w);
    }

    draw_snow(target);
    if (showHitBox) m_hitbox.debugDraw(target, sf::Color::Red);
}

int flyingfoogafoog::get_score_value() const {
    return FOOGA_SCORE_MIN + rand() % (FOOGA_SCORE_MAX - FOOGA_SCORE_MIN + 1);
}

// tornado

tornado::tornado(float x, float y, enemy_variant variant)
    : flyingfoogafoog(x, y, variant)
{
    m_shape.setFillColor(variantColor(sf::Color(50, 190, 200)));
    m_ground_duration = 2.f; m_fly_duration = 4.f;

    if (!s_loaded) {
        bool okR = s_tex_red.loadFromFile("assets/images/Tornado_Red.png");
        bool okB = s_tex_blue.loadFromFile("assets/images/Tornado_Blue.png");
        if (okR) s_tex_red.setSmooth(true);
        if (okB) s_tex_blue.setSmooth(true);
        s_loaded = okR;
    }
    if (s_loaded) {
        sf::Texture& tex = (m_variant == enemy_variant::Blue) ? s_tex_blue : s_tex_red;
        setup_sprite(tex, m_w, m_h, x, y);
        switch (m_variant) {
        case enemy_variant::Green:  m_sprite.setColor(sf::Color(60, 200, 80));  break;
        case enemy_variant::Purple: m_sprite.setColor(sf::Color(180, 60, 220)); break;
        default: break;
        }
    }
}

void tornado::update(float dt, platform* platforms, int platCount,
    float playerX, float playerY)
{
    flyingfoogafoog::update(dt, platforms, platCount, playerX, playerY);
    if (m_snow_level == snow_level::Full) return;

    m_knife_timer += dt;
    if (m_knife_timer >= m_knife_timeout && pending_projectile_count < 8) {
        m_knife_timer = 0.f;
        auto pos = m_shape.getPosition();
        pending_projectiles[pending_projectile_count++] =
            new knife(pos.x, pos.y, playerX, playerY);
    }
}

void tornado::draw(sf::RenderTarget& target, bool showHitBox) const {
    flyingfoogafoog::draw(target, showHitBox);

    // Spin lines
    auto pos = m_shape.getPosition();
    sf::Vertex lines[4] = {
        {{pos.x - 16.f, pos.y}, sf::Color(50, 220, 230, 180)},
        {{pos.x + 16.f, pos.y}, sf::Color(50, 220, 230, 180)},
        {{pos.x, pos.y - 16.f}, sf::Color(50, 220, 230, 180)},
        {{pos.x, pos.y + 16.f}, sf::Color(50, 220, 230, 180)},
    };
    target.draw(lines, 4, sf::Lines);
}

int tornado::get_score_value() const {
    return TORNADO_SCORE_MIN + rand() % (TORNADO_SCORE_MAX - TORNADO_SCORE_MIN + 1);
}

// mogera_child

mogera_child::mogera_child(float x, float y, float dirX)
    : enemy(x, y, 20.f, 20.f, enemy_variant::Red)
{
    m_shape.setFillColor(sf::Color(200, 120, 50));
    m_vx = dirX * 140.f;

    if (!s_loaded) {
        s_loaded = s_tex.loadFromFile("assets/images/Mogera_child.png");
        if (s_loaded) s_tex.setSmooth(true);
    }
    if (s_loaded) setup_sprite(s_tex, m_w, m_h, x, y);
}

void mogera_child::update(float dt, platform* platforms, int platCount, float, float) {
    if (m_snow_level == snow_level::Full) {
        m_encase_timer -= dt;  
        if (m_encase_timer <= 0.f) { m_snow_level = snow_level::None; m_snow_hits = 0; }
    }
    update_sprite_flip();
    apply_gravity_and_collide(dt, platforms, platCount);
}

void mogera_child::draw(sf::RenderTarget& target, bool showHitBox) const {
    if (m_use_sprite) target.draw(m_sprite);
    else              target.draw(m_shape);
    draw_snow(target);
    if (showHitBox) m_hitbox.debugDraw(target, sf::Color::Red);
}

// mogera

mogera::mogera(float x, float y)
    : enemy(x, y, 80.f, 80.f, enemy_variant::Red)
{
    m_shape.setFillColor(sf::Color(150, 50, 50));
    m_shape.setOutlineColor(sf::Color(200, 80, 80));
    m_shape.setOutlineThickness(3.f);

    if (!s_loaded) {
        s_loaded = s_tex.loadFromFile("assets/images/Mogera.png");
        if (s_loaded) s_tex.setSmooth(true);
    }
    if (s_loaded) setup_sprite(s_tex, m_w, m_h, x, y);
}

void mogera::update(float dt, platform* platforms, int platCount, float playerX, float) {
    if (m_defeated) return;
    m_spawn_timer += dt;
    if (m_spawn_timer >= m_spawn_rate && pending_enemy_count < 8) {
        m_spawn_timer = 0.f;
        auto pos = m_shape.getPosition();
        float dirX = (playerX < pos.x) ? -1.f : 1.f;
        pending_enemies[pending_enemy_count++] =
            new mogera_child(pos.x + dirX * 50.f, pos.y, dirX);
    }
    // Sit on floor/platform
    m_vy += GRAVITY * dt;
    float ny = m_shape.getPosition().y + m_vy * dt;
    for (int i = 0; i < platCount; ++i) {
        float pt = platforms[i].get_top();
        float pl = platforms[i].get_left();
        float pw = platforms[i].get_width();
        float nx = m_shape.getPosition().x;
        if (ny + 40.f >= pt && ny - 40.f < pt + platforms[i].get_height()
            && nx - 40.f < pl + pw && nx + 40.f > pl) {
            ny = pt - 40.f; m_vy = 0.f;
        }
    }
    if (ny + 40.f > WINDOW_HEIGHT - 10.f) { ny = WINDOW_HEIGHT - 50.f; m_vy = 0.f; }
    float nx = m_shape.getPosition().x;
    m_shape.setPosition(nx, ny);
    m_sprite.setPosition(nx, ny);
    m_hitbox.setPosition(nx - 40.f, ny - 40.f);
}

void mogera::draw(sf::RenderTarget& target, bool showHitBox) const {
    if (m_defeated) return;
    if (m_use_sprite) {
        target.draw(m_sprite);
    }
    else {
        target.draw(m_shape);
        auto pos = m_shape.getPosition();
        sf::CircleShape eye(8.f);
        eye.setFillColor(sf::Color::Yellow);
        eye.setOrigin(8.f, 8.f);
        eye.setPosition(pos.x - 18.f, pos.y - 15.f); target.draw(eye);
        eye.setPosition(pos.x + 18.f, pos.y - 15.f); target.draw(eye);
    }
    draw_snow(target);
    if (showHitBox) m_hitbox.debugDraw(target, sf::Color::Red);
}

// gamaakichi_child

gamaakichi_child::gamaakichi_child(float x, float y, float dirX)
    : enemy(x, y, 24.f, 22.f, enemy_variant::Red)
{
    m_shape.setFillColor(sf::Color(60, 160, 80));
    m_shape.setOutlineColor(sf::Color(30, 100, 50));
    m_shape.setOutlineThickness(1.5f);
    m_vx = dirX * 120.f;
}

void gamaakichi_child::update(float dt, platform* platforms, int platCount,
    float playerX, float)
{
    if (m_snow_level == snow_level::Full) {
        m_encase_timer -= dt;  
        if (m_encase_timer <= 0.f) {
            m_snow_level = snow_level::None;
            m_snow_hits = 0;
            m_vx = (playerX < m_shape.getPosition().x ? -1.f : 1.f) * 120.f;
        }
        apply_gravity_and_collide(dt, platforms, platCount);
        return;
    }

    m_hop_timer += dt;
    if (m_hop_timer >= m_hop_interval) {
        m_hop_timer = 0.f;
        m_vy = -260.f;
        m_vx = (playerX < m_shape.getPosition().x ? -1.f : 1.f) * 120.f;
    }
    if (m_snow_level == snow_level::Half) m_vx *= 0.5f;
    apply_gravity_and_collide(dt, platforms, platCount);
}

void gamaakichi_child::draw(sf::RenderTarget& target, bool showHitBox) const {
    target.draw(m_shape);
    auto pos = m_shape.getPosition();
    sf::CircleShape eye(4.f);
    eye.setFillColor(sf::Color(255, 220, 0));
    eye.setOrigin(4.f, 4.f);
    eye.setPosition(pos.x - 7.f, pos.y - 5.f); target.draw(eye);
    eye.setPosition(pos.x + 7.f, pos.y - 5.f); target.draw(eye);
    draw_snow(target);
    if (showHitBox) m_hitbox.debugDraw(target, sf::Color::Red);
}

// gamaakichi

gamakichi::gamakichi(float x, float y)
    : enemy(x, y, 110.f, 100.f, enemy_variant::Red)
{
    m_shape.setFillColor(sf::Color(40, 130, 60));
    m_shape.setOutlineColor(sf::Color(20, 200, 80));
    m_shape.setOutlineThickness(4.f);
    m_vx = m_drift_vx;

    if (!s_loaded) {
        s_loaded = s_tex.loadFromFile("assets/images/Gamakichi.png");
        if (s_loaded) s_tex.setSmooth(true);
    }
    if (s_loaded) setup_sprite(s_tex, m_w, m_h, x, y);
}

int gamakichi::currentPhase() const {
    float frac = (float)m_health / (float)m_max_health;
    if (frac > 0.66f) return 1;
    if (frac > 0.33f) return 2;
    return 3;
}

void gamakichi::fireVolley(float playerX, float playerY) {
    auto  pos = m_shape.getPosition();
    float dx = playerX - pos.x;
    float dy = playerY - pos.y;
    float base = atan2f(dy, dx);
    float step = 3.14159f / 10.f;
    int   half = m_rocket_count / 2;

    for (int i = 0; i < m_rocket_count && pending_projectile_count < 8; ++i) {
        float angle = base + (i - half) * step;
        float spd = 240.f + currentPhase() * 40.f;
        float rvx = cosf(angle) * spd;
        float rvy = sinf(angle) * spd;
        float offX = ((i % 2 == 0) ? -1.f : 1.f) * (m_w / 2.f + 10.f);
        pending_projectiles[pending_projectile_count++] =
            new rocket(pos.x + offX, pos.y, rvx, rvy);
    }
}

void gamakichi::update(float dt, platform* platforms, int platCount,
    float playerX, float playerY)
{
    if (m_defeated) return;

    int phase = currentPhase();
    switch (phase) {
    case 1: m_rocket_interval = 3.5f; m_spawn_interval = 5.0f; m_rocket_count = 2; break;
    case 2: m_rocket_interval = 2.5f; m_spawn_interval = 3.5f; m_rocket_count = 4; break;
    case 3: m_rocket_interval = 1.5f; m_spawn_interval = 2.0f; m_rocket_count = 6; break;
    }

    // Horizontal drift
    float nx = m_shape.getPosition().x + m_vx * dt;
    if (nx - m_w / 2.f < 20.f) { nx = 20.f + m_w / 2.f;               m_vx = fabsf(m_vx); }
    if (nx + m_w / 2.f > WINDOW_WIDTH - 20.f) { nx = WINDOW_WIDTH - 20.f - m_w / 2.f; m_vx = -fabsf(m_vx); }

    update_sprite_flip();

    // Siit on floor
    m_vy += GRAVITY * dt;
    float ny = m_shape.getPosition().y + m_vy * dt;
    if (ny + m_h / 2.f > WINDOW_HEIGHT - 10.f) { ny = WINDOW_HEIGHT - 10.f - m_h / 2.f; m_vy = 0.f; }

    m_shape.setPosition(nx, ny);
    m_sprite.setPosition(nx, ny);
    m_hitbox.setPosition(nx - m_w / 2.f, ny - m_h / 2.f);

    m_rocket_timer += dt;
    if (m_rocket_timer >= m_rocket_interval) {
        m_rocket_timer = 0.f;
        fireVolley(playerX, playerY);
    }

    m_spawn_timer += dt;
    if (m_spawn_timer >= m_spawn_interval && pending_enemy_count < 8) {
        m_spawn_timer = 0.f;
        auto pos = m_shape.getPosition();
        float dirX = (playerX < pos.x) ? -1.f : 1.f;
        pending_enemies[pending_enemy_count++] =
            new gamaakichi_child(pos.x + dirX * (m_w / 2.f + 12.f), pos.y, dirX);
    }

    // Each full encase = one damage hit 
    if (m_snow_level == snow_level::Full) takeDamage();
}

void gamakichi::drawArms(sf::RenderTarget& target) const {
    auto pos = m_shape.getPosition();
    int  phase = currentPhase();
    sf::Color armCol = (phase == 1) ? sf::Color(80, 180, 100) :
        (phase == 2) ? sf::Color(180, 200, 60) :
        sf::Color(220, 80, 50);

    sf::RectangleShape lArm({ 50.f, 14.f });
    lArm.setOrigin(50.f, 7.f);
    lArm.setPosition(pos.x - m_w / 2.f, pos.y + 10.f);
    lArm.setFillColor(armCol); lArm.setOutlineColor(sf::Color::Black); lArm.setOutlineThickness(1.5f);
    target.draw(lArm);

    sf::RectangleShape lCannon({ 22.f, 10.f });
    lCannon.setOrigin(0.f, 5.f);
    lCannon.setPosition(pos.x - m_w / 2.f - 50.f, pos.y + 10.f);
    lCannon.setFillColor(sf::Color(50, 50, 50));
    target.draw(lCannon);

    sf::RectangleShape rArm({ 50.f, 14.f });
    rArm.setOrigin(0.f, 7.f);
    rArm.setPosition(pos.x + m_w / 2.f, pos.y + 10.f);
    rArm.setFillColor(armCol); rArm.setOutlineColor(sf::Color::Black); rArm.setOutlineThickness(1.5f);
    target.draw(rArm);

    sf::RectangleShape rCannon({ 22.f, 10.f });
    rCannon.setOrigin(22.f, 5.f);
    rCannon.setPosition(pos.x + m_w / 2.f + 50.f, pos.y + 10.f);
    rCannon.setFillColor(sf::Color(50, 50, 50));
    target.draw(rCannon);
}

void gamakichi::draw(sf::RenderTarget& target, bool showHitBox) const {
    if (m_defeated) return;

    if (m_use_sprite) {
        target.draw(m_sprite);
    }
    else {
        target.draw(m_shape);
        auto pos = m_shape.getPosition();
        sf::CircleShape belly(32.f);
        belly.setOrigin(32.f, 32.f);
        belly.setPosition(pos.x, pos.y + 10.f);
        belly.setFillColor(sf::Color(180, 230, 180, 180));
        target.draw(belly);

        sf::CircleShape eb(14.f);
        eb.setOrigin(14.f, 14.f);
        eb.setFillColor(sf::Color(255, 240, 50));
        eb.setOutlineColor(sf::Color::Black);
        eb.setOutlineThickness(2.f);
        eb.setPosition(pos.x - 24.f, pos.y - 28.f); target.draw(eb);
        eb.setPosition(pos.x + 24.f, pos.y - 28.f); target.draw(eb);

        sf::CircleShape pupil(6.f);
        pupil.setOrigin(6.f, 6.f);
        pupil.setFillColor(sf::Color(20, 20, 20));
        pupil.setPosition(pos.x - 24.f, pos.y - 28.f); target.draw(pupil);
        pupil.setPosition(pos.x + 24.f, pos.y - 28.f); target.draw(pupil);
    }

    drawArms(target);

    if (currentPhase() == 3) {
        auto pos = m_shape.getPosition();
        sf::RectangleShape glow({ m_w + 16.f, m_h + 16.f });
        glow.setOrigin((m_w + 16.f) / 2.f, (m_h + 16.f) / 2.f);
        glow.setPosition(pos);
        glow.setFillColor(sf::Color::Transparent);
        glow.setOutlineColor(sf::Color(255, 80, 0, 160));
        glow.setOutlineThickness(4.f);
        target.draw(glow);
    }

    draw_snow(target);
    if (showHitBox) m_hitbox.debugDraw(target, sf::Color::Red);
}