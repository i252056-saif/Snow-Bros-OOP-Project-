#include "Game.h"
#include "LevelConfig.h"
#include "StrUtil.h"
#include <iostream>
#include <cstdlib>

// Construction

game::game()
    : m_window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
               "Snow Bros", sf::Style::Titlebar | sf::Style::Close)
{
    m_window.setFramerateLimit(TARGET_FPS);

    // Font
    if (!m_font.loadFromFile("assets/font.ttf")) {}


    level_registry::init();

    m_input.clear();
    m_input2.clear();
    for (int i = 0; i < MAX_STR; ++i) 
    { m_username[i] = 0; m_username2[i] = 0; }

    load_assets();
}

game::~game() 
{ destroy_world(); }
void game::destroy_world() 
{ delete m_world; m_world = nullptr; }

// Asset loading

void game::load_assets() {
    //  Background
    if (m_bg_texture.loadFromFile("assets/images/Background.png")) {
        m_bg_sprite.setTexture(m_bg_texture);
        // Scale to fill the whole window (1096×265 → 800×600)
        float scaleX = (float)WINDOW_WIDTH  / (float)m_bg_texture.getSize().x;
        float scaleY = (float)WINDOW_HEIGHT / (float)m_bg_texture.getSize().y;
        m_bg_sprite.setScale(scaleX, scaleY);
    }

    // Dark menu screens
    m_bg_overlay.setSize({(float)WINDOW_WIDTH, (float)WINDOW_HEIGHT});
    m_bg_overlay.setFillColor(sf::Color(0, 0, 20, 155));  // blue dark

    // My heros
    const char* paths[3] = {
        "assets/images/Nick.png",
        "assets/images/Tom.png",
        "assets/images/Nova.png"
    };
    for (int i = 0; i < 3; ++i) {
        if (m_char_tex[i].loadFromFile(paths[i])) {
            m_char_tex[i].setSmooth(true);
            m_char_sprite[i].setTexture(m_char_tex[i]);
        }
    }
    m_chars_loaded = true;
}

// Music helper

void game::update_music() {
    MusicTrack desired = MusicTrack::None;

    switch (m_state) {
        case game_state::MainMenu:
        case game_state::Login:
        case game_state::Login2:
        case game_state::Register:
            desired = MusicTrack::Menu;
            break;

        case game_state::CharacterSelect:
        case game_state::GameOver:
        case game_state::Shop:
            desired = MusicTrack::LevelEvent;
            break;

        case game_state::Leaderboard:
            desired = MusicTrack::Menu;
            break;

        case game_state::Playing:
            if (m_world) {
                bool boss = m_world->is_boss_level();
                desired = boss ? MusicTrack::Boss : MusicTrack::Gameplay;
            }
            break;

        default:
            desired = MusicTrack::Menu;
            break;
    }

    m_audio.play(desired);
}

// Background draw helper

void game::draw_background(float alpha) {
    m_window.draw(m_bg_sprite);
    sf::Color oc = m_bg_overlay.getFillColor();
    oc.a = (sf::Uint8)(alpha * 255.f);
    m_bg_overlay.setFillColor(oc);
    m_window.draw(m_bg_overlay);
}

// Main loop

void game::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) m_window.close();

            switch (m_state) {
                case game_state::MainMenu: update_main_menu(event); break;
                case game_state::Login: update_login(event); break;
                case game_state::Login2: update_login2(event);  break;
                case game_state::Register:  update_register(event); break;
                case game_state::CharacterSelect: update_char_select(event); break;
                case game_state::Playing: update_playing(event); break;
                case game_state::GameOver: update_game_over(event); break;
                case game_state::Leaderboard:  update_leaderboard(event); break;
                case game_state::Shop:  update_shop(event);  break;
                default: break;
            }
        }

        // Continuous playing update
        if (m_state == game_state::Playing && m_world) {
            m_world->update(dt);

            if (m_world->request_shop()) {
                m_world->clear_shop_request();
                m_shop_player = m_world->get_shop_requester();
                m_state = game_state::Shop;
            } else if (m_world->is_game_over()) {
                if (!m_guest_mode && m_username[0])
                    m_db.submit_score(m_username,
                                      m_world->get_player(0).getScore(),
                                      m_world->get_current_level());
                if (m_multiplayer && m_username2[0])
                    m_db.submit_score(m_username2,
                                      m_world->get_player(1).getScore(),
                                      m_world->get_current_level());
                m_state = game_state::GameOver;
            }
        }

        // Update music every frame
        update_music();

        // Draw
        m_window.clear();
        switch (m_state) {
            case game_state::MainMenu:  draw_main_menu();  break;
            case game_state::Login:   draw_login();  break;
            case game_state::Login2:   draw_login2();  break;
            case game_state::Register:   draw_register();   break;
            case game_state::CharacterSelect: draw_char_select();  break;
            case game_state::Playing:
                if (m_world) m_world->draw(m_window);
                break;
            case game_state::GameOver: draw_game_over(); break;
            case game_state::Leaderboard: draw_leaderboard();break;
            case game_state::Shop: draw_shop();  break;
            default: break;
        }
        m_window.display();
    }
}

