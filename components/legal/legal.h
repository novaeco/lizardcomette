#ifndef LEGAL_H
#define LEGAL_H

#include <stdbool.h>
#include "animals.h"

bool legal_generate_cerfa(const char *path, const Reptile *r);
bool legal_generate_ifap(const char *path, const Reptile *r);
bool legal_generate_cites(const char *path, const Reptile *r);
bool legal_generate_invoice(const char *path, const Reptile *r);
bool legal_generate_cerfa_official(const char *path, const Reptile *r);
bool legal_generate_cites_official(const char *path, const Reptile *r);

bool legal_is_cerfa_valid(const Reptile *r);
bool legal_is_cites_valid(const Reptile *r);
bool legal_quota_remaining(const Reptile *r);
bool legal_is_cdc_valid(const Reptile *r);
bool legal_is_aoe_valid(const Reptile *r);

void legal_check_documents(void);

void legal_generate_all(const char *dir, const Reptile *r);

#endif // LEGAL_H
