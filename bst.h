#ifndef BST_H
#define BST_H

#include <stddef.h>

typedef struct sbst_node {
  int key;
  struct sbst_node *left;
  struct sbst_node *right;
  struct sbst_node *parent;
} bst_node;

typedef struct {
  bst_node *root;
} bst;

/*
 * Create an empty bst
 */
void bst_create(bst *self);

/*
 * Destroy a bst
 */
void bst_destroy(bst *self);

/*
 * Get the size of the tree
 */
size_t bst_size(const bst *self);

/*
 * Get the height of the tree
 */
size_t bst_height(const bst *self);

/*
 * Walk in the tree in inorder and print each value
 */
void inorder_bst_walk(const bst *self);

/*
 * Find the node containing a value in the sub-tree rooted in self
 */
bst_node *bst_search(const bst *self, int value);

/*
 * Insert a value in the tree
 */
bst_node *bst_insert(bst *self, int value);

/*
 * Get minimum node in tree
 */
bst_node *bst_minimum(const bst *self);

/*
 * Get maximum node in tree
 */
bst_node *bst_maximum(const bst *self);

/*
 * Get predecessor of a given node
 */
bst_node *bst_predecessor(bst_node *self);

/*
 * Get successor of a given node
 */
bst_node *bst_successor(bst_node *self);

/*
 * Delete a node from tree
 */
void bst_delete(bst *self, bst_node *node);

#endif // BST_H
