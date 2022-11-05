#include "tap.c/tap.h"
// include the entire source so we may test static functions
// without conditional compilation
#include "hash.c"
#include <stdio.h>

void test_initialization (void) {
	int capacity = 20;
	char *k = "key";
	char *v = "value";

	h_table *ht = h_init_table(capacity);
	h_record *r = h_init_record(k, v);

	isnt(ht, NULL, "hash table is not NULL");
	is(ht->base_capacity, capacity, "given base capacity has been set");

	is(ht->capacity, next_prime(capacity), "given base capacity has been set");

	is(ht->count, 0, "initial count is 0");

	lives_ok({
		h_delete_table(ht);
	}, "frees the hash table heap memory");

	is(r->key, k, "key match");
	is(r->value, v, "value match");

	lives_ok({
		h_delete_record(r);
	}, "frees the record heap memory");
}

void test_insert (void) {
	h_table *ht = h_init_table(10);

	h_insert(ht, "k1", "v1");

	is(ht->count, 1, "increments the count when a record is inserted");

	h_insert(ht, "k2", "v2");
	is(ht->count, 2, "increments the count when another record is inserted");

	is(h_retrieve(ht, "k1"), "v1", "retrieves the correct value");
	is(h_retrieve(ht, "k2"), "v2", "retrieves the correct value");

	h_insert(ht, "k1", "v2");
	is(ht->count, 2, "maintains the count, as the preceding insert was an update operation");
	is(h_retrieve(ht, "k1"), "v2", "retrieves the updated value");

	h_delete(ht, "k1");
	is(ht->count, 1);

	h_insert(ht, "k1", "v1");
	is(ht->count, 2);
	is(h_retrieve(ht, "k1"), "v1", "inserts the record clean after having been deleted");
}

void test_search (void) {
	h_table *ht = h_init_table(10);

	h_insert(ht, "k1", "v1");
	h_insert(ht, "k2", "v2");
	h_insert(ht, "k3", "v3");

	is(h_search(ht, "k1")->value, "v1", "retrieves the value");
	// `h_retrieve`	is essentially a wrapper for `h_search`
	is(h_retrieve(ht, "k2"), "v2", "retrieves the value");
	is(h_retrieve(ht, "k3"), "v3", "retrieves the value");
	is(h_retrieve(ht, "k4"), NULL, "returns NULL if the search does not yield a record");

	h_delete(ht, "k1");
	is(h_retrieve(ht, "k1"), NULL, "returns NULL if the record was deleted");
}

void test_delete (void) {
	h_table *ht = h_init_table(10);

	h_insert(ht, "k1", "v1");
	h_insert(ht, "k2", "v2");
	h_insert(ht, "k3", "v3");

	is(h_delete(ht, "k1"), 0, "returns 0 when record deletion was successful");
	is(h_delete(ht, "k2"), 0, "returns 0 when record deletion was successful");
	is(h_delete(ht, "k3"), 0, "returns 0 when record deletion was successful");

	is(h_delete(ht, "k4"), 1, "returns 1 when record deletion was unsuccessful");
	is(h_delete(ht, "k1"), 1, "cannot delete the same record twice");

	is(h_retrieve(ht, "k1"), NULL, "returns NULL because the record has been deleted");
	is(h_retrieve(ht, "k2"), NULL, "returns NULL because the record has been deleted");
	is(h_retrieve(ht, "k3"), NULL, "returns NULL because the record has been deleted");
}

void test_capacity (void) {
	int initial_cap = 23;
	h_table *ht = h_init_table(initial_cap);

	for (int i = 0; i < initial_cap; i++) {
		char *buf;
		snprintf(buf, 5, "%s%d", "k", i);

		h_insert(ht, buf, "x");
	}

	h_insert(ht, "h", "11");

	is(ht->base_capacity, 46, "extends the base capacity");
	is(ht->capacity, next_prime(initial_cap * 2), "extends the actual capacity");
	is(ht->count, initial_cap + 1, "maintains the count");
}

int main (int argc, char *argv[]) {
	plan(33);

	test_initialization();
	test_insert();
	test_search();
	test_delete();
	test_capacity();

  done_testing();
}
