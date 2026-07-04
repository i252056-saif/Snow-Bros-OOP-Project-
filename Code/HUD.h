#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

/*
 class hud
  score, lives, gems, level indicator, and boss health bar.
  display class no game logic.
 */
class hud {
public:
    explicit hud(sf::Font& font);

    void update(const Player& p1, const Player* p2,
                int level_number, int totalLevels);
    void draw(sf::RenderTarget& target) const;

    void draw_boss_bar(sf::RenderTarget& target,
                     const char* boss_name,
                     int hp, int max_hp) const;

private:
    sf::Font& m_font;
    sf::Text  m_score_text;
    sf::Text  m_lives_text;
    sf::Text  m_gems_text;
    sf::Text  m_level_text;
    sf::Text  m_p2_text;
    sf::RectangleShape m_top_bar;

    // helper
    void configText(sf::Text& t, unsigned size,
                    sf::Color col, float x, float y) const;
};
