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
static unsigned char stored_hash[32];
static user_role_t current_role = ROLE_PARTICULIER;

void auth_init(void)
{
    ESP_LOGI(TAG, "Initialisation du module d'authentification");
    auth_set_credentials("changeme", ROLE_PARTICULIER);
}

void auth_set_credentials(const char *password, user_role_t role)
{
    sha256_hash(password, stored_hash);
    current_role = role;
}

bool auth_check(const char *password)
{
    unsigned char hash[32];
    sha256_hash(password, hash);
    return memcmp(hash, stored_hash, sizeof(hash)) == 0;
}

user_role_t auth_get_role(void)
{
    return current_role;
}
