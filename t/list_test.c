#include "list.h"

#include <stdio.h>

#include "tests.h"

static void test_list_prepend(void) {
  node_t *head = list_node_create(100);

  list_prepend(&head, 200);

  ok(head->next->value == 100, "tail node is now 100");
  ok(head->value == 200, "head node is now 200");
}

static void test_list_prepend_on_sentinel(void) {
  node_t *head = list_create_sentinel_node();

  list_prepend(&head, 200);
  list_prepend(&head, 300);
  list_prepend(&head, 400);
  list_prepend(&head, 500);

  ok(list_is_sentinel_node(head->next->next->next->next),
     "tail node is now sentinel");
  ok(head->next->next->next->next->value == 0, "tail node is now sentinel");
  ok(head->value == 500, "expected value");
  ok(head->next->value == 400, "expected value");
  ok(head->next->next->value == 300, "expected value");
  ok(head->next->next->next->value == 200, "expected value");
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

static void test_list_basic(void) {
  node_t *head = list_create_sentinel_node();

  list_prepend(&head, 200);
  list_prepend(&head, 300);
  list_remove(&head, 300);
  list_prepend(&head, 400);
  list_prepend(&head, 500);
  list_remove(&head, 500);
  list_prepend(&head, 600);

  ok(list_is_sentinel_node(head->next->next->next),
     "tail node is now sentinel");
  ok(head->next->next->next->value == 0, "tail node is now sentinel");
  ok(head->value == 600, "expected value");
  ok(head->next->value == 400, "expected value");
  ok(head->next->next->value == 200, "expected value");
}

void run_list_tests() {
  test_list_prepend();
  test_list_prepend_on_sentinel();
  test_list_remove();
  test_list_basic();
}