// Main Menu

void game::update_main_menu(const sf::Event& e) {
    if (e.type != sf::Event::KeyPressed) return;
    const int ITEMS = 6;
    if (e.key.code == sf::Keyboard::Up)   m_menu_sel = (m_menu_sel + ITEMS - 1) % ITEMS;
    if (e.key.code == sf::Keyboard::Down) m_menu_sel = (m_menu_sel + 1) % ITEMS;
    if (e.key.code == sf::Keyboard::Return) {
        switch (m_menu_sel) {
            case 0:
                m_input.clear();
                m_multiplayer = false;
                m_state = game_state::Login;
                break;
            case 1:
                m_guest_mode  = true;
                m_multiplayer = false;
                strCopy(m_username, "Guest");
                for (int i = 0; i < MAX_STR; ++i) m_username2[i] = 0;
                m_char_step = 0; m_char_sel = 0;
                m_state = game_state::CharacterSelect;
                break;
            case 2:
                m_input.clear();
                m_multiplayer = true;
                m_state = game_state::Login;
                break;
            case 3:
                m_state = game_state::Leaderboard;
                break;
            case 4:
                m_input.clear();
                m_state = game_state::Register;
                break;
            case 5:
                m_window.close();
                break;
        }
    }
}

void game::draw_main_menu() {
    draw_background(0.50f);

    // Big title 
    auto shadow = make_text("SNOW BROS", 68, sf::Color(0, 0, 0, 120),
                             (float)WINDOW_WIDTH / 2.f + 3.f, 63.f, true);
    shadow.setStyle(sf::Text::Bold);
    m_window.draw(shadow);

    auto title = make_text("SNOW BROS", 68, sf::Color(220, 245, 255),
                            (float)WINDOW_WIDTH / 2.f, 60.f, true);
    title.setStyle(sf::Text::Bold);
    m_window.draw(title);

    m_window.draw(make_text("OOP Project  |  Spring 2026", 16,
                             sf::Color(160, 200, 230),
                             (float)WINDOW_WIDTH / 2.f, 142.f, true));

    const char* items[] = {
        "Login  (Single Player)",
        "Play as Guest",
        "Multiplayer  (2 Players)",
        "Leaderboard",
        "Register",
        "Exit"
    };

    // Menu panel behind items
    draw_panel((float)WINDOW_WIDTH / 2.f - 175.f, 168.f, 350.f, 310.f);

    for (int i = 0; i < 6; ++i) {
        bool sel = (i == m_menu_sel);
        char buf[128];
        if (sel) { strCopy(buf, ">  "); strAppend(buf, items[i]); strAppend(buf, "  <"); }
        else  strCopy(buf, items[i]);

        sf::Color col = sel ? sf::Color(100, 235, 255) : sf::Color(210, 225, 240);
        m_window.draw(make_text(buf, sel ? 24 : 19, col,
                                (float)WINDOW_WIDTH / 2.f, 182.f + i * 46.f, true));
    }

    m_window.draw(make_text("Arrow Keys  |  ENTER to select",
                             13, sf::Color(140, 170, 200),
                             (float)WINDOW_WIDTH / 2.f, 500.f, true));
}

// Input helpers

