#include "LevelConfig.h"
#include "StrUtil.h"

level_config level_registry::s_levels[10];
int         level_registry::s_count       = 0;
bool        level_registry::s_initialized = false;

// Helpers to fill arrays 
static void addSpawn(level_config& c, enemy_spawn::Type t, float x, float y,
                     enemy_variant v = enemy_variant::Red)
{
    if (c.spawn_count < MAX_ENEMIES) {
        c.spawns[c.spawn_count] = {t, x, y, v};
        c.spawn_count++;
    }
}
static void addPlat(level_config& c, float x, float y, float w, float h) {
    if (c.platformCount < MAX_PLATFORMS) {
        c.platforms[c.platformCount++] = {x, y, w, h};
    }
}

void level_registry::init() {
    if (s_initialized) return;
    s_initialized = true;
    s_count       = 10;

    using T = enemy_spawn::Type;

    // Lvl 1
    {
        level_config& c = s_levels[0];
        c.level_number = 1; c.is_boss_level = false;
        c.bg_color = sf::Color(20,40,80);
        strCopy(c.title, "Snowy Village");
        addPlat(c, 0,560,800,40);
        addPlat(c,100,420,200,18); addPlat(c,500,420,200,18);
        addPlat(c,300,300,220,18);
        addPlat(c, 50,200,150,18); addPlat(c,600,200,150,18);
        addSpawn(c, T::botom,160,390); addSpawn(c, T::botom,550,390);
        addSpawn(c, T::botom,380,270); addSpawn(c, T::botom,110,170);
    }
    // Lvl 2
    {
        level_config& c = s_levels[1];
        c.level_number = 2; c.is_boss_level = false;
        c.bg_color = sf::Color(10,30,60);
        strCopy(c.title, "Ice Caves");
        addPlat(c,  0,560,800,40);
        addPlat(c, 80,440,160,18); addPlat(c,320,380,180,18); addPlat(c,570,430,180,18);
        addPlat(c,200,280,140,18); addPlat(c,460,250,140,18); addPlat(c,350,150,100,18);
        addSpawn(c, T::botom,150,410); addSpawn(c, T::botom,620,400);
        addSpawn(c, T::flyingfoogafoog,400,350);
        addSpawn(c, T::botom,260,250); addSpawn(c, T::flyingfoogafoog,500,220);
    }
    // Level 3
    {
        level_config& c = s_levels[2];
        c.level_number = 3; c.is_boss_level = false;
        c.bg_color = sf::Color(15,50,30);
        strCopy(c.title, "Winter Forest");
        addPlat(c,  0,560,800,40);
        addPlat(c, 60,460,140,18); addPlat(c,280,400,240,18); addPlat(c,600,460,140,18);
        addPlat(c,140,300,120,18); addPlat(c,500,280,120,18); addPlat(c,320,200,180,18);
        addPlat(c, 50,140,100,18); addPlat(c,660,140,100,18);
        addSpawn(c, T::botom,100,430);
        addSpawn(c, T::flyingfoogafoog,380,370);
        addSpawn(c, T::tornado,620,430);
        addSpawn(c, T::botom,190,270);
        addSpawn(c, T::flyingfoogafoog,550,250);
        addSpawn(c, T::tornado,400,170);
    }
    //Lvl 4
    {
        level_config& c = s_levels[3];
        c.level_number = 4; c.is_boss_level = false;
        c.bg_color = sf::Color(0,20,50);
        strCopy(c.title, "Frozen Lake");
        addPlat(c, 0, 560, 800, 40);
        addPlat(c, 120, 460, 120, 18); addPlat(c, 400, 460, 120, 18); addPlat(c, 660, 460, 120, 18);
        addPlat(c, 260, 360, 120, 18); addPlat(c, 540, 340, 120, 18);
        addPlat(c, 80, 260, 100, 18); addPlat(c, 360, 230, 100, 18); addPlat(c, 620, 250, 100, 18);
        addPlat(c, 230, 140, 140, 18); addPlat(c, 500, 140, 140, 18);
        c.is_collectable_level = true;
    }
    //  Level 5 — Badmash: mogera
    {
        level_config& c = s_levels[4];
        c.level_number = 5; c.is_boss_level = true;
        c.bg_color = sf::Color(30,5,5);
        strCopy(c.title, "Dark Lair");
        addPlat(c,  0,560,800,40);
        addPlat(c,  0,380,200,18); addPlat(c,600,380,200,18);
        addPlat(c,280,280,240,18);
        addPlat(c,100,180,140,18); addPlat(c,560,180,140,18);
        addSpawn(c, T::mogera,360,480);
    }
    // Level 6 
    {
        level_config& c = s_levels[5];
        c.level_number = 6; c.is_boss_level = false;
        c.bg_color = sf::Color(50,20,5);
        strCopy(c.title, "Volcanic Wasteland");
        addPlat(c,  0,560,800,40);
        addPlat(c, 80,430,160,18); addPlat(c,330,380,160,18); addPlat(c,590,430,160,18);
        addPlat(c,170,280,130,18); addPlat(c,500,260,130,18); addPlat(c,320,170,170,18);
        addSpawn(c, T::botom,130,400,enemy_variant::Green);
        addSpawn(c, T::flyingfoogafoog,400,350,enemy_variant::Green);
        addSpawn(c, T::tornado,640,400,enemy_variant::Green);
        addSpawn(c, T::botom,220,250,enemy_variant::Green);
        addSpawn(c, T::flyingfoogafoog,550,230,enemy_variant::Green);
        addSpawn(c, T::tornado,400,140,enemy_variant::Green);
    }
    // Level 7 
    {
        level_config& c = s_levels[6];
        c.level_number = 7; c.is_boss_level = false;
        c.bg_color = sf::Color(20,10,40);
        strCopy(c.title, "Haunted Castle");
        addPlat(c,  0,560,800,40);
        addPlat(c, 50,450,130,18); addPlat(c,250,390,130,18);
        addPlat(c,460,380,130,18); addPlat(c,660,440,130,18);
        addPlat(c,150,280,110,18); addPlat(c,380,240,110,18);
        addPlat(c,590,260,110,18); addPlat(c,280,150,160,18);
        addSpawn(c, T::flyingfoogafoog,100,420,enemy_variant::Blue);
        addSpawn(c, T::tornado,300,360,enemy_variant::Blue);
        addSpawn(c, T::tornado,510,350,enemy_variant::Blue);
        addSpawn(c, T::flyingfoogafoog,700,410,enemy_variant::Blue);
        addSpawn(c, T::tornado,200,250,enemy_variant::Blue);
        addSpawn(c, T::flyingfoogafoog,630,230,enemy_variant::Blue);
        addSpawn(c, T::tornado,360,120,enemy_variant::Blue);
    }
    //  Level 8
    {
        level_config& c = s_levels[7];
        c.level_number = 8; c.is_boss_level = false;
        c.bg_color = sf::Color(5,30,50);
        strCopy(c.title, "Alien Planet");
        addPlat(c,  0,560,800,40);
        addPlat(c,100,440,110,18); addPlat(c,290,410,110,18);
        addPlat(c,480,400,110,18); addPlat(c,670,440,110,18);
        addPlat(c,180,310,100,18); addPlat(c,390,285,100,18);
        addPlat(c,580,305,100,18);
        addPlat(c, 80,200, 90,18); addPlat(c,330,175, 90,18); addPlat(c,560,195, 90,18);
        addSpawn(c, T::botom,140,410,enemy_variant::Blue);
        addSpawn(c, T::tornado,340,380,enemy_variant::Blue);
        addSpawn(c, T::flyingfoogafoog,530,370,enemy_variant::Purple);
        addSpawn(c, T::tornado,710,410,enemy_variant::Blue);
        addSpawn(c, T::flyingfoogafoog,230,280,enemy_variant::Purple);
        addSpawn(c, T::tornado,440,255,enemy_variant::Purple);
        addSpawn(c, T::tornado,620,275,enemy_variant::Purple);
        addSpawn(c, T::flyingfoogafoog,370,145,enemy_variant::Purple);
    }
    // Level 9
    {
        level_config& c = s_levels[8];
        c.level_number = 9; c.is_boss_level = false;
        c.bg_color = sf::Color(10,10,30);
        strCopy(c.title, "Storm Clouds");
        addPlat(c,  0,560,800,40);
        addPlat(c, 60,470,100,18); addPlat(c,220,440,100,18);
        addPlat(c,380,410,100,18); addPlat(c,540,440,100,18); addPlat(c,680,460,100,18);
        addPlat(c,140,330, 90,18); addPlat(c,310,300, 90,18);
        addPlat(c,470,290, 90,18); addPlat(c,620,330, 90,18);
        c.is_collectable_level = true;
    }
    // Level 10 badmash: Gamakichi    
    {
        level_config& c = s_levels[9];
        c.level_number = 10; c.is_boss_level = true;
        c.bg_color = sf::Color(10,0,0);
        strCopy(c.title, "Final Arena");
        addPlat(c,  0,560,800,40);
        addPlat(c,  0,380,180,18); addPlat(c,620,380,180,18);
        addPlat(c,300,290,200,18);
        addPlat(c, 80,200,130,18); addPlat(c,590,200,130,18);
        // Gamakichi is place centre bottom
        addSpawn(c, T::gamakichi, 340, 480);
    }
}

const level_config& level_registry::get(int n) {
    return s_levels[n - 1];
}

int level_registry::count() {
    return s_count;
}
