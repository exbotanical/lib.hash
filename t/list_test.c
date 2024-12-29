#include "list.h"

#include <stdio.h>

#include "tests.h"

static void test_list_prepend(void) {
  node_t *head = list_node_create(100);

  list_prepend(&head, 200);

  ok(head->next->value == 100, "tail node is now 100");
  ok(head->value == 200, "head node is now 200");
}

static void test_list_remove(void) {
  node_t *head = list_node_create(100);
  list_prepend(&head, 200);
  list_prepend(&head, 300);
  list_prepend(&head, 400);

  list_remove(&head, 300);

  int n1 = head->value;
  int n2 = head->next->value;
  int n3 = head->next->next->value;

  ok(n1 == 400, "expected value");
  ok(n2 == 200, "expected value");
  ok(n3 == 100, "expected value");
  ok(head->next->next->next == NULL, "no more nodes");
}

void run_list_tests() {
  test_list_prepend();
  test_list_remove();
}
