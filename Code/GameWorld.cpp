#include "GameWorld.h"
#include "Constants.h"
#include "StrUtil.h"
#include <cstdlib>
#include <cmath>


GameWorld::GameWorld(sf::Font& font,
const char* username,
const char* username2,
database& db,
bool multiplayer,
int charSel1,
int charSel2)
: m_font(font), m_db(db), m_hud(font), m_multiplayer(multiplayer)
{
    strCopy(m_username,  username);
    strCopy(m_username2, username2 ? username2 : "");

    for (int i = 0; i < MAX_ENEMIES;++i) 
        m_enemies[i] = nullptr;
    for (int i = 0; i < MAX_PROJECTILES;++i) 
        m_projectiles[i] = nullptr;
    for (int i = 0; i < MAX_POWERUPS;++i) 
        m_powerups[i] = nullptr;

    // loading background 
    if (m_bg_texture.loadFromFile("assets/images/Background.png")) {
        m_bg_texture.setRepeated(false);
        m_bg_sprite.setTexture(m_bg_texture);
        float scaleX = (float)WINDOW_WIDTH  / (float)m_bg_texture.getSize().x;
        float scaleY = (float)WINDOW_HEIGHT / (float)m_bg_texture.getSize().y;
        m_bg_sprite.setScale(scaleX, scaleY);
    }
    m_bg_loaded = m_bg_texture.getSize().x > 0;

    // Character colours: 0=Nick(red/white)  1=Tom(blue)  2=Nova(orange)
    sf::Color charColors[3] = {
        sf::Color(220, 60,  60),   // Nick  red overalls feel
        sf::Color(60,  120, 220),  // Tom   blue overalls
        sf::Color(220, 130, 40)    // Nova  warm orange
    };

    const char* spritePaths[3] = {
    "assets/images/Nick.png",
    "assets/images/Tom.png",
    "assets/images/Nova.png"
    };

    m_players[0].setColor(charColors[charSel1 % 3]);
    m_players[0].loadSprite(spritePaths[charSel1 % 3]); 
    m_player_count = 1;

    if (multiplayer) {
        m_players[1] = Player(500.f, 500.f, 1);
        m_players[1].setColor(charColors[charSel2 % 3]);
        m_players[1].loadSprite(spritePaths[charSel2 % 3]); 
        m_player_count = 2;
    }
}

GameWorld::~GameWorld() { clear_all(); }

void GameWorld::clear_all() {
    for (int i = 0; i < m_enemy_count;++i) 
    { delete m_enemies[i];     
    m_enemies[i] = nullptr; }
    for (int i = 0; i < m_proj_count;++i) 
    { delete m_projectiles[i]; 
    m_projectiles[i] = nullptr; }
    for (int i = 0; i < m_pu_count;++i) 
    { delete m_powerups[i];    
    m_powerups[i] = nullptr; }
    m_enemy_count = m_proj_count = m_pu_count = m_plat_count = 0;
    m_boss = nullptr;
    m_gamakichi_boss = nullptr;
}

// Factory

enemy* GameWorld::make_enemy(const enemy_spawn& spawn) {
    switch (spawn.type) {
        case enemy_spawn::Type::flyingfoogafoog:
            return new flyingfoogafoog(spawn.x, spawn.y, spawn.variant);
        case enemy_spawn::Type::tornado:
            return new tornado(spawn.x, spawn.y, spawn.variant);
        case enemy_spawn::Type::mogera:
            return new mogera(spawn.x, spawn.y);
        case enemy_spawn::Type::gamakichi:
            return new gamakichi(spawn.x, spawn.y);
        default:
            return new botom(spawn.x, spawn.y, spawn.variant);
    }
}

// Array helpers

void GameWorld::add_projectile(projectile* p) {
    if (m_proj_count < MAX_PROJECTILES) 
        m_projectiles[m_proj_count++] = p;
    else delete p;
}
void GameWorld::add_enemy(enemy* e) {
    if (m_enemy_count < MAX_ENEMIES) 
        m_enemies[m_enemy_count++] = e;
    else delete e;
}
void GameWorld::add_power_up(power_up* p) {
    if (m_pu_count < MAX_POWERUPS) 
        m_powerups[m_pu_count++] = p;
    else delete p;
}

// load_level

