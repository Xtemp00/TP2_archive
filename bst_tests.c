#include "bst.h"

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define mu_assert(message, test)                                               \
  do {                                                                         \
    if (!(test))                                                               \
      return message;                                                          \
  } while (0)

#define mu_assert_tree(tree)                                                   \
  do {                                                                         \
    char *tree_msg = assert_tree_ok(tree);                                     \
    if (tree_msg != NULL)                                                      \
      return tree_msg;                                                         \
  } while (0)

static bool is_bst_node(const bst_node *node, long min, long max) {
  if (node == NULL) {
    return true;
  }
  if ((long)node->key < min || (long)node->key > max) {
    return false;
  }
  return is_bst_node(node->left, min, (long)node->key) &&
         is_bst_node(node->right, (long)node->key, max);
}

static bool has_parent_links(const bst_node *node, const bst_node *parent) {
  if (node == NULL) {
    return true;
  }
  if (node->parent != parent) {
    return false;
  }
  return has_parent_links(node->left, node) &&
         has_parent_links(node->right, node);
}

static char *assert_tree_ok(const bst *tree) {
  mu_assert("error, tree should not be NULL", tree != NULL);
  mu_assert("error, parent pointers are invalid",
            has_parent_links(tree->root, NULL));
  mu_assert("error, tree property is invalid",
            is_bst_node(tree->root, LONG_MIN, LONG_MAX));
  return NULL;
}

static char *test_bst_null() {
  bst_create(NULL);
  size_t size = bst_size(NULL);
  bst_destroy(NULL);
  mu_assert("error, bst size is not 0", size == 0);
  mu_assert("error, bst height(NULL) is not 0", bst_height(NULL) == 0);
  mu_assert("error, minimum(NULL) should be NULL", bst_minimum(NULL) == NULL);
  mu_assert("error, maximum(NULL) should be NULL", bst_maximum(NULL) == NULL);
  bst_delete(NULL, NULL);
  return NULL;
}

static char *test_bst_empty() {
  bst b;
  bst_create(&b);
  mu_assert("error, bst size is not 0", bst_size(&b) == 0);
  mu_assert("error, bst height is not 0", bst_height(&b) == 0);
  mu_assert("error, bst search on empty should be NULL",
            bst_search(&b, 42) == NULL);
  mu_assert("error, minimum empty should be NULL", bst_minimum(&b) == NULL);
  mu_assert("error, maximum empty should be NULL", bst_maximum(&b) == NULL);
  bst_destroy(&b);
  return NULL;
}

static char *test_bst_insert_and_search() {
  bst b;
  bst_create(&b);
  int values[] = {20, 10, 30, 5, 15, 25, 40, 13, 17};

  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
    mu_assert("error, insert returned NULL", bst_insert(&b, values[i]) != NULL);
  }

  mu_assert("error, size should be 9", bst_size(&b) == 9);
  mu_assert("error, height should be 4", bst_height(&b) == 4);
  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
    mu_assert("error, inserted value not found",
              bst_search(&b, values[i]) != NULL);
  }
  mu_assert("error, unknown value found", bst_search(&b, 999) == NULL);
  mu_assert_tree(&b);
  bst_destroy(&b);
  return NULL;
}

static char *test_bst_insert_duplicate_policy() {
  bst b;
  bst_create(&b);

  bst_insert(&b, 10);
  bst_insert(&b, 10);
  bst_insert(&b, 10);

  mu_assert("error, size should be 3 with duplicates", bst_size(&b) == 3);
  mu_assert("error, duplicate should go right",
            b.root->right != NULL && b.root->right->key == 10);
  mu_assert("error, second duplicate should go right-right",
            b.root->right->right != NULL && b.root->right->right->key == 10);
  mu_assert_tree(&b);
  bst_destroy(&b);
  return NULL;
}

static char *test_minimum_maximum_general() {
  bst b;
  bst_create(&b);
  int values[] = {12, 4, 18, 1, 7, 15, 22};
  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    bst_insert(&b, values[i]);

  bst_node *min = bst_minimum(&b);
  bst_node *max = bst_maximum(&b);

  mu_assert("error, minimum should be 1", min != NULL && min->key == 1);
  mu_assert("error, maximum should be 22", max != NULL && max->key == 22);
  bst_destroy(&b);
  return NULL;
}

static char *test_predecessor_successor_middle_nodes() {
  bst b;
  bst_create(&b);
  int values[] = {20, 10, 30, 5, 15, 25, 35};
  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    bst_insert(&b, values[i]);

  bst_node *n20 = bst_search(&b, 20);
  bst_node *n25 = bst_search(&b, 25);
  bst_node *n10 = bst_search(&b, 10);

  mu_assert("error, predecessor(20) should be 15",
            bst_predecessor(n20) != NULL && bst_predecessor(n20)->key == 15);
  mu_assert("error, successor(20) should be 25",
            bst_successor(n20) != NULL && bst_successor(n20)->key == 25);
  mu_assert("error, predecessor(25) should be 20",
            bst_predecessor(n25) != NULL && bst_predecessor(n25)->key == 20);
  mu_assert("error, successor(10) should be 15",
            bst_successor(n10) != NULL && bst_successor(n10)->key == 15);

  bst_destroy(&b);
  return NULL;
}

