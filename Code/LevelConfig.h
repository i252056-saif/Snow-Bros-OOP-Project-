#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"
#include "Enemy.h"

struct enemy_spawn {
    enum class Type { botom, flyingfoogafoog, tornado, mogera, gamakichi };
    Type         type    = Type::botom;
    float        x       = 0.f;
    float        y       = 0.f;
    enemy_variant variant = enemy_variant::Red;
};

struct platform_def {
    float x, y, w, h;
};

/*
 struct level_config
  All data required for one level. data no logic.
  Completely data adding level 11-20 needs no engine changes.
 */
struct level_config {
    int         level_number  = 1;
    bool        is_boss_level  = false;
    bool        is_collectable_level = false; //bonus lvl
    sf::Color   bg_color      = sf::Color(20, 20, 60);
    char        title[MAX_STR] = "Level";

    enemy_spawn  spawns[MAX_ENEMIES];
    int         spawn_count   = 0;

    platform_def platforms[MAX_PLATFORMS];
    int         platformCount = 0;
};

/*
class level_registry
 Central read only registry of all level configs.
 No hardcoded level count  driven purely by the array size.
 */
class level_registry {
public:
    static void init();  
    static const level_config& get(int level_number);  // 1 based
    static int count();

private:
    static level_config s_levels[10];
    static int  s_count;
    static bool  s_initialized;
};