void game::input_append(char* field, char c) const {
    int len = strLen(field);
    if (len < MAX_STR - 2) { field[len] = c; field[len + 1] = '\0'; }
}
void game::input_backspace(char* field) const {
    int len = strLen(field);
    if (len > 0) field[len - 1] = '\0';
}

// Login P1

void game::update_login(const sf::Event& e) {
    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code == sf::Keyboard::Escape) { m_state = game_state::MainMenu; return; }
        if (e.key.code == sf::Keyboard::Tab)
            m_input.active_field = m_input.active_field == 0 ? 1 : 0;
        if (e.key.code == sf::Keyboard::BackSpace) {
            if (m_input.active_field == 0) input_backspace(m_input.username);
            else  input_backspace(m_input.password);
        }
        if (e.key.code == sf::Keyboard::Return) {
            if (m_db.login_user(m_input.username, m_input.password)) {
                strCopy(m_username, m_input.username);
                m_guest_mode = false;
                if (m_multiplayer) { m_input2.clear(); m_state = game_state::Login2; }
                else               { m_char_step = 0; m_char_sel = 0; m_state = game_state::CharacterSelect; }
            } else {
                strCopy(m_input.message, "Invalid username or password.");
            }
        }
    }
    if (e.type == sf::Event::TextEntered) {
        char c = (char)e.text.unicode;
        if (c >= 32 && c < 127) {
            if (m_input.active_field == 0) input_append(m_input.username, c);
            else input_append(m_input.password, c);
        }
    }
}

void game::draw_login() {
    draw_background(0.55f);
    draw_panel(200, 155, 400, 285);
    const char* title = m_multiplayer ? "LOGIN  —  PLAYER 1" : "LOGIN";
    m_window.draw(make_text(title, 28, sf::Color(200, 235, 255),
                             (float)WINDOW_WIDTH / 2.f, 170.f, true));

    char masked[MAX_STR];
    int plen = strLen(m_input.password);
    for (int i = 0; i < plen; ++i) masked[i] = '*'; masked[plen] = '\0';

    draw_field("Username:", m_input.username, m_input.active_field == 0, false, 220.f, 222.f);
    draw_field("Password:", masked,m_input.active_field == 1, false, 220.f, 285.f);

    m_window.draw(make_text("TAB - switch  |  ENTER - login  |  ESC - back",
                             13, sf::Color(140, 160, 200),
                             (float)WINDOW_WIDTH / 2.f, 378.f, true));
    if (m_input.message[0])
        m_window.draw(make_text(m_input.message, 15, sf::Color(255, 110, 110),
                                 (float)WINDOW_WIDTH / 2.f, 410.f, true));
}

//Login P2

void game::update_login2(const sf::Event& e) {
    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code == sf::Keyboard::Escape) { m_input.clear(); m_state = game_state::Login; return; }
        if (e.key.code == sf::Keyboard::Tab)
            m_input2.active_field = m_input2.active_field == 0 ? 1 : 0;
        if (e.key.code == sf::Keyboard::BackSpace) {
            if (m_input2.active_field == 0) input_backspace(m_input2.username);
            else input_backspace(m_input2.password);
        }
        if (e.key.code == sf::Keyboard::Return) {
            if (strEq(m_input2.username, m_username)) {
                strCopy(m_input2.message, "P2 must use a different account.");
                return;
            }
            if (m_db.login_user(m_input2.username, m_input2.password)) {
                strCopy(m_username2, m_input2.username);
                m_char_step = 0; m_char_sel = 0; m_char_sel2 = 0;
                m_state = game_state::CharacterSelect;
            } else {
                strCopy(m_input2.message, "Invalid username or password.");
            }
        }
    }
    if (e.type == sf::Event::TextEntered) {
        char c = (char)e.text.unicode;
        if (c >= 32 && c < 127) {
            if (m_input2.active_field == 0) input_append(m_input2.username, c);
            else input_append(m_input2.password, c);
        }
    }
}

