#ifndef LIBHASH_LIST_H
#define LIBHASH_LIST_H

#include <stdio.h>
#include <stdlib.h>

typedef struct node node_t;
struct node {
  int value;
  node_t *next;
};

static node_t LIST_SENTINEL_NODE = {0, NULL};

static node_t *list_node_create_head(void) { return &LIST_SENTINEL_NODE; }

static node_t *list_node_create(const int value) {
  node_t *n = (node_t *)malloc(sizeof(node_t));
  n->value = value;
  return n;
}

static void list_prepend(node_t **head, int value) {
  // TODO: xmalloc
  node_t *new_node = (node_t *)malloc(sizeof(node_t));
  new_node->value = value;

  node_t *tmp = *head;
  *head = new_node;
  new_node->next = tmp;
}

static void list_remove(node_t **head, int value) {
  node_t *current = *head;
  node_t *prev = NULL;

  while (current != NULL) {
    if (current->value == value) {
      if (prev == NULL) {
        *head = current->next;
      } else {
        prev->next = current->next;
      }
      free(current);
      return;
    }
    prev = current;
    current = current->next;
  }
}

// TODO: Use and test
static void list_free(node_t *head) {
  node_t *tmp;
  while (head != &LIST_SENTINEL_NODE) {
    tmp = head;
    head = head->next;
    free(tmp);
  }
}

#endif /* LIBHASH_LIST_H */
