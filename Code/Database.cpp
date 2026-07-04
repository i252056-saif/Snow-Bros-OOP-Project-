#include "Database.h"
#include "StrUtil.h"
#include <cstdlib>
#include <ctime>
#include<fstream>

#include <direct.h>   // _mkdir — Windows only

database::database() {
    strCopy(m_data_dir, "./data/");
    ensure_data_dir();
}

void database::ensure_data_dir() const {
    _mkdir(m_data_dir);
}

//User helpers
int database::load_users(user_account* out, int max) const {
    char path[MAX_STR * 2];
    strCopy(path, m_data_dir);
    strAppend(path, "users.csv");
    std::ifstream f(path);
    if (!f.is_open()) return 0;
    int count = 0;
    char line[256];
    while (f.getline(line, 256) && count < max) {
        if (line[0] == '#' || line[0] == '\0') continue;
        // parse: username,password,email
        user_account& u = out[count];
        int i = 0, field = 0, fi = 0;
        while (line[i] && field < 3) {
            if (line[i] == ',') {
                if (field == 0) u.username[fi] = '\0';
                else if (field == 1) u.password[fi] = '\0';
                else if (field == 2) u.email[fi] = '\0';
                ++field; fi = 0;
            }
            else {
                if (field == 0 && fi < MAX_STR - 1)
                    u.username[fi++] = line[i];
                else if (field == 1 && fi < MAX_STR - 1)
                    u.password[fi++] = line[i];
                else if (field == 2 && fi < MAX_STR - 1)
                    u.email[fi++] = line[i];
            }
            ++i;
        }
        // terminate last field
        if (field == 0)
            u.username[fi] = '\0';
        else if (field == 1)
            u.password[fi] = '\0';
        else
            u.email[fi] = '\0';
        ++count;
    }
    return count;
}

void database::save_users(user_account* arr, int count) const {
    char path[MAX_STR * 2];
    strCopy(path, m_data_dir); strAppend(path, "users.csv");
    std::ofstream f(path);
    f << "# username,password,email\n";
    for (int i = 0; i < count; ++i)
        f << arr[i].username << "," << arr[i].password << "," << arr[i].email << "\n";
}

bool database::user_exists(const char* username) const {
    user_account arr[64];
    int n = load_users(arr, 64);
    for (int i = 0; i < n; ++i)
        if (strEq(arr[i].username, username)) return true;
    return false;
}

bool database::register_user(const char* username, const char* password, const char* email) {
    if (user_exists(username)) return false;
    user_account arr[64];
    int n = load_users(arr, 64);
    if (n >= 64) return false;
    strCopy(arr[n].username, username);
    strCopy(arr[n].password, password);          // store plain-text password
    strCopy(arr[n].email, email ? email : "");
    save_users(arr, n + 1);
    // Create blank progress record
    player_progress p;
    strCopy(p.username, username);
    save_progress(p);
    return true;
}

bool database::login_user(const char* username, const char* password) {
    user_account arr[64];
    int n = load_users(arr, 64);
    for (int i = 0; i < n; ++i)
        if (strEq(arr[i].username, username) && strEq(arr[i].password, password))
            return true;
    return false;
}

// ---- Progress helpers ----
int database::load_progress(player_progress* out, int max) const {
    char path[MAX_STR * 2];
    strCopy(path, m_data_dir);
    strAppend(path, "progress.csv");
    std::ifstream f(path);
    if (!f.is_open()) return 0;
    int count = 0;
    char line[256];
    while (f.getline(line, 256) && count < max) {
        if (line[0] == '#' || line[0] == '\0') continue;
        player_progress& p = out[count];
        char fields[5][MAX_STR];
        int fi = 0, ci = 0, i = 0;
        while (line[i] && fi < 5) {
            if (line[i] == ',')
            {
                fields[fi][ci] = '\0';
                ++fi;
                ci = 0;
            }
            else if (ci < MAX_STR - 1)
                fields[fi][ci++] = line[i];
            ++i;
        }
        fields[fi][ci] = '\0';
        strCopy(p.username, fields[0]);
        p.current_level = strToInt(fields[1]);
        p.lives_remaining = strToInt(fields[2]);
        p.gem_count = strToInt(fields[3]);
        p.high_score = strToInt(fields[4]);
        ++count;
    }
    return count;
}

