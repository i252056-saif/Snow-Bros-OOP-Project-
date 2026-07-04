#pragma once

// Window
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int TARGET_FPS = 60;

// Physics
const float GRAVITY = 800.f;
const float JUMP_VELOCITY = -420.f;
const float PLAYER_SPEED = 200.f;
const float SNOWBALL_SPEED = 380.f;
const float ROLL_SPEED = 280.f;
const float ENCASE_TIMEOUT = 6.0f;

// Player
const int PLAYER_START_LIVES  = 10;
const float PLAYER_WIDTH = 32.f;
const float PLAYER_HEIGHT = 40.f;
const float INVINCIBLE_DURATION = 2.0f;

// enemy speeds
const float BOTOM_SPEED = 80.f;
const float FOOGA_SPEED = 100.f;
const float TORNADO_SPEED = 130.f;

// Score ranges
const int BOTOM_SCORE_MIN = 100;
const int BOTOM_SCORE_MAX = 500;
const int FOOGA_SCORE_MIN = 200;
const int FOOGA_SCORE_MAX = 800;
const int TORNADO_SCORE_MIN = 300;
const int TORNADO_SCORE_MAX = 1200;
const int MOGERA_SCORE = 5000;
const int GAMAKICHI_SCORE = 10000;
const float CHAIN_BONUS_PCT = 0.10f;

// Gems
const int MOGERA_GEM_REWARD  = 200;
const int GAMAKICHI_GEM_REWARD = 500;

// Powerup
const float SPEED_BOOST_MULT = 1.5f;
const float SPEED_BOOST_TIME  = 15.f;
const float BALLOON_TIME = 10.f;
const float BALLOON_FLOAT_VEL = -60.f;

// Shop prices
const int SHOP_EXTRA_LIFE = 50;
const int SHOP_SPEED_BOOST = 20;
const int SHOP_SNOWBALL_POWER = 30;
const int SHOP_DISTANCE = 25;
const int SHOP_BALLOON = 35;

// Array size 
const int MAX_ENEMIES = 32;
const int MAX_PROJECTILES = 64;
const int MAX_POWERUPS = 16;
const int MAX_PLATFORMS   = 20;
const int MAX_PLAYERS = 2;
const int MAX_LEADERBOARD = 10;
const int MAX_STR = 64;   // max characters for string
