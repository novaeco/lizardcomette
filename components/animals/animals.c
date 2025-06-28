#include "animals.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "animals";

static Reptile animals[ANIMALS_MAX];
static int animal_count = 0;

void animals_init(void)
{
    animal_count = 0;
    memset(animals, 0, sizeof(animals));
    ESP_LOGI(TAG, "Initialisation des donnees animaux");
}

bool animals_add(const Reptile *r)
{
    if (animal_count >= ANIMALS_MAX || !r)
        return false;
    animals[animal_count] = *r;
    animal_count++;
    ESP_LOGI(TAG, "Ajout du reptile %d", r->id);
    return true;
}

static int find_index(int id)
{
    for (int i = 0; i < animal_count; ++i) {
        if (animals[i].id == id)
            return i;
    }
    return -1;
}

const Reptile *animals_get(int id)
{
    int idx = find_index(id);
    if (idx >= 0)
        return &animals[idx];
    return NULL;
}

bool animals_update(int id, const Reptile *r)
{
    int idx = find_index(id);
    if (idx < 0 || !r)
        return false;
    animals[idx] = *r;
    ESP_LOGI(TAG, "Mise a jour du reptile %d", id);
    return true;
}

bool animals_delete(int id)
{
    int idx = find_index(id);
    if (idx < 0)
        return false;
    for (int i = idx; i < animal_count - 1; ++i) {
        animals[i] = animals[i + 1];
    }
    animal_count--;
    ESP_LOGI(TAG, "Suppression du reptile %d", id);
    return true;
}

int animals_count(void)
{
    return animal_count;
}

const Reptile *animals_get_by_index(int index)
{
    if (index < 0 || index >= animal_count)
        return NULL;
    return &animals[index];
}

int animals_count_for_elevage(int elevage_id)
{
    int count = 0;
    for (int i = 0; i < animal_count; ++i) {
        if (animals[i].elevage_id == elevage_id)
            count++;
    }
    return count;
}

const Reptile *animals_get_by_index_for_elevage(int index, int elevage_id)
{
    int current = 0;
    for (int i = 0; i < animal_count; ++i) {
        if (animals[i].elevage_id == elevage_id) {
            if (current == index)
                return &animals[i];
            current++;
        }
    }
    return NULL;
}
