#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "hashed.h"

static h_record H_RECORD_SENTINEL = {
	NULL,
	NULL
};

static const int H_PRIME_1 = 151;
static const int H_PRIME_2 = 163;

/**
 * @brief Hash a given ASCII key, where `prime` is a prime number
 * larger than 128 (ASCII alphabet max)
 *
 * @param key
 * @param prime
 * @param capacity
 * @return int
 */
static int h_hash (const char* key, const int prime, const int capacity) {
	long hash = 0;

	const int len_s = strlen(key);

	for (int i = 0; i < len_s; i++) {
		// convert the key to a large integer
		hash += (long)pow(prime, len_s - (i+1)) * key[i];
		// reduce said large integer to a fixed range
		hash = hash % capacity;
	}

	return (int)hash;
}

/**
 * @brief Resolve a hash from the given key, using open addressed double-hashing.
 * This method is adjusted contingent on the number of attempts to resolve
 * a hash without a collision. If no collisions have occurred, i == 0 and we
 * resolve to `hash_a`. If a collision occurs, we modify the hash with `hash_b`.
 * Finally, if `hash_b` returns 0, the second term is reduced to 0, causing the table to
 * attempt inserting into the same index indefinitely. We mitigate this behavior by adding
 * 1 to the result of `hash_b`, ensuring it is never 0.
 *
 * @param key
 * @param capacity
 * @param attempt Number of attempts made to generate a non-colliding hash.
 * @return int
 */
static int h_resolve_hash (
	const char* key, const int capacity, const int attempt
) {
	const int hash_a = h_hash(key, H_PRIME_1, capacity);
	const int hash_b = h_hash(key, H_PRIME_2, capacity);

	return (hash_a + (attempt * (hash_b + 1))) % capacity;
}

/**
 * @brief Initialize a new hash table record with the given k, v pair
 *
 * @param k Record key
 * @param v Record value
 * @return h_record*
 */
static h_record* h_init_record (const char* k, const char* v) {
	h_record* r = malloc(sizeof(h_record));
	r->key = strdup(k);
	r->value = strdup(v);

	return r;
}

/**
 * @brief Delete a record and deallocate its memory
 *
 * @param r Record to delete
 */
static void h_delete_record (h_record* r) {
	free(r->key);
	free(r->value);
	free(r);
}

/**
 * Public API
 */

/**
 * @brief Initialize a new hash table with a size of `max_size`
 *
 * @param max_size The hash table capacity
 * @return h_table*
 */
h_table* h_init_table (int max_capacity) {
	if (!max_capacity || max_capacity < 1) {
		max_capacity = 50;
	}

	h_table* ht = malloc(sizeof(h_table));

	ht->capacity = max_capacity;
	ht->count = 0;
	ht->records = calloc((size_t)ht->capacity, sizeof(h_record*));

	return ht;
}

/**
 * @brief Delete a hash table and deallocate its memory
 *
 * @param ht Hash table to delete
 */
void h_delete_table (h_table* ht) {
	for (int i = 0; i < ht->capacity; i++) {
		h_record* r = ht->records[i];

		if (r != NULL) {
			h_delete_record(r);
		}
	}

	free(ht->records);
	free(ht);
}

/**
 * @brief Insert a key, value pair into the given hash table.
 *
 * @param ht
 * @param key
 * @param value
 */
void h_insert (h_table* ht, const char* key, const char* value) {
	h_record* new_record = h_init_record(key, value);

  int i = 0;
  int idx = h_resolve_hash(new_record->key, ht->capacity, i);

  h_record* current_record = ht->records[idx];

	// i.e. if there was a collision
  while (current_record != NULL) {
		// update existing key/value
		if (strcmp(current_record->key, key) == 0) {
      h_delete_record(current_record);
      ht->records[idx] = new_record;

      return;
    }

		// TODO verify i is 1..
    idx = h_resolve_hash(new_record->key, ht->capacity, ++i);
		current_record = ht->records[idx];
  }

  ht->records[idx] = new_record;
  ht->count++;
}

/**
 * @brief Search
 *
 * @param ht
 * @param key
 * @return char*
 */
char* h_search (h_table* ht, const char* key) {
  int i = 0;
  int idx = h_resolve_hash(key, ht->capacity, i);

  h_record* current_record = ht->records[idx];

  while (current_record != NULL && current_record != &H_RECORD_SENTINEL) {
		if (strcmp(current_record->key, key) == 0) {
			return current_record->value;
		}

    idx = h_resolve_hash(key, ht->capacity, ++i);
		current_record = ht->records[idx];
  }

	return NULL;
}

/**
 * @brief Delete a record for the given key `key`. Because records
 * may be part of a collision chain, and removing them completely
 * could cause infinite lookup attempts, we replace the deleted record
 * with a NULL sentinel record.
 *
 * @param ht
 * @param key
 *
 * @return int 0 if a record was deleted, 1 if no record corresponding
 * to the given key could be found
 */
int h_delete (h_table* ht, const char* key) {
	int ret = 1;
  int i = 0;
	int idx = h_resolve_hash(key, ht->capacity, i);

	h_record* current_record = ht->records[idx];

	while (current_record != NULL && current_record != &H_RECORD_SENTINEL) {
		if (strcmp(current_record->key, key) == 0) {
			h_delete_record(current_record);
			ht->records[idx] = &H_RECORD_SENTINEL;

			ret = 0;
			ht->count--;
		}

    idx = h_resolve_hash(key, ht->capacity, ++i);
		current_record = ht->records[idx];
  }

	return ret;
}
