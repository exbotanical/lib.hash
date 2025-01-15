#include <math.h>
#include <stdio.h>

#include "tests.h"

// include the entire source so we may test static functions
// without conditional compilation
#include "hash_table.c"

static hash_table *init_test_ht(void) {
  hash_table *ht = ht_init(10, NULL);

  ht_insert(ht, "k1", "v1");
  ht_insert(ht, "k2", "v2");
  ht_insert(ht, "k3", "v3");

  return ht;
}

static void test_ht_initialization(void) {
  char *k = "key";
  char *v = "value";

  hash_table *ht = ht_init(20, NULL);
  ht_entry *r = ht_entry_init(k, v);

  ok(ht != NULL, "hash table is not NULL");
  ok(ht->base_capacity == HT_DEFAULT_CAPACITY,
     "given base capacity has been adjusted to min %d", HT_DEFAULT_CAPACITY);

  ok(ht->count == 0, "initial count is 0");

  lives({ ht_delete_table(ht); }, "frees the hash table heap memory");

  is(r->key, k, "key match");
  is(r->value, v, "value match");

  lives({ ht_delete_entry(r, false); }, "frees the entry heap memory");
}

static void test_ht_insert(void) {
  hash_table *ht = ht_init(10, NULL);

  ht_insert(ht, "k1", "v1");

  ok(ht->count == 1, "increments the count when a entry is inserted");

  ht_insert(ht, "k2", "v2");
  ok(ht->count == 2, "increments the count when another entry is inserted");

  is(ht_get(ht, "k1"), "v1", "retrieves the correct value");
  is(ht_get(ht, "k2"), "v2", "retrieves the correct value");

  ht_insert(ht, "k1", "v2");
  ok(ht->count == 2,
     "maintains the count, as the preceding insert was an update operation");
  is(ht_get(ht, "k1"), "v2", "retrieves the updated value");

  ht_delete(ht, "k1");
  ok(ht->count == 1, "count after deletion");

  ht_insert(ht, "k1", "v1");
  ok(ht->count == 2, "count after insertion");
  is(ht_get(ht, "k1"), "v1",
     "inserts the entry clean after having been deleted");
}

static void test_ht_search(void) {
  hash_table *ht = init_test_ht();

  is(ht_search(ht, "k1")->value, "v1", "retrieves the value");
  // `ht_get`	is essentially a wrapper for `ht_search`
  is(ht_get(ht, "k2"), "v2", "retrieves the value");
  is(ht_get(ht, "k3"), "v3", "retrieves the value");
  is(ht_get(ht, "k4"), NULL,
     "returns NULL if the search does not yield a entry");

  ht_delete(ht, "k1");
  is(ht_get(ht, "k1"), NULL, "returns NULL if the entry was deleted");
}

static void test_ht_delete(void) {
  hash_table *ht = init_test_ht();

  ok(ht_delete(ht, "k1") == 1, "returns 1 when entry deletion was successful");
  ok(ht_delete(ht, "k2") == 1, "returns 1 when entry deletion was successful");
  ok(ht_delete(ht, "k3") == 1, "returns 1 when entry deletion was successful");

  ok(ht_delete(ht, "k4") == 0,
     "returns 0 when entry deletion was unsuccessful");

  ok(ht_delete(ht, "k1") == 0, "cannot delete the same entry twice");

  is(ht_get(ht, "k1"), NULL, "returns NULL because the entry has been deleted");
  is(ht_get(ht, "k2"), NULL, "returns NULL because the entry has been deleted");
  is(ht_get(ht, "k3"), NULL, "returns NULL because the entry has been deleted");
}

static void test_ht_capacity(void) {
  int initial_cap = 23;
  hash_table *ht = ht_init(initial_cap, NULL);

  for (int i = 0; i < initial_cap; i++) {
    double digits = i == 0 ? 1 : floor(log10(abs(i))) + 1;

    char buf[(int)digits + 2];
    snprintf(buf, digits + 2, "%s%d", "k", i);

    ht_insert(ht, buf, "x");
  }

  ok(ht->base_capacity == HT_DEFAULT_CAPACITY, "extends the base capacity");
  ok(ht->capacity == HT_DEFAULT_CAPACITY, "extends the actual capacity");
  ok(ht->count == initial_cap, "maintains the count");
}

static void test_ht_delete_with_free(void) {
  hash_table *ht = ht_init(10, free);

  char *v1 = strdup("v1");
  char *v2 = strdup("v2");
  char *v3 = strdup("v3");

  ht_insert(ht, "k1", v1);
  ht_insert(ht, "k2", v2);
  ht_insert(ht, "k3", v3);

  ok(ht_delete(ht, "k1") == 1, "returns 1 when entry deletion was successful");
  ok(strcmp(v1, "v1") != 0, "passes the value to the provided free function");
  ok(ht_delete(ht, "k2") == 1, "returns 1 when entry deletion was successful");
  ok(strcmp(v2, "v2") != 0, "passes the value to the provided free function");
  ok(ht_delete(ht, "k3") == 1, "returns 1 when entry deletion was successful");
  ok(strcmp(v3, "v3") != 0, "passes the value to the provided free function");

  ok(ht_delete(ht, "k4") == 0,
     "returns 0 when entry deletion was unsuccessful");
  ok(ht_delete(ht, "k1") == 0, "cannot delete the same entry twice");

  is(ht_get(ht, "k1"), NULL, "returns NULL because the entry has been deleted");
  is(ht_get(ht, "k2"), NULL, "returns NULL because the entry has been deleted");
  is(ht_get(ht, "k3"), NULL, "returns NULL because the entry has been deleted");

  // It'll be some junk value - just not the og value.
  ok(strcmp(v1, "v1") != 0, "frees the entry value");
  ok(strcmp(v2, "v2") != 0, "frees the entry value");
  ok(strcmp(v3, "v3") != 0, "frees the entry value");
}

static void test_ht_iterate(void) {
  hash_table *ht = init_test_ht();
  ht_delete(ht, "k2");
  ht_insert(ht, "k4", "v4");

  unsigned int count = 0;
  HT_ITER_START(ht)
  switch (count++) {
    case 0:
      is(entry->key, "k4", "most recent entry at head");
      break;
    case 1:
      is(entry->key, "k3", "retains entry");
      break;
    case 2:
      is(entry->key, "k1", "first entry at tail");
      break;
    case 3:
      is(entry->key, NULL, "terminates where expected");
      break;
  }
  HT_ITER_END
}

static void test_hash_bugfix_1(void) {
  const char *s1 = "^([a-zA-Z_-][a-zA-Z0-9_-]*)=\"([^\"]*)\"(?<! )$";
  const char *s2 = "crontabs";
  const char *s3 = "^w.*";

  hash_table *ht = ht_init(1, NULL);

  ht_insert(ht, s1, "x");
  ht_insert(ht, s2, "y");
  ht_insert(ht, s3, "z");

  is("x", ht_get(ht, s1), "retrieves expected value");
  is("y", ht_get(ht, s2), "retrieves expected value");
  is("z", ht_get(ht, s3), "retrieves expected value");
}

void run_hash_table_tests(void) {
  test_ht_initialization();
  test_ht_insert();
  test_ht_search();
  test_ht_delete();
  test_ht_capacity();
  test_ht_delete_with_free();
  test_ht_iterate();
  test_hash_bugfix_1();
}