void GameWorld::load_level(int level_number) {
    clear_all();
    m_current_level  = level_number;
    m_level_complete = false;
    m_game_over = false;
    m_paused = false;

    const level_config& cfg = level_registry::get(level_number);
    m_bg_color = cfg.bg_color;
    m_is_boss_level  = cfg.is_boss_level;
    m_is_bonus_level = (!m_is_boss_level) && (rand() % 4 == 0);

    m_is_bonus_level = cfg.is_collectable_level; // bonus lvl

    // spawn collectables for bonus lvl
    if (cfg.is_collectable_level) {
        // spread bonus
        float xs[] = { 100.f, 200.f, 350.f, 500.f, 650.f, 150.f, 420.f, 580.f };
        float ys[] = { 530.f, 390.f, 270.f, 390.f, 390.f, 170.f, 200.f, 270.f };
        for (int i = 0; i < 8; ++i) {
            int r = i % 5;
            power_up* pu = nullptr;
            switch (r) {
            case 0: pu = new speed_boost_powerup(xs[i], ys[i]); break;
            case 1: pu = new snowball_powerup(xs[i], ys[i]);      break;
            case 2: pu = new distance_powerup(xs[i], ys[i]);      break;
            case 3: pu = new balloon_powerup(xs[i], ys[i]);        break;
            case 4: pu = new extra_life_powerup(xs[i], ys[i]);    break;
            }
            if (pu) add_power_up(pu);
        }
    }

    for (int i = 0; i < cfg.platformCount; ++i) {
        const platform_def& pd = cfg.platforms[i];
        m_platforms[m_plat_count++] = platform(pd.x, pd.y, pd.w, pd.h);
    }

    for (int i = 0; i < cfg.spawn_count; ++i) {
        enemy* e = make_enemy(cfg.spawns[i]);
        if (cfg.spawns[i].type == enemy_spawn::Type::mogera)
            m_boss = static_cast<mogera*>(e);
        else if (cfg.spawns[i].type == enemy_spawn::Type::gamakichi)
            m_gamakichi_boss = static_cast<gamakichi*>(e);
        add_enemy(e);
    }
}

// handle_event

void GameWorld::handle_event(const sf::Event& event) {
    if (event.type != sf::Event::KeyPressed) return;
    auto key = event.key.code;

    if (key == sf::Keyboard::F1 || key == sf::Keyboard::H)
        m_show_hit_boxes = !m_show_hit_boxes;

    if (key == sf::Keyboard::Escape || key == sf::Keyboard::P)
        m_paused = !m_paused;

    if (!m_paused) {
        // P1 throw
        if (key == sf::Keyboard::Space || key == sf::Keyboard::J) {
            projectile* p = m_players[0].tryThrow();
            if (p) add_projectile(p);
        }
        // P2 throw (L or RControl)
        if (m_multiplayer) {
            if (key == sf::Keyboard::L || key == sf::Keyboard::RControl) {
                projectile* p = m_players[1].tryThrow();
                if (p) add_projectile(p);
            }
        }
    }

    // Shop pause : P1 uses S         P2 uses k
    if (m_paused) {
        if (key == sf::Keyboard::S) {
            m_shop_requester = 0; m_open_shop = true;
        }
        if (m_multiplayer && key == sf::Keyboard::K) {
            m_shop_requester = 1; m_open_shop = true;
        }
    }
}

// update

