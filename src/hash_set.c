#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "libhash.h"
#include "prime.h"
#include "strdup/strdup.h"

static const int H_DEFAULT_CAPACITY = 50;

/**
 * Resize the hash table. This implementation has a set capacity;
 * hash collisions rise beyond the capacity and `h_instert` will fail.
 * To mitigate this, we resize up if the load (measured as the ratio of keys
 * count to capacity) is less than .1, or down if the load exceeds .7. To
 * resize, we create a new table approx. 1/2x or 2x times the current table
 * size, then insert into it all non-deleted keys.
 *
 * @param ht
 * @param base_capacity
 * @return int
 */
static void hs_resize(hash_set *ht, const int base_capacity) {
  if (base_capacity < 0) {
    return;
  }

  hash_set *new_ht = hs_init(base_capacity);

  for (int i = 0; i < ht->capacity; i++) {
    const char *r = ht->keys[i];

    if (r != NULL) {
      hs_insert(new_ht, r);
    }
  }

  ht->base_capacity = new_ht->base_capacity;
  ht->count = new_ht->count;

  const int tmp_capacity = ht->capacity;

  ht->capacity = new_ht->capacity;
  new_ht->capacity = tmp_capacity;

  char **tmp_keys = ht->keys;
  ht->keys = new_ht->keys;
  new_ht->keys = tmp_keys;

  hs_delete_set(new_ht);
}

/**
 * Resize the table to a larger size, the first prime subsequent
 * to approx. 2x the base capacity.
 *
 * @param ht
 */
static void hs_resize_up(hash_set *ht) {
  const int new_capacity = ht->base_capacity * 2;

  hs_resize(ht, new_capacity);
}

/**
 * Resize the table to a smaller size, the first prime subsequent
 * to approx. 1/2x the base capacity.
 *
 * @param ht
 */
static void hs_resize_down(hash_set *ht) {
  const int new_capacity = ht->base_capacity / 2;

  hs_resize(ht, new_capacity);
}

/**
 * Delete a record and deallocate its memory
 *
 * @param r Record to delete
 */
static void hs_delete_record(char *r) { free(r); }

hash_set *hs_init(int base_capacity) {
  if (!base_capacity) {
    base_capacity = H_DEFAULT_CAPACITY;
  }

  hash_set *ht = malloc(sizeof(hash_set));
  ht->base_capacity = base_capacity;

  ht->capacity = next_prime(ht->base_capacity);
  ht->count = 0;
  ht->keys = calloc((size_t)ht->capacity, sizeof(char *));

  return ht;
}

void hs_insert(hash_set *ht, const void *key) {
  if (ht == NULL) {
    return;
  }

  const int load = ht->count * 100 / ht->capacity;
  if (load > 70) {
    hs_resize_up(ht);
  }

  void *new_record = strdup(key);

  int idx = h_resolve_hash(key, ht->capacity, 0);
  char *current_key = ht->keys[idx];
  int i = 1;

  // i.e. if there was a collision
  while (current_key != NULL) {
    // update existing key/value
    if (strcmp(current_key, key) == 0) {
      hs_delete_record(current_key);
      ht->keys[idx] = new_record;

      return;
    }

    // TODO verify i is 1..
    idx = h_resolve_hash(new_record, ht->capacity, i);
    current_key = ht->keys[idx];
    i++;
  }

  ht->keys[idx] = new_record;
  ht->count++;
}

int hs_contains(hash_set *ht, const char *key) {
  int idx = h_resolve_hash(key, ht->capacity, 0);
  char *current_key = ht->keys[idx];

  int i = 1;
  while (current_key != NULL) {
    if (strcmp(current_key, key) == 0) {
      return 1;
    }

    idx = h_resolve_hash(key, ht->capacity, i);
    current_key = ht->keys[idx];
    i++;
  }

  return 0;
}

void hs_delete_set(hash_set *ht) {
  for (int i = 0; i < ht->capacity; i++) {
    char *r = ht->keys[i];

    if (r != NULL) {
      hs_delete_record(r);
    }
  }

  free(ht->keys);
  free(ht);
}

int hs_delete(hash_set *ht, const char *key) {
  const int load = ht->count * 100 / ht->capacity;

  if (load < 10) {
    hs_resize_down(ht);
  }

  int i = 0;
  int idx = h_resolve_hash(key, ht->capacity, i);

  char *current_key = ht->keys[idx];

  while (current_key != NULL) {
    if (strcmp(current_key, key) == 0) {
      hs_delete_record(current_key);
      ht->keys[idx] = NULL;

      ht->count--;

      return 1;
    }

    idx = h_resolve_hash(key, ht->capacity, ++i);
    current_key = ht->keys[idx];
  }

  return 0;
}