void game::draw_login2() {
    draw_background(0.55f);
    draw_panel(200, 155, 400, 295);
    m_window.draw(make_text("LOGIN  —  PLAYER 2", 28, sf::Color(255, 200, 100),
                             (float)WINDOW_WIDTH / 2.f, 170.f, true));
    char hint[MAX_STR * 2];
    strCopy(hint, "P1: "); strAppend(hint, m_username);
    m_window.draw(make_text(hint, 14, sf::Color(120, 220, 120),
                             (float)WINDOW_WIDTH / 2.f, 205.f, true));

    char masked[MAX_STR];
    int plen = strLen(m_input2.password);
    for (int i = 0; i < plen; ++i) masked[i] = '*'; masked[plen] = '\0';

    draw_field("Username:", m_input2.username, m_input2.active_field == 0, false, 220.f, 232.f);
    draw_field("Password:", masked, m_input2.active_field == 1, false, 220.f, 296.f);

    m_window.draw(make_text("TAB - switch  |  ENTER - login  |  ESC - back",
                             13, sf::Color(140, 160, 200),
                             (float)WINDOW_WIDTH / 2.f, 388.f, true));
    if (m_input2.message[0])
        m_window.draw(make_text(m_input2.message, 15, sf::Color(255, 110, 110),
                                 (float)WINDOW_WIDTH / 2.f, 420.f, true));
}

// Register

void game::update_register(const sf::Event& e) {
    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code == sf::Keyboard::Escape) { m_state = game_state::MainMenu; return; }
        if (e.key.code == sf::Keyboard::Tab)
            m_input.active_field = (m_input.active_field + 1) % 3;
        if (e.key.code == sf::Keyboard::BackSpace) {
            if      (m_input.active_field == 0) input_backspace(m_input.username);
            else if (m_input.active_field == 1) input_backspace(m_input.password);
            else                                 input_backspace(m_input.email);
        }
        if (e.key.code == sf::Keyboard::Return) {
            if (!m_input.username[0] || !m_input.password[0]) {
                strCopy(m_input.message, "Username and password required.");
            } else if (m_db.register_user(m_input.username, m_input.password, m_input.email)) {
                strCopy(m_input.message, "Account created! Please login.");
                for (int i = 0; i < MAX_STR; ++i) m_input.password[i] = 0;
                m_state = game_state::Login;
            } else {
                strCopy(m_input.message, "Username already taken.");
            }
        }
    }
    if (e.type == sf::Event::TextEntered) {
        char c = (char)e.text.unicode;
        if (c >= 32 && c < 127) {
            if (m_input.active_field == 0) input_append(m_input.username, c);
            else if (m_input.active_field == 1) input_append(m_input.password, c);
            else input_append(m_input.email, c);
        }
    }
}

void game::draw_register() {
    draw_background(0.55f);
    draw_panel(180, 125, 440, 340);
    m_window.draw(make_text("REGISTER", 30, sf::Color(200, 235, 255),
                             (float)WINDOW_WIDTH / 2.f, 140.f, true));

    char masked[MAX_STR];
    int plen = strLen(m_input.password);
    for (int i = 0; i < plen; ++i) masked[i] = '*'; masked[plen] = '\0';

    draw_field("Username:",m_input.username, m_input.active_field == 0, false, 200.f, 205.f);
    draw_field("Password:",masked,m_input.active_field == 1, false, 200.f, 270.f);
    draw_field("Email (optional):", m_input.email,m_input.active_field == 2, false, 200.f, 335.f);

    m_window.draw(make_text("TAB - switch  |  ENTER - register  |  ESC - back",
                             13, sf::Color(140, 160, 200),
                             (float)WINDOW_WIDTH / 2.f, 420.f, true));
    if (m_input.message[0])
        m_window.draw(make_text(m_input.message, 15, sf::Color(255, 210, 100),
                                 (float)WINDOW_WIDTH / 2.f, 450.f, true));
}

//Character Select

void game::update_char_select(const sf::Event& e) {
    if (e.type != sf::Event::KeyPressed) return;
    int& cur = (m_char_step == 0) ? m_char_sel : m_char_sel2;
    if (e.key.code == sf::Keyboard::Left)   
        cur = (cur + 2) % 3;
    if (e.key.code == sf::Keyboard::Right)  
        cur = (cur + 1) % 3;
    if (e.key.code == sf::Keyboard::Escape) 
    { m_state = game_state::MainMenu; return; }
    if (e.key.code == sf::Keyboard::Return) {
        if (m_multiplayer && m_char_step == 0) {
            m_char_sel2 = (m_char_sel + 1) % 3;
            m_char_step = 1;
        } else {
            start_new_game();
        }
    }
}