void GameWorld::update(float dt) {
    if (m_paused || m_level_complete || m_game_over) return;

    // Players
    for (int i = 0; i < m_player_count; ++i) {
        m_players[i].handleInput(dt);
        m_players[i].update(dt, m_platforms, m_plat_count);
    }

    // Use P1 position as primary target. P2 target picked inside enemy update
    int currentCount = m_enemy_count;
    for (int i = 0; i < currentCount; ++i) {
        if (!m_enemies[i] || !m_enemies[i]->is_alive()) continue;

        // Pick nearest player for kill lives
        float px = m_players[0].getPosition().x;
        float py = m_players[0].getPosition().y;
        if (m_multiplayer) {
            float d1 = (m_players[0].getPosition().x - m_enemies[i]->getPosition().x);
            float d2 = (m_players[1].getPosition().x - m_enemies[i]->getPosition().x);
            if (d1 < 0.f) d1 = -d1;
            if (d2 < 0.f) d2 = -d2;
            if (d2 < d1) {
                px = m_players[1].getPosition().x;
                py = m_players[1].getPosition().y;
            }
        }
        m_enemies[i]->update(dt, m_platforms, m_plat_count, px, py);
    }

    harvest_pending_spawns();

    for (int i = 0; i < m_proj_count; ++i)
        if (m_projectiles[i] && !m_projectiles[i]->is_expired())
            m_projectiles[i]->update(dt, m_platforms, m_plat_count);

    harvest_rocket_explosions();

    for (int i = 0; i < m_pu_count; ++i)
        if (m_powerups[i] && !m_powerups[i]->isCollected())
            m_powerups[i]->update(dt);

    resolve_snowball_vs_enemies();
    resolve_rolling_vs_enemies();
    resolve_enemy_vs_player();
    resolve_power_up_vs_player();
    resolve_knife_vs_player();
    resolve_explosion_vs_player();

    cleanup_defeated();
    check_level_complete();

    // Bonus lvl
    static float bonusTimer = 0.f;
    if (m_level_complete && m_is_bonus_level) {
        bonusTimer += dt;
        if ((int)(bonusTimer * 4) % 2 == 0) {
            for (int i = 0; i < m_player_count; ++i) {
                m_players[i].addScore(1000);
                m_players[i].addGems(10);
            }
        }
    } else {
        bonusTimer = 0.f;
    }

    // HUD update
    const Player* p2ptr = (m_multiplayer && m_player_count > 1) ? &m_players[1] : nullptr;
    m_hud.update(m_players[0], p2ptr, m_current_level, level_registry::count());

    // Game over players must dead
    bool anyAlive = false;
    for (int i = 0; i < m_player_count; ++i)
        if (m_players[i].is_alive()) { anyAlive = true; break; }
    if (!anyAlive) m_game_over = true;
}

//  Harvest spawns / explosions

void GameWorld::harvest_pending_spawns() {
    for (int i = 0; i < m_enemy_count; ++i) {
        if (!m_enemies[i]) continue;
        for (int j = 0; j < m_enemies[i]->pending_projectile_count; ++j)
            add_projectile(m_enemies[i]->pending_projectiles[j]);
        m_enemies[i]->pending_projectile_count = 0;
        for (int j = 0; j < m_enemies[i]->pending_enemy_count; ++j)
            add_enemy(m_enemies[i]->pending_enemies[j]);
        m_enemies[i]->pending_enemy_count = 0;
    }
}

void GameWorld::harvest_rocket_explosions() {
    for (int i = 0; i < m_proj_count; ++i) {
        projectile* p = m_projectiles[i];
        if (!p) continue;
        if (p->pending_explosion) {
            add_projectile(p->pending_explosion);
            p->pending_explosion = nullptr;
        }
    }
}

// Collision: snowball vs enemies

void GameWorld::resolve_snowball_vs_enemies() {
    for (int pi = 0; pi < m_proj_count; ++pi) {
        projectile* proj = m_projectiles[pi];
        if (!proj || proj->is_expired()) continue;
        if (proj->get_type() != projectile_type::snowball) continue;

        for (int ei = 0; ei < m_enemy_count; ++ei) {
            enemy* e = m_enemies[ei];
            if (!e || !e->is_alive() || e->is_encased()) continue;
            if (proj->get_hitbox().intersects(e->get_hitbox())) {
                e->apply_snow();
                proj->expire();
                if (e->is_encased()) {
                    float ex  = e->getPosition().x;
                    float ey  = e->getPosition().y;
                    float dir = (ex < WINDOW_WIDTH / 2.f) ? 1.f : -1.f;
                    add_projectile(new rolling_snowball(ex, ey, dir));
                    e->defeat();
                }
                break;
            }
        }

        if (m_boss && m_boss->is_alive() && !proj->is_expired()) {
            if (proj->get_hitbox().intersects(m_boss->get_hitbox())) {
                m_boss->apply_snow();
                if (m_boss->get_snow_level() == snow_level::Full)
                    m_boss->takeDamage();
                proj->expire();
            }
        }
        if (m_gamakichi_boss && m_gamakichi_boss->is_alive() && !proj->is_expired()) {
            if (proj->get_hitbox().intersects(m_gamakichi_boss->get_hitbox())) {
                m_gamakichi_boss->apply_snow();
                proj->expire();
            }
        }
    }
}

// takkar: rolling vs enemies

