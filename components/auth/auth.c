#include "auth.h"
#include "esp_log.h"
#include <string.h>
#include "mbedtls/sha256.h"
#include "db.h"
#include <sqlite3.h>

static void sha256_hash(const char *input, unsigned char output[32])
{
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts_ret(&ctx, 0);
    mbedtls_sha256_update_ret(&ctx, (const unsigned char *)input, strlen(input));
    mbedtls_sha256_finish_ret(&ctx, output);
    mbedtls_sha256_free(&ctx);
}

static void hash_to_hex(const unsigned char hash[32], char out[65])
{
    static const char hexchars[] = "0123456789abcdef";
    for (int i = 0; i < 32; ++i) {
        out[i * 2] = hexchars[(hash[i] >> 4) & 0xF];
        out[i * 2 + 1] = hexchars[hash[i] & 0xF];
    }
    out[64] = '\0';
}

static bool hex_to_hash(const char *hex, unsigned char out[32])
{
    if (!hex || strlen(hex) != 64)
        return false;
    for (int i = 0; i < 32; ++i) {
        int hi, lo;
        char c1 = hex[i * 2];
        char c2 = hex[i * 2 + 1];
        if (c1 >= '0' && c1 <= '9') hi = c1 - '0';
        else if (c1 >= 'a' && c1 <= 'f') hi = c1 - 'a' + 10;
        else if (c1 >= 'A' && c1 <= 'F') hi = c1 - 'A' + 10;
        else return false;
        if (c2 >= '0' && c2 <= '9') lo = c2 - '0';
        else if (c2 >= 'a' && c2 <= 'f') lo = c2 - 'a' + 10;
        else if (c2 >= 'A' && c2 <= 'F') lo = c2 - 'A' + 10;
        else return false;
        out[i] = (hi << 4) | lo;
    }
    return true;
}

static const char *TAG = "auth";
static auth_user_t users[AUTH_MAX_USERS];
static int user_count = 0;

void auth_init(void)
{
    ESP_LOGI(TAG, "Initialisation du module d'authentification");
    user_count = 0;
    memset(users, 0, sizeof(users));

    sqlite3_stmt *stmt = db_query("SELECT username,hash,role FROM users;");
    if (stmt) {
        while (sqlite3_step(stmt) == SQLITE_ROW && user_count < AUTH_MAX_USERS) {
            const char *uname = (const char *)sqlite3_column_text(stmt, 0);
            const char *hash_hex = (const char *)sqlite3_column_text(stmt, 1);
            int role = sqlite3_column_int(stmt, 2);
            if (uname && hash_hex) {
                strncpy(users[user_count].username, uname,
                        sizeof(users[user_count].username) - 1);
                hex_to_hash(hash_hex, users[user_count].hash);
                users[user_count].role = role;
                users[user_count].elevage_count = 0;
                user_count++;
            }
        }
        sqlite3_finalize(stmt);
    }

    stmt = db_query("SELECT username,elevage_id FROM user_elevages;");
    if (stmt) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *uname = (const char *)sqlite3_column_text(stmt, 0);
            int elev_id = sqlite3_column_int(stmt, 1);
            if (uname) {
                for (int i = 0; i < user_count; ++i) {
                    if (strcmp(users[i].username, uname) == 0) {
                        if (users[i].elevage_count < AUTH_MAX_ELEVAGES_PER_USER) {
                            users[i].elevages[users[i].elevage_count++] = elev_id;
                        }
                        break;
                    }
                }
            }
        }
        sqlite3_finalize(stmt);
    }

    if (user_count == 0) {
        auth_add_user("admin", "changeme", ROLE_PARTICULIER);
        auth_link_elevage("admin", 0);
    }
}

bool auth_add_user(const char *username, const char *password, user_role_t role)
{
    if (user_count >= AUTH_MAX_USERS || !username || !password)
        return false;
    strncpy(users[user_count].username, username, sizeof(users[user_count].username) - 1);
    users[user_count].username[sizeof(users[user_count].username) - 1] = '\0';
    sha256_hash(password, users[user_count].hash);
    users[user_count].role = role;
    users[user_count].elevage_count = 0;

    char hex[65];
    hash_to_hex(users[user_count].hash, hex);
    sqlite3_stmt *stmt =
        db_query("INSERT INTO users(username,hash,role) VALUES(?,?,?);");
    if (!stmt)
        return false;
    sqlite3_bind_text(stmt, 1, users[user_count].username, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, hex, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, role);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    if (!ok)
        return false;

    user_count++;
    return true;
}

bool auth_check(const char *username, const char *password)
{
    unsigned char hash[32];
    sha256_hash(password, hash);
    for (int i = 0; i < user_count; ++i) {
        if (strcmp(users[i].username, username) == 0 &&
            memcmp(users[i].hash, hash, sizeof(hash)) == 0)
            return true;
    }
    return false;
}

user_role_t auth_get_role(const char *username)
{
    for (int i = 0; i < user_count; ++i) {
        if (strcmp(users[i].username, username) == 0)
            return users[i].role;
    }
    return ROLE_PARTICULIER;
}

bool auth_link_elevage(const char *username, int elevage_id)
{
    for (int i = 0; i < user_count; ++i) {
        if (strcmp(users[i].username, username) == 0) {
            if (users[i].elevage_count >= AUTH_MAX_ELEVAGES_PER_USER)
                return false;
            sqlite3_stmt *stmt =
                db_query("INSERT INTO user_elevages(username,elevage_id) VALUES(?,?);");
            if (!stmt)
                return false;
            sqlite3_bind_text(stmt, 1, username, -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 2, elevage_id);
            bool ok = sqlite3_step(stmt) == SQLITE_DONE;
            sqlite3_finalize(stmt);
            if (!ok)
                return false;
            users[i].elevages[users[i].elevage_count++] = elevage_id;
            return true;
        }
    }
    return false;
}

int auth_get_elevage_count(const char *username)
{
    for (int i = 0; i < user_count; ++i) {
        if (strcmp(users[i].username, username) == 0)
            return users[i].elevage_count;
    }
    return 0;
}

int auth_get_elevage_by_index(const char *username, int index)
{
    for (int i = 0; i < user_count; ++i) {
        if (strcmp(users[i].username, username) == 0) {
            if (index >= 0 && index < users[i].elevage_count)
                return users[i].elevages[index];
        }
    }
    return -1;
}