void game::draw_char_select() {
    draw_background(0.42f);

    const char* phase_title = (!m_multiplayer || m_char_step == 0)
        ? "SELECT  CHARACTER"
        : "PLAYER 2  —  SELECT CHARACTER";
    m_window.draw(make_text(phase_title, 32, sf::Color(220, 245, 255),
                             (float)WINDOW_WIDTH / 2.f, 28.f, true));

    const char* active_user = (m_char_step == 0) ? m_username : m_username2;
    char buf[MAX_STR * 2];
    strCopy(buf, m_char_step == 0 ? "Player 1: " : "Player 2: ");
    strAppend(buf, active_user);
    m_window.draw(make_text(buf, 15, sf::Color(160, 210, 255),
                             (float)WINDOW_WIDTH / 2.f, 72.f, true));

    if (m_multiplayer && m_char_step == 1) {
        const char* names[] = { "Nick", "Tom", "Nova" };
        char hint[MAX_STR * 2];
        strCopy(hint, "P1 chose:  "); strAppend(hint, names[m_char_sel]);
        m_window.draw(make_text(hint, 13, sf::Color(100, 230, 130),
                                 (float)WINDOW_WIDTH / 2.f, 94.f, true));
    }

    const char* names[] = { "Nick", "Tom", "Nova" };
    const char* descs[] = {
        "Balanced stats.\nDefault hero.",
        "Faster speed.\nShorter range.",
        "Longer range.\nSlower speed."
    };

    int& cur = (m_char_step == 0) ? m_char_sel : m_char_sel2;

    // Card positions spread in window 
    float cardW = 180.f, cardH = 310.f;
    float startX = (WINDOW_WIDTH - (3 * cardW + 2 * 30.f)) / 2.f;

    for (int i = 0; i < 3; ++i) {
        float cx   = startX + i * (cardW + 30.f);
        bool  sel  = (i == cur);
        bool  taken = (m_multiplayer && m_char_step == 1 && i == m_char_sel);

        // Card background
        sf::RectangleShape card({ cardW, cardH });
        card.setPosition(cx, 110.f);
        card.setFillColor(taken   ? sf::Color(50, 10, 10, 200) :
                          sel     ? sf::Color(20, 50, 100, 220) :
                                    sf::Color(10, 20, 50, 200));
        card.setOutlineColor(taken  ? sf::Color(150, 40,  40) :
                             sel    ? sf::Color(120, 210, 255) :
                                      sf::Color(60,  90, 140));
        card.setOutlineThickness(sel ? 3.f : 1.5f);
        m_window.draw(card);

        // Character  image
        if (m_chars_loaded) {
            sf::Sprite& spr = m_char_sprite[i];
            // Scale portrait
            float texW = (float)m_char_tex[i].getSize().x;
            float texH = (float)m_char_tex[i].getSize().y;
            float fitW = cardW - 20.f;
            float fitH = 190.f;
            float scaleX = fitW / texW;
            float scaleY = fitH / texH;
            float scale  = scaleX < scaleY ? scaleX : scaleY;
            spr.setScale(scale, scale);
            // Centre in card
            float imgW = texW * scale;
            float imgH = texH * scale;
            spr.setPosition(cx + (cardW - imgW) / 2.f, 118.f);
            sf::Uint8 alpha = taken ? 70 : 255;
            spr.setColor({ 255, 255, 255, alpha });
            m_window.draw(spr);
        }

        // Name
        m_window.draw(make_text(names[i], 20,
                                taken ? sf::Color(160, 60, 60) :
                                sel   ? sf::Color(100, 225, 255) :
                                        sf::Color(240, 240, 255),
                                cx + cardW / 2.f, 316.f, true));

        // Description
        m_window.draw(make_text(descs[i], 12, sf::Color(170, 185, 215),
                                cx + cardW / 2.f, 342.f, true));

        if (taken) {
            m_window.draw(make_text("TAKEN", 14, sf::Color(220, 60, 60),
                                    cx + cardW / 2.f, 370.f, true));
        }

        // Selection arrow undr selected card
        if (sel) {
            m_window.draw(make_text("▲", 18, sf::Color(120, 210, 255),
                                    cx + cardW / 2.f, 428.f, true));
        }
    }

    m_window.draw(make_text("< >  Arrow Keys   |   ENTER - Confirm   |   ESC - Menu",
                             14, sf::Color(160, 180, 220),
                             (float)WINDOW_WIDTH / 2.f, 460.f, true));
}

