#include "list.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct __node {
  long value;
  struct list_head head;
} node_t;

int list_length(struct list_head *list) {
  int i = 0;
  node_t *entry;
  list_for_each_entry(entry, list, head) { i++; }
  return i;
}

void list_construct(struct list_head *list, int n) {
  node_t *node = malloc(sizeof(node_t));
  node->value = n;
  list_add(&node->head, list);
}

void list_free(struct list_head *list) {
  node_t *entry, *tmp;
  list_for_each_entry_safe(entry, tmp, list, head) { free(entry); }
}

/* Verify if list is order */
static bool list_is_ordered(struct list_head *list) {
  bool first = true;
  int value;
  node_t *entry;

  list_for_each_entry(entry, list, head) {
    if (first) {
      value = entry->value;
      first = false;
    } else {
      if (entry->value < value)
        return false;
      value = entry->value;
    }
  }
  return true;
}

void quick_sort(struct list_head *list) {
  int n = list_length(list);
  int value, i, max_level = 2 * n;
  struct list_head begin[max_level];
  LIST_HEAD(left);
  LIST_HEAD(right);

  i = 0;
  while (i < max_level)
    INIT_LIST_HEAD(&begin[i++]);
  list_splice(list, &begin[0]);
  INIT_LIST_HEAD(list);

  i = 0;
  while (i >= 0) {
    struct list_head *L = &begin[i];

    if (!list_empty(L) && !list_is_singular(L)) {
      node_t *pivot = list_first_entry(L, node_t, head);
      list_del(&pivot->head);
      value = pivot->value;

      node_t *pos, *tmp;
      list_for_each_entry_safe(pos, tmp, L, head) {
        list_del(&pos->head);
        list_add(&pos->head, pos->value > value ? &right : &left);
      }

      list_splice(&left, &begin[i]);
      list_add(&pivot->head, &begin[i + 1]);
      list_splice(&right, &begin[i + 2]);
      INIT_LIST_HEAD(&left);
      INIT_LIST_HEAD(&right);

      i += 2;
    } else {
      if (list_is_singular(L)) {
        node_t *tmp = list_first_entry(L, node_t, head);
        list_del(&tmp->head);
        list_add(&tmp->head, list);
      }
      i--;
    }
  }
}

/* shuffle array, only work if n < RAND_MAX */
void shuffle(int *array, size_t n) {
  if (n <= 0)
    return;

  for (size_t i = 0; i < n - 1; i++) {
    size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
    int t = array[j];
    array[j] = array[i];
    array[i] = t;
  }
}

int main(int argc, char **argv) {
  LIST_HEAD(list);

  size_t count = 100000;

  int *test_arr = malloc(sizeof(int) * count);

  for (int i = 0; i < count; ++i)
    test_arr[i] = i;
  shuffle(test_arr, count);

  while (count--)
    list_construct(&list, test_arr[count]);

  quick_sort(&list);

  assert(list_is_ordered(&list));

  list_free(&list);

  free(test_arr);

  return 0;
}