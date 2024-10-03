#include <math.h>
#include <stdio.h>

#include "libhash.h"
#include "libtap/libtap.h"
#include "prime.h"

void test_initialization(void) {
  int capacity = 20;
  char *k = "key";

  hash_set *hs = hs_init(capacity);

  ok(hs != NULL, "hash set is not NULL");
  ok(hs->base_capacity == capacity, "given base capacity has been set");

  ok(hs->capacity == next_prime(capacity), "given base capacity has been set");

  ok(hs->count == 0, "initial count is 0");

  lives({ hs_delete_set(hs); }, "frees the hash set heap memory");
}

void test_insert(void) {
  hash_set *hs = hs_init(10);
  const char *k1 = "k1";
  const char *k2 = "k2";

  hs_insert(hs, k1);
  ok(hs->count == 1, "increments the count when a key is inserted");

  hs_insert(hs, k2);
  ok(hs->count == 2, "increments the count when another key is inserted");

  ok(hs_contains(hs, k1) == 1, "contains the originally inserted key");
  ok(hs_contains(hs, k2) == 1, "contains the inserted key");

  hs_insert(hs, k1);
  ok(hs->count == 2,
     "maintains the count, as the preceding insert was an update operation");
  ok(hs_contains(hs, k1) == 1, "contains the re-inserted key");

  hs_delete(hs, k1);
  ok(hs->count == 1, "count after deletion");

  hs_insert(hs, k1);
  ok(hs->count == 2, "count after insertion");
  ok(hs_contains(hs, k1), "inserts the key clean after having been deleted");
}

void test_contains(void) {
  hash_set *hs = hs_init(10);
  const char *k1 = "k1";
  const char *k2 = "k2";
  const char *k3 = "k3";
  const char *k4 = "k4";

  hs_insert(hs, k1);
  hs_insert(hs, k2);
  hs_insert(hs, k3);

  ok(hs_contains(hs, k1) == 1, "retrieves the value");

  ok(hs_contains(hs, k2) == 1, "retrieves the value");
  ok(hs_contains(hs, k3) == 1, "retrieves the value");
  ok(hs_contains(hs, k4) == 0, "returns 0 if the search does not yield a key");

  hs_delete(hs, k1);
  ok(hs_contains(hs, k1) == 0, "returns 0 if the entry was deleted");
}

void test_delete(void) {
  const char *k1 = "k1";
  const char *k2 = "k2";
  const char *k3 = "k3";
  const char *k4 = "k4";

  hash_set *hs = hs_init(10);

  hs_insert(hs, k2);
  hs_insert(hs, k3);
  hs_insert(hs, k1);

  ok(hs_delete(hs, k1) == 1, "returns 1 when key deletion was successful");

  ok(hs_delete(hs, k2) == 1, "returns 1 when key deletion was successful");
  ok(hs_delete(hs, k3) == 1, "returns 1 when key deletion was successful");

  ok(hs_delete(hs, k4) == 0, "returns 0 when key deletion was unsuccessful");
  ok(hs_delete(hs, k1) == 0, "cannot delete the same key twice");

  ok(hs_contains(hs, "k1") == 0,
     "returns 0 because the entry has been deleted");
  ok(hs_contains(hs, "k2") == 0,
     "returns 0 because the entry has been deleted");
  ok(hs_contains(hs, "k3") == 0,
     "returns 0 because the entry has been deleted");
}

void test_capacity(void) {
  int initial_cap = 23;
  hash_set *hs = hs_init(initial_cap);

  for (unsigned int i = 0; i < initial_cap; i++) {
    double digits = i == 0 ? 1 : floor(log10(abs(i))) + 1;

    char buf[(int)digits + 2];
    snprintf(buf, digits + 2, "%s%d", "k", i);

    hs_insert(hs, buf);
  }

  ok(hs->base_capacity == 46, "extends the base capacity");
  ok(hs->capacity == next_prime(initial_cap * 2),
     "extends the actual capacity");
  ok(hs->count == initial_cap, "maintains the count");
}

void test_contains_miss(void) {
  hash_set *hs = hs_init(2);

  hs_insert(hs, "Content-Type");
  hs_insert(hs, "Content-Length");

  ok(hs_contains(hs, "key2") == 0, "does not contain the key");
}

int main() {
  plan(31);

  test_initialization();
  test_insert();
  test_contains();
  test_delete();
  test_capacity();
  test_contains_miss();

  done_testing();
}