// Playing

void game::update_playing(const sf::Event& e) {
    if (m_world) m_world->handle_event(e);
    if (e.type == sf::Event::KeyPressed)
        if (e.key.code == sf::Keyboard::Return && m_world && m_world->is_level_complete())
            advance_level();
}

// Game Over

void game::update_game_over(const sf::Event& e) {
    if (e.type != sf::Event::KeyPressed) 
        return;
    if (e.key.code == sf::Keyboard::Return) 
        start_new_game();
    if (e.key.code == sf::Keyboard::Escape) 
        m_state = game_state::MainMenu;
    if (e.key.code == sf::Keyboard::L)  
        m_state = game_state::Leaderboard;
}

void game::draw_game_over() {
    draw_background(0.65f);

    // Title
    auto sh = make_text("GAME OVER", 62, sf::Color(0,0,0,100),
                         (float)WINDOW_WIDTH/2.f+3.f, 123.f, true);
    sh.setStyle(sf::Text::Bold); m_window.draw(sh);
    auto t = make_text("GAME OVER", 62, sf::Color(240, 70, 70),
                        (float)WINDOW_WIDTH/2.f, 120.f, true);
    t.setStyle(sf::Text::Bold); m_window.draw(t);

    draw_panel((float)WINDOW_WIDTH/2.f - 220.f, 200.f, 440.f, 200.f);

    if (m_world) {
        char buf[MAX_STR * 2]; char num[32];

        // P1 row
        strCopy(buf, m_username[0] ? m_username : "P1");
        strAppend(buf, "   Score: ");
        intToStr(num, m_world->get_player(0).getScore()); 
        strAppend(buf, num);
        strAppend(buf, "   Lives: ");
        intToStr(num, m_world->get_player(0).getLives()); 
        strAppend(buf, num);
        m_window.draw(make_text(buf, 20, sf::Color(255, 230, 100),
                                 (float)WINDOW_WIDTH/2.f, 215.f, true));

        if (m_multiplayer) {
            strCopy(buf, m_username2[0] ? m_username2 : "P2");
            strAppend(buf, "   Score: ");
            intToStr(num, m_world->get_player(1).getScore()); strAppend(buf, num);
            strAppend(buf, "   Lives: ");
            intToStr(num, m_world->get_player(1).getLives()); strAppend(buf, num);
            m_window.draw(make_text(buf, 20, sf::Color(220, 170, 60),
                                     (float)WINDOW_WIDTH/2.f, 250.f, true));
        }

        strCopy(buf, "Level Reached:  ");
        intToStr(num, m_world->get_current_level()); strAppend(buf, num);
        m_window.draw(make_text(buf, 18, sf::Color(190, 200, 240),
                                 (float)WINDOW_WIDTH/2.f, 300.f, true));
    }

    m_window.draw(make_text("ENTER - Play Again     ESC - Menu     L - Leaderboard",
                             16, sf::Color(160, 170, 210),
                             (float)WINDOW_WIDTH/2.f, 440.f, true));
}

// Shining stars Leaderboard

void game::update_leaderboard(const sf::Event& e) {
    if (e.type != sf::Event::KeyPressed) return;
    if (e.key.code == sf::Keyboard::Escape || e.key.code == sf::Keyboard::Return)
        m_state = game_state::MainMenu;
}

