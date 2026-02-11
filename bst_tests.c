#include "bst.h"

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define mu_assert(message, test)                                               \
  do {                                                                         \
    if (!(test))                                                               \
      return message;                                                          \
  } while (0)

static void test_bst_node_create(bst_node *self) {
  self->key = 0;
  self->left = NULL;
  self->right = NULL;
  self->parent = NULL;
}

static char *test_bst_null() {
  bst_create(NULL);
  size_t size = bst_size(NULL);
  bst_destroy(NULL);
  mu_assert("error, bst size is not 0", size == 0);
  return NULL;
}

static char *test_bst_empty() {
  bst b;
  bst_create(&b);
  mu_assert("error, bst size is not 0", bst_size(&b) == 0);
  mu_assert("error, bst height is not 0", bst_height(&b) == 0);
  mu_assert("error, bst search on empty should be NULL",
            bst_search(&b, 42) == NULL);
  bst_destroy(&b);
  return NULL;
}

static char *test_bst_size_single() {
  bst b;
  bst_create(&b);
  b.root = calloc(1, sizeof(*b.root));
  test_bst_node_create(b.root);
  mu_assert("error, bst size is not 1", bst_size(&b) == 1);
  bst_destroy(&b);
  return NULL;
}

static char *test_bst_size_two() {
  bst b;
  bst_create(&b);
  b.root = calloc(1, sizeof(*b.root));
  test_bst_node_create(b.root);
  b.root->left = calloc(1, sizeof(*b.root));
  test_bst_node_create(b.root->left);
  b.root->left->parent = b.root;
  mu_assert("error, bst size is not 2", bst_size(&b) == 2);
  bst_destroy(&b);
  return NULL;
}

static char *test_bst_height_manual_shapes() {
  bst b;
  bst_create(&b);

  mu_assert("error, height empty should be 0", bst_height(&b) == 0);

  b.root = calloc(1, sizeof(*b.root));
  test_bst_node_create(b.root);
  b.root->key = 10;
  mu_assert("error, height single node should be 1", bst_height(&b) == 1);

  b.root->left = calloc(1, sizeof(*b.root));
  test_bst_node_create(b.root->left);
  b.root->left->key = 5;
  b.root->left->parent = b.root;
  mu_assert("error, height 2 nodes should be 2", bst_height(&b) == 2);

  b.root->left->left = calloc(1, sizeof(*b.root));
  test_bst_node_create(b.root->left->left);
  b.root->left->left->key = 3;
  b.root->left->left->parent = b.root->left;
  mu_assert("error, height 3 nodes skewed should be 3", bst_height(&b) == 3);

  bst_destroy(&b);
  return NULL;
}

static char *test_bst_inorder_walk_smoke() {
  bst b;
  bst_create(&b);

  b.root = calloc(1, sizeof(*b.root));
  test_bst_node_create(b.root);
  b.root->key = 2;

  b.root->left = calloc(1, sizeof(*b.root));
  test_bst_node_create(b.root->left);
  b.root->left->key = 1;
  b.root->left->parent = b.root;

  b.root->right = calloc(1, sizeof(*b.root));
  test_bst_node_create(b.root->right);
  b.root->right->key = 3;
  b.root->right->parent = b.root;

  inorder_bst_walk(NULL);
  inorder_bst_walk(&b);

  bst_destroy(&b);
  return NULL;
}

static char *test_bst_insert_and_size_basic() {
  bst b;
  bst_create(&b);
  bst_insert(&b, 5);
  bst_insert(&b, 3);
  bst_insert(&b, 7);
  mu_assert("error, bst size is not 3", bst_size(&b) == 3);
  bst_destroy(&b);
  return NULL;
}

static char *test_bst_search_basic() {
  bst b;
  bst_create(&b);
  bst_insert(&b, 5);
  bst_insert(&b, 3);
  bst_insert(&b, 7);

  mu_assert("error, bst search 5 is not found", bst_search(&b, 5) != NULL);
  mu_assert("error, bst search 3 is not found", bst_search(&b, 3) != NULL);
  mu_assert("error, bst search 7 is not found", bst_search(&b, 7) != NULL);
  mu_assert("error, bst search 4 is found", bst_search(&b, 4) == NULL);

  bst_destroy(&b);
  return NULL;
}

