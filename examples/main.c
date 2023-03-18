#include <stdio.h>

#include "libhash.h"

int main(int argc, char const* argv[]) {
  hash_table* ht = ht_init(0);

  ht_insert(ht, "key", "value2");

  printf("value: %s", ht_get(ht, "key"));

  return 0;
}
