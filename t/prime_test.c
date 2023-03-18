#include "prime.h"

#include "tap.c/tap.h"

static const int prime_map[] = {2,  3,  5,  7,  11, 13, 17, 19, 23,
                                29, 31, 37, 41, 43, 47, 53, 59, 61,
                                67, 71, 73, 79, 83, 89, 97};

void test_is_prime() {
  for (int i = 0; i < sizeof(prime_map) / sizeof(int); i++) {
    int prime = prime_map[i];

    ok(is_prime(prime) == 1, "expected prime %d", prime);
  }
}

void test_next_prime() {
  for (int i = 2; i < sizeof(prime_map) / sizeof(int); i++) {
    int prime = prime_map[i];

    ok(next_prime(prime - 1) == prime_map[i], "expected next prime %d", prime);
  }
}

int main(int argc, char const *argv[]) {
  plan(48);

  test_is_prime();
  test_next_prime();

  done_testing();

  return 0;
}