void GameWorld::resolve_rolling_vs_enemies() {
    for (int pi = 0; pi < m_proj_count; ++pi) {
        projectile* proj = m_projectiles[pi];
        if (!proj || proj->is_expired()) continue;
        if (proj->get_type() != projectile_type::encased_enemy) continue;

        // Find which player threw it 
        int ownerIdx = 0;
        if (m_multiplayer) {
            // Award points to nearest player 
            float d0 = m_players[0].getPosition().x - proj->get_hitbox().get_left();
            float d1 = m_players[1].getPosition().x - proj->get_hitbox().get_left();
            if (d0 < 0.f) d0 = -d0;
            if (d1 < 0.f) d1 = -d1;
            if (d1 < d0) ownerIdx = 1;
        }

        for (int ei = 0; ei < m_enemy_count; ++ei) {
            enemy* e = m_enemies[ei];
            if (!e || !e->is_alive() || e->is_defeated()) continue;
            if (proj->get_hitbox().intersects(e->get_hitbox())) {
                proj->increment_chain();
                int chain     = proj->get_chain_count();
                int baseScore = e->get_score_value();
                int bonus     = (int)(baseScore * CHAIN_BONUS_PCT * (chain - 1));

                m_players[ownerIdx].addScore(baseScore + bonus);
                m_players[ownerIdx].addGems(3 + chain);
                e->defeat();
                spawn_power_up(e->getPosition().x, e->getPosition().y);
            }
        }
    }
}

// takkar: enemy vs player

void GameWorld::resolve_enemy_vs_player() {
    for (int pi = 0; pi < m_player_count; ++pi) {
        if (m_players[pi].isInvincible() || m_players[pi].isBalloonMode()) continue;
        for (int ei = 0; ei < m_enemy_count; ++ei) {
            enemy* e = m_enemies[ei];
            if (!e || !e->is_alive() || e->is_encased()) continue;
            if (m_players[pi].get_hitbox().intersects(e->get_hitbox()))
                m_players[pi].takeDamage();
        }
    }
}

// takar: power-up vs player

void GameWorld::resolve_power_up_vs_player() {
    for (int pi = 0; pi < m_player_count; ++pi) {
        for (int ui = 0; ui < m_pu_count; ++ui) {
            power_up* pu = m_powerups[ui];
            if (!pu || pu->isCollected()) continue;
            if (m_players[pi].get_hitbox().intersects(pu->get_hitbox())) {
                m_players[pi].applyPowerUp(pu->get_type());
                if (pu->get_type() == power_up_type::extra_life)
                    m_players[pi].addLife();
                pu->collect();
            }
        }
    }
}

// takar: knife/explosion vs player

void GameWorld::resolve_knife_vs_player() {
    for (int pi = 0; pi < m_player_count; ++pi) {
        if (m_players[pi].isInvincible()) continue;
        for (int ki = 0; ki < m_proj_count; ++ki) {
            projectile* proj = m_projectiles[ki];
            if (!proj || proj->is_expired()) continue;
            if (proj->get_type() != projectile_type::knife) continue;
            if (m_players[pi].get_hitbox().intersects(proj->get_hitbox())) {
                m_players[pi].takeDamage();
                proj->expire();
            }
        }
    }
}

void GameWorld::resolve_explosion_vs_player() {
    for (int pi = 0; pi < m_player_count; ++pi) {
        if (m_players[pi].isInvincible()) continue;
        for (int ki = 0; ki < m_proj_count; ++ki) {
            projectile* proj = m_projectiles[ki];
            if (!proj || proj->is_expired()) continue;
            if (proj->get_type() != projectile_type::explosion) continue;
            explosion* expl = static_cast<explosion*>(proj);
            if (!expl->is_dangerous()) continue;
            if (m_players[pi].get_hitbox().intersects(proj->get_hitbox()))
                m_players[pi].takeDamage();
        }
    }
}

// Cleanup safai nisf iman

