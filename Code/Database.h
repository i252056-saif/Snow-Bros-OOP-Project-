#pragma once
#include <iostream>
#include "Constants.h"

struct leaderboard_entry {
    char username[MAX_STR];
    int  score = 0;
    int  level_reached = 1;
    char date[MAX_STR];
};

struct player_progress {
    char username[MAX_STR];
    int  current_level = 1;
    int  lives_remaining = PLAYER_START_LIVES;
    int  gem_count = 0;
    int  high_score = 0;
};

struct user_account {
    char username[MAX_STR];
    char password[MAX_STR];
    char email[MAX_STR];
};

/*
class database
 File based.
 Passwords stored text.
 File I/O std::fstream .
 */
class database {
public:
    database();

    bool register_user(const char* username, const char* password,
                       const char* email = "");
    bool login_user (const char* username, const char* password);
    bool user_exists  (const char* username) const;

    player_progress load_progress(const char* username) const;
    void save_progress(const player_progress& progress);

    int  get_top_scores(leaderboard_entry* out, int maxCount) const;
    void submit_score (const char* username, int score, int level_reached);

private:
    char m_data_dir[MAX_STR];

    int  load_users (user_account* out, int max) const;
    int  load_progress(player_progress* out, int max) const;
    int  load_leaderboard (leaderboard_entry* out, int max) const;

    void save_users (user_account* arr, int count) const;
    void save_all_progress (player_progress*  arr, int count) const;
    void save_leaderboard (leaderboard_entry* arr, int count) const;

    void currentDate(char* out) const;
    void ensure_data_dir() const;
};