static char *test_bst_insert_returns_nodes_and_parents() {
  bst b;
  bst_create(&b);

  bst_node *n5 = bst_insert(&b, 5);
  bst_node *n3 = bst_insert(&b, 3);
  bst_node *n7 = bst_insert(&b, 7);
  bst_node *n4 = bst_insert(&b, 4);

  mu_assert("error, insert should return non-null",
            n5 != NULL && n3 != NULL && n7 != NULL && n4 != NULL);
  mu_assert("error, root key should be 5", b.root != NULL && b.root->key == 5);
  mu_assert("error, root parent should be NULL", b.root->parent == NULL);

  mu_assert("error, 3 should be left child of 5",
            b.root->left != NULL && b.root->left->key == 3);
  mu_assert("error, 7 should be right child of 5",
            b.root->right != NULL && b.root->right->key == 7);

  mu_assert("error, parent of 3 should be 5", b.root->left->parent == b.root);
  mu_assert("error, parent of 7 should be 5", b.root->right->parent == b.root);

  mu_assert("error, 4 should be right child of 3",
            b.root->left->right != NULL && b.root->left->right->key == 4);
  mu_assert("error, parent of 4 should be 3",
            b.root->left->right->parent == b.root->left);

  bst_destroy(&b);
  return NULL;
}

static char *test_bst_insert_ordered_causes_height() {
  bst b;
  bst_create(&b);

  for (int i = 1; i <= 10; i++)
    bst_insert(&b, i);

  mu_assert("error, size should be 10", bst_size(&b) == 10);
  mu_assert("error, height should be 10 for increasing inserts",
            bst_height(&b) == 10);

  bst_destroy(&b);
  return NULL;
}

static char *test_bst_extreme_values() {
  bst b;
  bst_create(&b);

  bst_insert(&b, INT_MIN);
  bst_insert(&b, 0);
  bst_insert(&b, INT_MAX);

  mu_assert("error, INT_MIN not found", bst_search(&b, INT_MIN) != NULL);
  mu_assert("error, 0 not found", bst_search(&b, 0) != NULL);
  mu_assert("error, INT_MAX not found", bst_search(&b, INT_MAX) != NULL);

  bst_destroy(&b);
  return NULL;
}

static char *test_bst_destroy_is_idempotent() {
  bst b;
  bst_create(&b);
  bst_insert(&b, 2);
  bst_insert(&b, 1);
  bst_insert(&b, 3);

  bst_destroy(&b);
  mu_assert("error, after destroy root must be NULL", b.root == NULL);
  mu_assert("error, size after destroy must be 0", bst_size(&b) == 0);

  bst_destroy(&b);
  mu_assert("error, destroy twice should keep root NULL", b.root == NULL);
  return NULL;
}

static char *test_bst_search_null_tree() {
  mu_assert("error, search on NULL should be NULL",
            bst_search(NULL, 1) == NULL);
  return NULL;
}

static char *test_bst_insert_null_tree() {
  mu_assert("error, insert on NULL should be NULL",
            bst_insert(NULL, 1) == NULL);
  return NULL;
}

char *(*tests_functions[])() = {
    test_bst_null,
    test_bst_empty,
    test_bst_size_single,
    test_bst_size_two,
    test_bst_height_manual_shapes,
    test_bst_inorder_walk_smoke,
    test_bst_insert_and_size_basic,
    test_bst_search_basic,
    test_bst_insert_returns_nodes_and_parents,
    test_bst_insert_ordered_causes_height,
    test_bst_extreme_values,
    test_bst_destroy_is_idempotent,
    test_bst_search_null_tree,
    test_bst_insert_null_tree,
};

int main(int argc, const char *argv[]) {
  size_t n = sizeof(tests_functions) / sizeof(tests_functions[0]);
  size_t test_index;

  if (argc == 1) {
    const char *msg[n];
    size_t tests_pass = 0;

    for (test_index = 0; test_index < n; test_index++) {
      const char *message = tests_functions[test_index]();
      if (message) {
        printf("F");
      } else {
        printf(".");
        tests_pass++;
      }
      msg[test_index] = message;
    }

    printf("\n");
    for (test_index = 0; test_index < n; test_index++)
      if (msg[test_index])
        printf("Test %zu failed: %s\n", test_index, msg[test_index]);

    if (n == tests_pass)
      printf("All %zu tests passed\n", n);
    else {
      printf("Tests passed/run: %zu/%zu\n", tests_pass, n);
      exit(1);
    }
  } else {
    int index = atoi(argv[1]);
    if (index < 0) {
      printf("%zu\n", n);
    } else {
      test_index = (size_t)index;
      if (test_index < n) {
        const char *message = tests_functions[test_index]();
        if (message) {
          printf("Test %zu failed: %s\n", test_index, message);
          exit(1);
        } else
          printf("Test %zu passed\n", test_index);
      }
    }
  }

  exit(0);
}
