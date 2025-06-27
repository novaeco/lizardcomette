#include "storage.h"
#include "esp_log.h"
#include "mbedtls/aes.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const char *TAG = "storage";
static const unsigned char aes_key[16] = "lizard-secret!!";

bool storage_encrypt_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f)
        return false;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    long padded_len = (len + 15) & ~15;
    unsigned char *buf = calloc(1, padded_len);
    if (!buf) {
        fclose(f);
        return false;
    }

    fread(buf, 1, len, f);
    fclose(f);

    mbedtls_aes_context ctx;
    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, aes_key, 128);
    unsigned char iv[16] = {0};
    mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, padded_len, iv, buf, buf);
    mbedtls_aes_free(&ctx);

    f = fopen(path, "wb");
    if (!f) {
        free(buf);
        return false;
    }

    fwrite(buf, 1, padded_len, f);
    fclose(f);
    free(buf);
    return true;
}

void storage_init(void)
{
    ESP_LOGI(TAG, "Initialisation du stockage externe");
    // TODO: monter la carte SD ou SPIFFS
}

void storage_export(void)
{
    ESP_LOGI(TAG, "Export des données");
    // TODO: exporter les données en CSV/JSON
    const char *path = "/spiffs/export.enc";
    FILE *f = fopen(path, "w");
    if (f) {
        fputs("placeholder", f);
        fclose(f);
        storage_encrypt_file(path);
    }
}
