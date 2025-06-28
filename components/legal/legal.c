#include "legal.h"
#include "legal_templates.h"
#include "esp_log.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

static const char *TAG = "legal";

static const char *cerfa_form_path = "forms/cerfa_base.pdf";
static const char *cites_form_path = "forms/cites_base.pdf";

static const char *valid_cdc_numbers[] = { "CDC12345", "CDC67890", NULL };
static const char *valid_aoe_numbers[] = { "AOE12345", "AOE67890", NULL };

static bool write_basic_pdf(const char *path, const char *content)
{
    if (!path || !content)
        return false;
    FILE *f = fopen(path, "w");
    if (!f) {
        ESP_LOGE(TAG, "Impossible d'ouvrir %s", path);
        return false;
    }

    int off1, off2, off3, off4;
    fprintf(f, "%%PDF-1.4\n");
    off1 = ftell(f);
    fprintf(f, "1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n");
    off2 = ftell(f);
    fprintf(f, "2 0 obj\n<< /Type /Pages /Kids [3 0 R] /Count 1 >>\nendobj\n");
    off3 = ftell(f);
    fprintf(f, "3 0 obj\n<< /Type /Page /Parent 2 0 R /MediaBox [0 0 595 842] /Contents 4 0 R >>\nendobj\n");
    off4 = ftell(f);
    fprintf(f, "4 0 obj\n<< /Length %zu >>\nstream\n%s\nendstream\nendobj\n", strlen(content), content);
    int xref = ftell(f);
    fprintf(f, "xref\n0 5\n");
    fprintf(f, "0000000000 65535 f \n");
    fprintf(f, "%010d 00000 n \n", off1);
    fprintf(f, "%010d 00000 n \n", off2);
    fprintf(f, "%010d 00000 n \n", off3);
    fprintf(f, "%010d 00000 n \n", off4);
    fprintf(f, "trailer\n<< /Size 5 /Root 1 0 R >>\nstartxref\n%d\n%%EOF\n", xref);
    fclose(f);
    ESP_LOGI(TAG, "PDF genere: %s", path);
    return true;
}

static bool write_overlay_pdf(const char *template_path, const char *path, const char *content)
{
    if (!template_path || !path || !content)
        return false;
    FILE *tpl = fopen(template_path, "rb");
    if (!tpl) {
        ESP_LOGE(TAG, "Template introuvable: %s", template_path);
        return false;
    }
    FILE *out = fopen(path, "wb");
    if (!out) {
        ESP_LOGE(TAG, "Impossible d'ouvrir %s", path);
        fclose(tpl);
        return false;
    }
    char buf[256];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), tpl)) > 0)
        fwrite(buf, 1, n, out);
    fclose(tpl);

    fprintf(out, "\n%% overlay\n%s\n", content);
    fclose(out);
    ESP_LOGI(TAG, "PDF genere: %s", path);
    return true;
}

bool legal_generate_cerfa(const char *path, const Reptile *r)
{
    if (!r)
        return false;
    char buffer[256];
    snprintf(buffer, sizeof(buffer), cerfa_template, r->name, r->species, r->sex, r->birth_date);
    return write_basic_pdf(path, buffer);
}

bool legal_generate_ifap(const char *path, const Reptile *r)
{
    if (!r)
        return false;
    char buffer[256];
    snprintf(buffer, sizeof(buffer), ifap_template, r->name, r->species, r->id);
    return write_basic_pdf(path, buffer);
}

bool legal_generate_cites(const char *path, const Reptile *r)
{
    if (!r)
        return false;
    char buffer[256];
    snprintf(buffer, sizeof(buffer), cites_template, r->name, r->species);
    return write_basic_pdf(path, buffer);
}

bool legal_generate_invoice(const char *path, const Reptile *r)
{
    if (!r)
        return false;
    char buffer[256];
    snprintf(buffer, sizeof(buffer), invoice_template, r->name, r->species);
    return write_basic_pdf(path, buffer);
}