player_progress database::load_progress(const char* username) const {
    player_progress arr[64];
    int n = load_progress(arr, 64);
    for (int i = 0; i < n; ++i)
        if (strEq(arr[i].username, username)) return arr[i];
    player_progress blank;
    strCopy(blank.username, username);
    return blank;
}

void database::save_all_progress(player_progress* arr, int count) const {
    char path[MAX_STR * 2];
    strCopy(path, m_data_dir); strAppend(path, "progress.csv");
    std::ofstream f(path);
    f << "# username,currentLevel,lives,gems,highScore\n";
    for (int i = 0; i < count; ++i)
        f << arr[i].username << ","
        << arr[i].current_level << ","
        << arr[i].lives_remaining << ","
        << arr[i].gem_count << ","
        << arr[i].high_score << "\n";
}

void database::save_progress(const player_progress& progress) {
    player_progress arr[64];
    int n = load_progress(arr, 64);
    bool found = false;
    for (int i = 0; i < n; ++i) {
        if (strEq(arr[i].username, progress.username)) {
            arr[i] = progress; found = true; break;
        }
    }
    if (!found && n < 64) arr[n++] = progress;
    save_all_progress(arr, n);
}

// ---- Leaderboard ----
int database::load_leaderboard(leaderboard_entry* out, int max) const {
    char path[MAX_STR * 2];
    strCopy(path, m_data_dir); strAppend(path, "leaderboard.csv");
    std::ifstream f(path);
    if (!f.is_open()) return 0;
    int count = 0;
    char line[256];
    while (f.getline(line, 256) && count < max) {
        if (line[0] == '#' || line[0] == '\0') continue;
        leaderboard_entry& e = out[count];
        char fields[4][MAX_STR];
        int fi = 0, ci = 0, i = 0;
        while (line[i] && fi < 4) {
            if (line[i] == ',')
            {
                fields[fi][ci] = '\0'; ++fi; ci = 0;
            }
            else if (ci < MAX_STR - 1)
                fields[fi][ci++] = line[i];
            ++i;
        }
        fields[fi][ci] = '\0';
        strCopy(e.username, fields[0]);
        e.score = strToInt(fields[1]);
        e.level_reached = strToInt(fields[2]);
        strCopy(e.date, fields[3]);
        ++count;
    }
    return count;
}

void database::save_leaderboard(leaderboard_entry* arr, int count) const {
    char path[MAX_STR * 2];
    strCopy(path, m_data_dir); strAppend(path, "leaderboard.csv");
    std::ofstream f(path);
    f << "# username,score,levelReached,date\n";
    for (int i = 0; i < count; ++i)
        f << arr[i].username << ","
        << arr[i].score << ","
        << arr[i].level_reached << ","
        << arr[i].date << "\n";
}

int database::get_top_scores(leaderboard_entry* out, int maxCount) const {
    leaderboard_entry arr[256];
    int n = load_leaderboard(arr, 256);
    // Simple insertion sort descending by score
    for (int i = 1; i < n; ++i) {
        leaderboard_entry key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].score < key.score) {
            arr[j + 1] = arr[j]; --j;
        }
        arr[j + 1] = key;
    }
    int ret = (n < maxCount) ? n : maxCount;
    for (int i = 0; i < ret; ++i) out[i] = arr[i];
    return ret;
}

void database::submit_score(const char* username, int score, int level_reached) {
    leaderboard_entry arr[256];
    int n = load_leaderboard(arr, 256);
    if (n < 256) {
        strCopy(arr[n].username, username);
        arr[n].score = score;
        arr[n].level_reached = level_reached;
        currentDate(arr[n].date);
        ++n;
    }
    save_leaderboard(arr, n);
}

void database::currentDate(char* out) const {
    // Delegate to the free function in StrUtil.h
    ::currentDate(out, MAX_STR);
}