void GameWorld::cleanup_defeated() {
    int newCount = 0;
    for (int i = 0; i < m_enemy_count; ++i) {
        if (m_enemies[i] && m_enemies[i]->is_defeated()) {
            if (m_enemies[i] == m_boss)          
                m_boss = nullptr;
            if (m_enemies[i] == m_gamakichi_boss) 
                m_gamakichi_boss = nullptr;
            delete m_enemies[i]; 
            m_enemies[i] = nullptr;
        } else {
            m_enemies[newCount++] = m_enemies[i];
        }
    }
    for (int i = newCount; i < m_enemy_count; ++i) m_enemies[i] = nullptr;
    m_enemy_count = newCount;

    newCount = 0;
    for (int i = 0; i < m_proj_count; ++i) {
        if (m_projectiles[i] && m_projectiles[i]->is_expired()) {
            delete m_projectiles[i]; m_projectiles[i] = nullptr;
        } else {
            m_projectiles[newCount++] = m_projectiles[i];
        }
    }
    for (int i = newCount; i < m_proj_count; ++i) m_projectiles[i] = nullptr;
    m_proj_count = newCount;

    newCount = 0;
    for (int i = 0; i < m_pu_count; ++i) {
        if (m_powerups[i] && m_powerups[i]->isCollected()) {
            delete m_powerups[i]; 
            m_powerups[i] = nullptr;
        } else {
            m_powerups[newCount++] = m_powerups[i];
        }
    }
    for (int i = newCount; i < m_pu_count; ++i) 
        m_powerups[i] = nullptr;
    m_pu_count = newCount;
}

// Check lvl complete

void GameWorld::check_level_complete() {
    if (m_level_complete) return;

    if (m_is_bonus_level && m_enemy_count == 0) {
        // bonus level: complete when all power ups are collected
        bool allCollected = true;
        for (int i = 0; i < m_pu_count; ++i)
            if (m_powerups[i] && !m_powerups[i]->isCollected()) 
            { allCollected = false; break; }
        if (!allCollected) return;
    }
    else {
        // Normal lvl: complete when all enemies are went
        for (int i = 0; i < m_enemy_count; ++i)
            if (m_enemies[i] && m_enemies[i]->is_alive()) 
                return;
    }
    m_level_complete = true;

    // Auto save P1 progress
    player_progress prog;
    for (int k = 0; k < MAX_STR; ++k) prog.username[k] = 0;
    strCopy(prog.username, m_username);
    prog.current_level   = m_current_level + 1;
    prog.lives_remaining = m_players[0].getLives();
    prog.gem_count = m_players[0].getGems();
    prog.high_score = m_players[0].getScore();
    m_db.save_progress(prog);

    // Boss gem rewards both players get them
    if (m_is_boss_level) {
        int reward = (m_current_level == 10) ? GAMAKICHI_GEM_REWARD : MOGERA_GEM_REWARD;
        for (int i = 0; i < m_player_count; ++i)
            m_players[i].addGems(reward);
    }
}

// Spawn power-up

void GameWorld::spawn_power_up(float x, float y) {
    if (rand() % 3 != 0) return;
    int r = rand() % 5;
    power_up* pu = nullptr;
    switch (r) {
        case 0: pu = new speed_boost_powerup(x, y); break;
        case 1: pu = new snowball_powerup(x, y); break;
        case 2: pu = new distance_powerup(x, y); break;
        case 3: pu = new balloon_powerup(x, y); break;
        case 4: pu = new extra_life_powerup(x, y);  break;
    }
    if (pu) add_power_up(pu);
}

// draw

