#include "auth.h"
#include "esp_log.h"
#include <string.h>
#include "mbedtls/sha256.h"

static void sha256_hash(const char *input, unsigned char output[32])
{
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts_ret(&ctx, 0);
    mbedtls_sha256_update_ret(&ctx, (const unsigned char *)input, strlen(input));
    mbedtls_sha256_finish_ret(&ctx, output);
    mbedtls_sha256_free(&ctx);
}

static const char *TAG = "auth";
static auth_user_t users[AUTH_MAX_USERS];
static int user_count = 0;

void auth_init(void)
{
    ESP_LOGI(TAG, "Initialisation du module d'authentification");
    user_count = 0;
    auth_add_user("admin", "changeme", ROLE_PARTICULIER);
    auth_link_elevage("admin", 0);
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
