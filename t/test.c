#include "tap.c/tap.h"
// include the entire source so we may test static functions2
// without conditional compilation
#include "hashed.c"

void test_initialization (void) {
	int capacity = 20;
	char* k = "key";
	char* v = "value";

	h_table* ht = h_init_table(capacity);
	h_record* r = h_init_record(k, v);

	plan(7);

	isnt(ht, NULL, "hash table is not NULL");
	is(ht->capacity, capacity, "given capacity has been set");
	is(ht->count, 0, "initial count is 0");

	lives_ok({
		h_delete_table(ht);
	}, "frees the hash table heap memory");

	is(r->key, k, "key match");
	is(r->value, v, "value match");

	lives_ok({
		h_delete_record(r);
	}, "frees the record heap memory");

  done_testing();
}

int main (int argc, char* argv[]) {
	test_initialization();

	return EXIT_SUCCESS;
}
