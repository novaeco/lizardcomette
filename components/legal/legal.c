#include "legal.h"
#include "legal_templates.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "legal";

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

void legal_generate_all(const char *dir, const Reptile *r)
{
    if (!dir || !r)
        return;
    char path[128];
    snprintf(path, sizeof(path), "%s/cerfa.pdf", dir);
    legal_generate_cerfa(path, r);

    snprintf(path, sizeof(path), "%s/ifap.pdf", dir);
    legal_generate_ifap(path, r);

    snprintf(path, sizeof(path), "%s/cites.pdf", dir);
    legal_generate_cites(path, r);

    snprintf(path, sizeof(path), "%s/invoice.pdf", dir);
    legal_generate_invoice(path, r);
}