bool legal_generate_cerfa_official(const char *path, const Reptile *r)
{
    if (!r)
        return false;
    char buffer[512];
    snprintf(buffer, sizeof(buffer), cerfa_official_template,
             r->name, r->species, r->sex, r->birth_date,
             r->cdc_number, r->aoe_number, r->ifap_id,
             r->quota_used, r->quota_limit);
    return write_overlay_pdf(cerfa_form_path, path, buffer);
}

bool legal_generate_cites_official(const char *path, const Reptile *r)
{
    if (!r)
        return false;
    char buffer[256];
    snprintf(buffer, sizeof(buffer), cites_official_template,
             r->name, r->species, r->ifap_id);
    return write_overlay_pdf(cites_form_path, path, buffer);
}

bool legal_is_cerfa_valid(const Reptile *r)
{
    if (!r)
        return false;
    return r->cerfa_valid_until > (int)time(NULL);
}

static bool number_in_list(const char *num, const char *const list[])
{
    if (!num || !list)
        return false;
    for (int i = 0; list[i]; ++i) {
        if (strcmp(num, list[i]) == 0)
            return true;
    }
    return false;
}

bool legal_is_cdc_valid(const Reptile *r)
{
    return r && number_in_list(r->cdc_number, valid_cdc_numbers);
}

bool legal_is_aoe_valid(const Reptile *r)
{
    return r && number_in_list(r->aoe_number, valid_aoe_numbers);
}

bool legal_is_cites_valid(const Reptile *r)
{
    if (!r)
        return false;
    return r->cites_valid_until > (int)time(NULL);
}

bool legal_quota_remaining(const Reptile *r)
{
    if (!r)
        return false;
    return r->quota_used < r->quota_limit;
}

void legal_check_documents(void)
{
    for (int i = 0; i < animals_count(); ++i) {
        const Reptile *r = animals_get_by_index(i);
        if (!r)
            continue;
        time_t now = time(NULL);
        if (!legal_is_cdc_valid(r)) {
            ESP_LOGW(TAG, "CDC invalide pour %s", r->name);
            ui_notify("CDC invalide");
        }
        if (!legal_is_aoe_valid(r)) {
            ESP_LOGW(TAG, "AOE invalide pour %s", r->name);
            ui_notify("AOE invalide");
        }
        if (!legal_is_cerfa_valid(r) || !legal_is_cites_valid(r)) {
            ESP_LOGW(TAG, "Documents expir\xC3\xA9s pour %s", r->name);
            ui_notify("Documents expir\xC3\xA9s");
        } else {
            if (r->cerfa_valid_until - now < 30 * 86400 ||
                r->cites_valid_until - now < 30 * 86400) {
                ESP_LOGI(TAG, "Documents bientot expir\xC3\xA9s pour %s", r->name);
                ui_notify("Documents bientot expir\xC3\xA9s");
            }
        }
        if (!legal_quota_remaining(r)) {
            ESP_LOGW(TAG, "Quota atteint pour %s", r->name);
            ui_notify("Quota atteint");
        } else if (r->quota_limit - r->quota_used <= 1) {
            ESP_LOGI(TAG, "Quota presque atteint pour %s", r->name);
            ui_notify("Quota presque atteint");
        }
    }
}

void legal_generate_all(const char *dir, const Reptile *r)
{
    if (!dir || !r)
        return;
    char path[128];
    snprintf(path, sizeof(path), "%s/cerfa.pdf", dir);
    legal_generate_cerfa(path, r);

    snprintf(path, sizeof(path), "%s/cerfa_officiel.pdf", dir);
    legal_generate_cerfa_official(path, r);

    snprintf(path, sizeof(path), "%s/ifap.pdf", dir);
    legal_generate_ifap(path, r);

    snprintf(path, sizeof(path), "%s/cites.pdf", dir);
    legal_generate_cites(path, r);

    snprintf(path, sizeof(path), "%s/cites_officiel.pdf", dir);
    legal_generate_cites_official(path, r);

    snprintf(path, sizeof(path), "%s/invoice.pdf", dir);
    legal_generate_invoice(path, r);
}