void game::draw_leaderboard() {
    draw_background(0.58f);
    draw_panel(95, 55, 610, 475);

    m_window.draw(make_text("LEADERBOARD", 34, sf::Color(255, 225, 60),
                             (float)WINDOW_WIDTH / 2.f, 68.f, true));

    leaderboard_entry entries[MAX_LEADERBOARD];
    int n = m_db.get_top_scores(entries, MAX_LEADERBOARD);

    if (n == 0) {
        m_window.draw(make_text("No scores yet. Play the game!", 18,
                                 sf::Color(160, 165, 210),
                                 (float)WINDOW_WIDTH / 2.f, 210.f, true));
    } else {
        m_window.draw(make_text("#   Name               Score       Lvl    Date",
                                 14, sf::Color(120, 140, 190), 115.f, 118.f));
        for (int i = 0; i < n; ++i) {
            char row[128]; char num[16];
            char rankCol[4], nameCol[20], scoreCol[12], lvlCol[7];
            intToStr(num, i+1); padRight(rankCol,  num,                  3);
            padRight(nameCol,  entries[i].username, 19);
            intToStr(num, entries[i].score);
            padRight(scoreCol, num,                  11);
            intToStr(num, entries[i].level_reached);
            padRight(lvlCol,   num,                   6);
            strCopy(row, rankCol);
            strAppend(row, nameCol); strAppend(row, scoreCol);
            strAppend(row, lvlCol);  strAppend(row, entries[i].date);

            sf::Color col = (i==0) ? sf::Color(255,215,0) :
                            (i==1) ? sf::Color(200,200,200) :
                            (i==2) ? sf::Color(210,140,60) :
                                     sf::Color(200,205,230);
            m_window.draw(make_text(row, 15, col, 115.f, 146.f + i*30.f));
        }
    }
    m_window.draw(make_text("ESC / ENTER - Back", 14, sf::Color(110, 130, 170),
                             (float)WINDOW_WIDTH / 2.f, 548.f, true));
}

// Shop market

void game::update_shop(const sf::Event& e) {
    if (e.type != sf::Event::KeyPressed) return;
    if (e.key.code == sf::Keyboard::Escape) { m_state = game_state::Playing; return; }
    if (e.key.code == sf::Keyboard::Up)   m_shop_sel = (m_shop_sel + 4) % 5;
    if (e.key.code == sf::Keyboard::Down) m_shop_sel = (m_shop_sel + 1) % 5;
    if (e.key.code == sf::Keyboard::Return && m_world) {
        int costs[] = { SHOP_EXTRA_LIFE, SHOP_SPEED_BOOST, SHOP_SNOWBALL_POWER,
                        SHOP_DISTANCE, SHOP_BALLOON };
        power_up_type types[] = {
            power_up_type::extra_life,    power_up_type::speed_boost,
            power_up_type::snowball_power, power_up_type::distance_increase,
            power_up_type::balloon_mode
        };
        Player& buyer = m_world->get_player(m_shop_player);
        if (buyer.getGems() >= costs[m_shop_sel]) {
            buyer.addGems(-costs[m_shop_sel]);
            buyer.applyPowerUp(types[m_shop_sel]);
            m_state = game_state::Playing;
        }
    }
}

void game::draw_shop() {
    draw_background(0.60f);
    draw_panel(135, 75, 530, 435);
    m_window.draw(make_text("ITEM  SHOP", 32, sf::Color(255, 225, 60),
                             (float)WINDOW_WIDTH / 2.f, 90.f, true));

    if (m_world) {
        char buf[MAX_STR * 2]; char num[32];
        const char* pname = (m_shop_player == 0) ? m_username : m_username2;
        if (!pname[0]) pname = m_shop_player == 0 ? "P1" : "P2";
        strCopy(buf, "Shopping: "); strAppend(buf, pname);
        strAppend(buf, "    Gems: ");
        intToStr(num, m_world->get_player(m_shop_player).getGems());
        strAppend(buf, num);
        m_window.draw(make_text(buf, 17, sf::Color(100, 255, 180),
                                 (float)WINDOW_WIDTH / 2.f, 138.f, true));
    }

    const char* items[] = {
        "Extra Life                            50 gems",
        "Speed Boost  x1.5               20 gems   (15s)",
        "1-Hit Encase                      30 gems   (level)",
        "Max Throw Range              25 gems   (level)",
        "Balloon Mode                     35 gems   (10s)"
    };
    for (int i = 0; i < 5; ++i) {
        bool sel = (i == m_shop_sel);
        if (sel) {
            sf::RectangleShape hl({ 490.f, 32.f });
            hl.setPosition(145.f, 178.f + i * 50.f);
            hl.setFillColor(sf::Color(30, 65, 120, 190));
            m_window.draw(hl);
        }
        m_window.draw(make_text(items[i], 17,
                                sel ? sf::Color(100, 225, 255) : sf::Color(200, 210, 230),
                                160.f, 183.f + i * 50.f));
    }
    m_window.draw(make_text("ENTER - Buy     ESC - Close", 14, sf::Color(120, 140, 180),
                             (float)WINDOW_WIDTH / 2.f, 442.f + 50.f, true));
}

