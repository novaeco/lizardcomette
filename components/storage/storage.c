#include "storage.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "mbedtls/aes.h"
#include "db.h"
#include "legal.h"
#include "esp_http_client.h"
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

    uint32_t orig_len = (uint32_t)len;
    fwrite(&orig_len, 1, sizeof(orig_len), f);
    fwrite(buf, 1, padded_len, f);
    fclose(f);
    free(buf);
    return true;
}

bool storage_decrypt_file(const char *src, const char *dst)
{
    FILE *f = fopen(src, "rb");
    if (!f)
        return false;

    uint32_t orig_len;
    if (fread(&orig_len, 1, sizeof(orig_len), f) != sizeof(orig_len)) {
        fclose(f);
        return false;
    }

    fseek(f, 0, SEEK_END);
    long file_len = ftell(f);
    long enc_len = file_len - sizeof(orig_len);
    fseek(f, sizeof(orig_len), SEEK_SET);

    unsigned char *buf = malloc(enc_len);
    if (!buf) {
        fclose(f);
        return false;
    }

    fread(buf, 1, enc_len, f);
    fclose(f);

    mbedtls_aes_context ctx;
    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_dec(&ctx, aes_key, 128);
    unsigned char iv[16] = {0};
    mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, enc_len, iv, buf, buf);
    mbedtls_aes_free(&ctx);

    f = fopen(dst, "wb");
    if (!f) {
        free(buf);
        return false;
    }

    fwrite(buf, 1, orig_len, f);
    fclose(f);
    free(buf);
    return true;
}

bool storage_mount_sdcard(void)
{
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t *card;
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    const esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host,
                                                 &slot_config,
                                                 &mount_config, &card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erreur montage SD: %s", esp_err_to_name(ret));
        return false;
    }
    ESP_LOGI(TAG, "Carte SD montee");
    return true;
}

void storage_init(void)
{
    ESP_LOGI(TAG, "Initialisation du stockage externe");
    storage_mount_sdcard();
}


void storage_export_csv(const char *path)
{
    ESP_LOGI(TAG, "Export CSV vers %s", path);
    db_export_csv(path);
    storage_encrypt_file(path);
}

void storage_export_pdf(const char *dir, const Reptile *r)
{
    ESP_LOGI(TAG, "Generation des PDF dans %s", dir);
    legal_generate_all(dir, r);
}

bool storage_wifi_transfer(const char *path, const char *url)
{
    ESP_LOGI(TAG, "Transfert Wi-Fi de %s vers %s", path, url);
    FILE *f = fopen(path, "rb");
    if (!f) {
        ESP_LOGE(TAG, "Fichier %s introuvable", path);
        return false;
    }
    fclose(f);
    // Simulation d'envoi HTTP; a implementer reellement avec esp_http_client
    ESP_LOGI(TAG, "Transfert termine");
    return true;
}

void storage_restore(void)
{
    ESP_LOGI(TAG, "Restauration de la base de donnees");
    if (!storage_decrypt_file("/sdcard/lizard_backup.db", "/spiffs/lizard.db")) {
        ESP_LOGE(TAG, "Echec restauration");
    }
}
