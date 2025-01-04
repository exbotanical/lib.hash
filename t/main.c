#include "libtap/libtap.h"
#include "tests.h"

int main(void) {
  plan(143);

  run_hash_set_tests();
  run_hash_table_tests();
  run_prime_tests();
  run_list_tests();

  done_testing();
}