static char *test_predecessor_successor_extremes() {
  bst b;
  bst_create(&b);
  int values[] = {8, 3, 10, 1, 6, 14};
  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    bst_insert(&b, values[i]);

  bst_node *min = bst_minimum(&b);
  bst_node *max = bst_maximum(&b);

  mu_assert("error, predecessor(min) should be NULL",
            bst_predecessor(min) == NULL);
  mu_assert("error, successor(max) should be NULL", bst_successor(max) == NULL);
  mu_assert("error, predecessor(NULL) should be NULL",
            bst_predecessor(NULL) == NULL);
  mu_assert("error, successor(NULL) should be NULL",
            bst_successor(NULL) == NULL);

  bst_destroy(&b);
  return NULL;
}

static char *test_delete_leaf() {
  bst b;
  bst_create(&b);
  int values[] = {20, 10, 30, 25};
  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    bst_insert(&b, values[i]);

  bst_delete(&b, bst_search(&b, 25));

  mu_assert("error, deleted leaf still present", bst_search(&b, 25) == NULL);
  mu_assert("error, size should be 3", bst_size(&b) == 3);
  mu_assert_tree(&b);
  bst_destroy(&b);
  return NULL;
}

static char *test_delete_node_with_one_child() {
  bst b;
  bst_create(&b);
  int values[] = {20, 10, 30, 25, 27};
  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    bst_insert(&b, values[i]);

  bst_delete(&b, bst_search(&b, 25));

  mu_assert("error, deleted node still present", bst_search(&b, 25) == NULL);
  mu_assert("error, child should replace deleted node",
            b.root->right->left != NULL && b.root->right->left->key == 27);
  mu_assert("error, size should be 4", bst_size(&b) == 4);
  mu_assert_tree(&b);
  bst_destroy(&b);
  return NULL;
}

static char *test_delete_node_with_two_children() {
  bst b;
  bst_create(&b);
  int values[] = {20, 10, 30, 25, 35, 23, 27};
  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    bst_insert(&b, values[i]);

  bst_delete(&b, bst_search(&b, 30));

  mu_assert("error, deleted node still present", bst_search(&b, 30) == NULL);
  mu_assert("error, size should be 6", bst_size(&b) == 6);
  mu_assert("error, root right should be successor 35 or 23/25 path valid",
            b.root->right != NULL);
  mu_assert_tree(&b);
  bst_destroy(&b);
  return NULL;
}

static char *test_delete_root_cases() {
  bst b;
  bst_create(&b);

  bst_insert(&b, 8);
  bst_delete(&b, bst_search(&b, 8));
  mu_assert("error, root delete single node should empty tree", b.root == NULL);

  bst_insert(&b, 8);
  bst_insert(&b, 4);
  bst_delete(&b, bst_search(&b, 8));
  mu_assert("error, root should become 4", b.root != NULL && b.root->key == 4);

  bst_insert(&b, 10);
  bst_insert(&b, 9);
  bst_delete(&b, bst_search(&b, 4));
  mu_assert("error, root should exist after delete", b.root != NULL);
  mu_assert_tree(&b);

  bst_destroy(&b);
  return NULL;
}

static char *test_delete_all_nodes_sequence() {
  bst b;
  bst_create(&b);
  int values[] = {50, 30, 70, 20, 40, 60, 80};
  for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    bst_insert(&b, values[i]);

  int remove_order[] = {20, 30, 70, 50, 40, 60, 80};
  for (size_t i = 0; i < sizeof(remove_order) / sizeof(remove_order[0]); ++i) {
    bst_node *node = bst_search(&b, remove_order[i]);
    bst_delete(&b, node);
    mu_assert("error, size mismatch during full delete",
              bst_size(&b) ==
                  (sizeof(remove_order) / sizeof(remove_order[0]) - i - 1));
    mu_assert_tree(&b);
  }

  mu_assert("error, tree should be empty", b.root == NULL);
  mu_assert("error, height should be 0", bst_height(&b) == 0);
  bst_destroy(&b);
  return NULL;
}

static char *test_delete_null_cases() {
  bst b;
  bst_create(&b);
  bst_insert(&b, 1);

  bst_delete(NULL, bst_search(&b, 1));
  bst_delete(&b, NULL);

  mu_assert("error, node should still exist", bst_search(&b, 1) != NULL);
  bst_destroy(&b);
  return NULL;
}

static char *test_extreme_values() {
  bst b;
  bst_create(&b);

  bst_insert(&b, INT_MIN);
  bst_insert(&b, 0);
  bst_insert(&b, INT_MAX);

  mu_assert("error, min should be INT_MIN",
            bst_minimum(&b) != NULL && bst_minimum(&b)->key == INT_MIN);
  mu_assert("error, max should be INT_MAX",
            bst_maximum(&b) != NULL && bst_maximum(&b)->key == INT_MAX);
  mu_assert("error, predecessor(0) should be INT_MIN",
            bst_predecessor(bst_search(&b, 0)) != NULL &&
                bst_predecessor(bst_search(&b, 0))->key == INT_MIN);
  mu_assert("error, successor(0) should be INT_MAX",
            bst_successor(bst_search(&b, 0)) != NULL &&
                bst_successor(bst_search(&b, 0))->key == INT_MAX);

  bst_destroy(&b);
  return NULL;
}

static char *test_destroy_is_idempotent() {
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

char *(*tests_functions[])() = {
    test_bst_null,
    test_bst_empty,
    test_bst_insert_and_search,
    test_bst_insert_duplicate_policy,
    test_minimum_maximum_general,
    test_predecessor_successor_middle_nodes,
    test_predecessor_successor_extremes,
    test_delete_leaf,
    test_delete_node_with_one_child,
    test_delete_node_with_two_children,
    test_delete_root_cases,
    test_delete_all_nodes_sequence,
    test_delete_null_cases,
    test_extreme_values,
    test_destroy_is_idempotent,
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