// UI Helpers

sf::Text game::make_text(const char* str, unsigned size,
                          sf::Color col, float x, float y, bool centered)
{
    sf::Text t;
    t.setFont(m_font);
    t.setCharacterSize(size);
    t.setFillColor(col);
    t.setString(str);
    if (centered) {
        auto b = t.getLocalBounds();
        t.setOrigin(b.left + b.width / 2.f, b.top);
    }
    t.setPosition(x, y);
    return t;
}

void game::draw_panel(float x, float y, float w, float h) {
    sf::RectangleShape p({ w, h });
    p.setPosition(x, y);
    p.setFillColor(sf::Color(8, 12, 40, 210));
    p.setOutlineColor(sf::Color(70, 110, 180, 200));
    p.setOutlineThickness(2.f);
    m_window.draw(p);
}

void game::draw_field(const char* label, const char* value,
                       bool active, bool /*mask*/,
                       float x, float y)
{
    sf::Color fc = active ? sf::Color(120, 230, 255) : sf::Color(160, 175, 210);
    m_window.draw(make_text(label, 15, sf::Color(140, 165, 210), x, y));

    char display[MAX_STR + 2];
    strCopy(display, value);
    if (active) strAppend(display, "_");

    sf::RectangleShape box({ 360.f, 28.f });
    box.setPosition(x, y + 20.f);
    box.setFillColor(sf::Color(20, 25, 60));
    box.setOutlineColor(fc);
    box.setOutlineThickness(1.5f);
    m_window.draw(box);
    m_window.draw(make_text(display, 15, sf::Color::White, x + 8.f, y + 24.f));
}

// start_new_game / advance_level

void game::start_new_game() {
    m_current_level = 1;
    if (!m_guest_mode && m_username[0]) {
        player_progress prog = m_db.load_progress(m_username);
        if (prog.current_level > 1) m_current_level = prog.current_level;
        if (m_current_level > level_registry::count()) m_current_level = 1;
    }
    destroy_world();
    m_world = new GameWorld(m_font, m_username, m_username2, m_db,
                             m_multiplayer, m_char_sel, m_char_sel2);
    m_world->load_level(m_current_level);
    m_state = game_state::Playing;
}

void game::advance_level() {
    ++m_current_level;
    if (m_current_level > level_registry::count()) {
        if (!m_guest_mode && m_username[0])
            m_db.submit_score(m_username,
                               m_world->get_player(0).getScore(), m_current_level - 1);
        if (m_multiplayer && m_username2[0])
            m_db.submit_score(m_username2,
                               m_world->get_player(1).getScore(), m_current_level - 1);
        m_state = game_state::Leaderboard;
        return;
    }

    int lives0 = m_world->get_player(0).getLives();
    int gems0  = m_world->get_player(0).getGems();
    int score0 = m_world->get_player(0).getScore();
    int lives1 = 0, gems1 = 0, score1 = 0;
    if (m_multiplayer) {
        lives1 = m_world->get_player(1).getLives();
        gems1  = m_world->get_player(1).getGems();
        score1 = m_world->get_player(1).getScore();
    }

    destroy_world();
    m_world = new GameWorld(m_font, m_username, m_username2, m_db,
                             m_multiplayer, m_char_sel, m_char_sel2);
    m_world->load_level(m_current_level);

    m_world->get_player(0).setLives(lives0);
    m_world->get_player(0).setGems(gems0);
    m_world->get_player(0).setScore(score0);
    if (m_multiplayer) {
        m_world->get_player(1).setLives(lives1);
        m_world->get_player(1).setGems(gems1);
        m_world->get_player(1).setScore(score1);
    }
    m_state = game_state::Playing;
}
