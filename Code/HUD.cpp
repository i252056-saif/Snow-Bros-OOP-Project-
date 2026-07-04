#include "HUD.h"
#include "Constants.h"
#include "StrUtil.h"

hud::hud(sf::Font& font) : m_font(font) {
    m_top_bar.setSize({(float)WINDOW_WIDTH, 36.f});
    m_top_bar.setFillColor(sf::Color(0, 0, 0, 160));

    configText(m_score_text, 16, sf::Color(255, 230, 100),8.f,8.f);
    configText(m_lives_text, 16, sf::Color(255, 100, 100), 8.f,22.f);
    configText(m_gems_text,  16, sf::Color(100, 255, 180), 580.f,8.f);
    configText(m_level_text, 16, sf::Color(200, 200, 255), 340.f,8.f);
    configText(m_p2_text, 14, sf::Color(220, 160,  80), 200.f,8.f);
}

void hud::configText(sf::Text& t, unsigned size,
                      sf::Color col, float x, float y) const
{
    t.setFont(m_font);
    t.setCharacterSize(size);
    t.setFillColor(col);
    t.setPosition(x, y);
}

void hud::update(const Player& p1, const Player* p2,
                  int level_number, int totalLevels)
{
    char buf[MAX_STR];

    fmtInt(buf, "Score: ", p1.getScore());
    m_score_text.setString(buf);

    fmtInt(buf, "Lives: ", p1.getLives());
    m_lives_text.setString(buf);

    fmtInt(buf, "Gems: ", p1.getGems());
    m_gems_text.setString(buf);

    fmtFraction(buf, "Level ", level_number, totalLevels);
    m_level_text.setString(buf);

    if (p2) {
        char buf2[MAX_STR * 2];
        strCopy(buf2, "P2 Score:");
        char num[16]; intToStr(num, p2->getScore());
        strAppend(buf2, num);
        strAppend(buf2, "  Lives:");
        intToStr(num, p2->getLives());
        strAppend(buf2, num);
        m_p2_text.setString(buf2);
    } else {
        m_p2_text.setString("");
    }
}

void hud::draw(sf::RenderTarget& target) const {
    target.draw(m_top_bar);
    target.draw(m_score_text);
    target.draw(m_lives_text);
    target.draw(m_gems_text);
    target.draw(m_level_text);
    target.draw(m_p2_text);
}

void hud::draw_boss_bar(sf::RenderTarget& target,
                         const char* bossName,
                         int hp, int maxHp) const
{
    float barW = 300.f, barH = 14.f;
    float barX = (WINDOW_WIDTH - barW) / 2.f, barY = 42.f;
    float fill = barW * ((float)hp / (float)maxHp);
    if (fill < 0.f) fill = 0.f;

    sf::RectangleShape bg({barW, barH});
    bg.setPosition(barX, barY);
    bg.setFillColor(sf::Color(60, 0, 0, 200));
    target.draw(bg);

    sf::RectangleShape bar({fill, barH});
    bar.setPosition(barX, barY);
    bar.setFillColor(sf::Color(220, 40, 40));
    target.draw(bar);

    sf::Text lbl;
    lbl.setFont(m_font);
    lbl.setCharacterSize(13);
    lbl.setFillColor(sf::Color::White);
    lbl.setString(bossName);
    lbl.setPosition(barX, barY - 16.f);
    target.draw(lbl);
}