void GameWorld::draw(sf::RenderTarget& target) {
    // Draw background image
    if (m_bg_loaded) {
        target.clear(sf::Color(10, 10, 30));
        m_bg_sprite.setColor(sf::Color(
            (sf::Uint8)(m_bg_color.r / 2 + 80),
            (sf::Uint8)(m_bg_color.g / 2 + 80),
            (sf::Uint8)(m_bg_color.b / 2 + 100),
            255));
        target.draw(m_bg_sprite);
        // transparent  so the game elements are visible
        sf::RectangleShape overlay({(float)WINDOW_WIDTH, (float)WINDOW_HEIGHT});
        overlay.setFillColor(sf::Color(0, 0, 0, 110));
        target.draw(overlay);
    } else {
        target.clear(m_bg_color);
    }

    for (int i = 0; i < m_plat_count; ++i)
        m_platforms[i].draw(target, m_show_hit_boxes);

    for (int i = 0; i < m_pu_count; ++i)
        if (m_powerups[i] && !m_powerups[i]->isCollected())
            m_powerups[i]->draw(target, m_show_hit_boxes);

    for (int i = 0; i < m_enemy_count; ++i)
        if (m_enemies[i] && m_enemies[i]->is_alive())
            m_enemies[i]->draw(target, m_show_hit_boxes);

    for (int i = 0; i < m_proj_count; ++i)
        if (m_projectiles[i] && !m_projectiles[i]->is_expired())
            m_projectiles[i]->draw(target, m_show_hit_boxes);

    for (int i = 0; i < m_player_count; ++i)
        m_players[i].draw(target, m_show_hit_boxes);

    m_hud.draw(target);

    if (m_is_boss_level) {
        if (m_boss && m_boss->is_alive())
            m_hud.draw_boss_bar(target, "MOGERA",
                                m_boss->getCurrentHealth(), m_boss->getMaxHealth());
        if (m_gamakichi_boss && m_gamakichi_boss->is_alive())
            m_hud.draw_boss_bar(target, "GAMAKICHI",
                                m_gamakichi_boss->getCurrentHealth(),
                                m_gamakichi_boss->getMaxHealth());
    }

    // Paused overlay
    if (m_paused) {
        sf::RectangleShape ov({ (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT });
        ov.setFillColor(sf::Color(0, 0, 0, 140));
        target.draw(ov);

        sf::Text t;
        t.setFont(m_font); t.setCharacterSize(40);
        t.setFillColor(sf::Color::White); t.setString("PAUSED");
        t.setOrigin(t.getLocalBounds().width / 2.f, 0);
        t.setPosition(WINDOW_WIDTH / 2.f, 210.f);
        target.draw(t);

        const char* shopHint = m_multiplayer
            ? "ESC/P - Resume    S - P1 Shop    K - P2 Shop"
            : "ESC/P - Resume    S - Shop";
        sf::Text h;
        h.setFont(m_font); h.setCharacterSize(16);
        h.setFillColor(sf::Color(180, 180, 220));
        h.setString(shopHint);
        h.setOrigin(h.getLocalBounds().width / 2.f, 0);
        h.setPosition(WINDOW_WIDTH / 2.f, 265.f);
        target.draw(h);

        // Show both players stats when paused
        if (m_multiplayer) {
            char statBuf[128];
            for (int pi = 0; pi < 2; ++pi) {
                strCopy(statBuf, pi == 0 ? "P1  Gems:" : "P2  Gems:");
                char num[16]; intToStr(num, m_players[pi].getGems());
                strAppend(statBuf, num);
                strAppend(statBuf, "  Lives:");
                intToStr(num, m_players[pi].getLives());
                strAppend(statBuf, num);
                sf::Text st;
                st.setFont(m_font); st.setCharacterSize(15);
                st.setFillColor(pi == 0 ? sf::Color(100, 200, 255) : sf::Color(220, 160, 60));
                st.setString(statBuf);
                st.setOrigin(st.getLocalBounds().width / 2.f, 0);
                st.setPosition(WINDOW_WIDTH / 2.f, 300.f + pi * 24.f);
                target.draw(st);
            }
        }
    }

    // Level complete 
    if (m_level_complete) {
        sf::RectangleShape ov({ (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT });
        ov.setFillColor(sf::Color(0, 30, 0, 150));
        target.draw(ov);

        sf::Text t;
        t.setFont(m_font); t.setCharacterSize(42);
        t.setFillColor(sf::Color(100, 255, 150));
        t.setString("LEVEL CLEAR!");
        t.setOrigin(t.getLocalBounds().width / 2.f, 0);
        t.setPosition(WINDOW_WIDTH / 2.f, 200.f);
        target.draw(t);

        if (m_is_bonus_level) {
            sf::Text b;
            b.setFont(m_font); b.setCharacterSize(22);
            b.setFillColor(sf::Color(255, 220, 50));
            b.setString("BONUS LEVEL!  +1000 pts  +10 gems each!");
            b.setOrigin(b.getLocalBounds().width / 2.f, 0);
            b.setPosition(WINDOW_WIDTH / 2.f, 260.f);
            target.draw(b);
        }

        sf::Text n;
        n.setFont(m_font); n.setCharacterSize(18);
        n.setFillColor(sf::Color(200, 200, 255));
        n.setString("Press ENTER for next level");
        n.setOrigin(n.getLocalBounds().width / 2.f, 0);
        n.setPosition(WINDOW_WIDTH / 2.f, 310.f);
        target.draw(n);
    }
}
