#include <stdio.h>

#include "libhash.h"

int main(int argc, char const* argv[]) {
  hash_table* ht = ht_init(3, NULL);

  ht_insert(ht, "key1", "value1");
  ht_insert(ht, "key2", "value2");
  ht_insert(ht, "key3", "value3");

  printf("value for key1: %s", ht_get(ht, "key"));

  return 0;
}
