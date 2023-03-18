#include <math.h>
#include <stdio.h>

#include "tap.c/tap.h"

// include the entire source so we may test static functions
// without conditional compilation
#include "hash_table.c"

void test_initialization(void) {
  int capacity = 20;
  char *k = "key";
  char *v = "value";

  hash_table *ht = ht_init(capacity);
  ht_record *r = h_init_record(k, v);

  ok(ht != NULL, "hash table is not NULL");
  ok(ht->base_capacity == capacity, "given base capacity has been set");

  ok(ht->capacity == next_prime(capacity), "given base capacity has been set");

  ok(ht->count == 0, "initial count is 0");

  lives_ok({ ht_delete_table(ht); }, "frees the hash table heap memory");

  is(r->key, k, "key match");
  is(r->value, v, "value match");

  lives_ok({ ht_delete_record(r); }, "frees the record heap memory");
}

void test_insert(void) {
  hash_table *ht = ht_init(10);

  ht_insert(ht, "k1", "v1");

  ok(ht->count == 1, "increments the count when a record is inserted");

  ht_insert(ht, "k2", "v2");
  ok(ht->count == 2, "increments the count when another record is inserted");

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
     "inserts the record clean after having been deleted");
}

void test_search(void) {
  hash_table *ht = ht_init(10);

  ht_insert(ht, "k1", "v1");
  ht_insert(ht, "k2", "v2");
  ht_insert(ht, "k3", "v3");

  is(ht_search(ht, "k1")->value, "v1", "retrieves the value");
  // `ht_get`	is essentially a wrapper for `ht_search`
  is(ht_get(ht, "k2"), "v2", "retrieves the value");
  is(ht_get(ht, "k3"), "v3", "retrieves the value");
  is(ht_get(ht, "k4"), NULL,
     "returns NULL if the search does not yield a record");

  ht_delete(ht, "k1");
  is(ht_get(ht, "k1"), NULL, "returns NULL if the record was deleted");
}

void test_delete(void) {
  hash_table *ht = ht_init(10);

  ht_insert(ht, "k1", "v1");
  ht_insert(ht, "k2", "v2");
  ht_insert(ht, "k3", "v3");

  ok(ht_delete(ht, "k1") == 1, "returns 1 when record deletion was successful");
  ok(ht_delete(ht, "k2") == 1, "returns 1 when record deletion was successful");
  ok(ht_delete(ht, "k3") == 1, "returns 1 when record deletion was successful");

  ok(ht_delete(ht, "k4") == 0,
     "returns 0 when record deletion was unsuccessful");
  ok(ht_delete(ht, "k1") == 0, "cannot delete the same record twice");

  is(ht_get(ht, "k1"), NULL,
     "returns NULL because the record has been deleted");
  is(ht_get(ht, "k2"), NULL,
     "returns NULL because the record has been deleted");
  is(ht_get(ht, "k3"), NULL,
     "returns NULL because the record has been deleted");
}

void test_capacity(void) {
  int initial_cap = 23;
  hash_table *ht = ht_init(initial_cap);

  for (int i = 0; i < initial_cap; i++) {
    double digits = i == 0 ? 1 : floor(log10(abs(i))) + 1;

    char buf[(int)digits + 2];
    snprintf(buf, digits + 2, "%s%d", "k", i);

    ht_insert(ht, buf, "x");
  }

  ok(ht->base_capacity == 46, "extends the base capacity");
  ok(ht->capacity == next_prime(initial_cap * 2),
     "extends the actual capacity");
  ok(ht->count == initial_cap, "maintains the count");
}

int main(int argc, char *argv[]) {
  plan(33);

  test_initialization();
  test_insert();
  test_search();
  test_delete();
  test_capacity();

  done_testing();
}
