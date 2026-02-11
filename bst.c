#include "bst.h"

#include <stdio.h>
#include <stdlib.h>

void bst_create(bst *self) {
  if (self == NULL) {
    return;
  }
  self->root = NULL;
}

static void bst_destroy_node(bst_node *node) {
  if (node == NULL) {
    return;
  }
  bst_destroy_node(node->left);
  bst_destroy_node(node->right);
  free(node);
}

void bst_destroy(bst *self) {
  if (self == NULL) {
    return;
  }
  bst_destroy_node(self->root);
  self->root = NULL;
}

static size_t bst_node_size(const bst_node *node) {
  if (node == NULL) {
    return 0;
  }
  return 1 + bst_node_size(node->left) + bst_node_size(node->right);
}

size_t bst_size(const bst *self) {
  if (self == NULL) {
    return 0;
  }
  return bst_node_size(self->root);
}

static size_t node_height(const bst_node *node) {
  if (node == NULL) {
    return 0;
  }
  size_t hl = node_height(node->left);
  size_t hr = node_height(node->right);
  return (hl > hr ? hl : hr) + 1;
}

size_t bst_height(const bst *self) {
  if (self == NULL) {
    return 0;
  }
  return node_height(self->root);
}

static void inorder_walk(const bst_node *node) {
  if (node == NULL) {
    return;
  }
  inorder_walk(node->left);
  printf("%d ", node->key);
  inorder_walk(node->right);
}

void inorder_bst_walk(const bst *self) {
  if (self == NULL) {
    return;
  }
  inorder_walk(self->root);
  printf("\n");
}

bst_node *bst_search(const bst *self, int value) {
  if (self == NULL) {
    return NULL;
  }
  bst_node *cur = self->root;
  while (cur != NULL) {
    if (cur->key == value) {
      return cur;
    }
    if (value < cur->key) {
      cur = cur->left;
    } else {
      cur = cur->right;
    }
  }
  return NULL;
}

bst_node *bst_insert(bst *self, int value) {
  if (self == NULL) {
    return NULL;
  }

  bst_node *new_node = calloc(1, sizeof(*new_node));
  if (new_node == NULL) {
    return NULL;
  }
  new_node->key = value;

  if (self->root == NULL) {
    self->root = new_node;
    return new_node;
  }

  bst_node *parent = NULL;
  bst_node *cur = self->root;

  while (cur != NULL) {
    parent = cur;
    if (value < cur->key) {
      cur = cur->left;
    } else {
      cur = cur->right;
    }
  }

  if (value < parent->key) {
    parent->left = new_node;
  } else {
    parent->right = new_node;
  }
  new_node->parent = parent;

  return new_node;
}

static bst_node *bst_minimum_node(bst_node *node) {
  if (node == NULL) {
    return NULL;
  }
  while (node->left != NULL) {
    node = node->left;
  }
  return node;
}

static bst_node *bst_maximum_node(bst_node *node) {
  if (node == NULL) {
    return NULL;
  }
  while (node->right != NULL) {
    node = node->right;
  }
  return node;
}

bst_node *bst_minimum(const bst *self) {
  if (self == NULL) {
    return NULL;
  }
  return bst_minimum_node(self->root);
}

bst_node *bst_maximum(const bst *self) {
  if (self == NULL) {
    return NULL;
  }
  return bst_maximum_node(self->root);
}

bst_node *bst_predecessor(bst_node *self) {
  if (self == NULL) {
    return NULL;
  }

  if (self->left != NULL) {
    return bst_maximum_node(self->left);
  }

  bst_node *current = self;
  bst_node *ancestor = self->parent;

  while (ancestor != NULL && current == ancestor->left) {
    current = ancestor;
    ancestor = ancestor->parent;
  }

  return ancestor;
}

bst_node *bst_successor(bst_node *self) {
  if (self == NULL) {
    return NULL;
  }

  if (self->right != NULL) {
    return bst_minimum_node(self->right);
  }

  bst_node *current = self;
  bst_node *ancestor = self->parent;

  while (ancestor != NULL && current == ancestor->right) {
    current = ancestor;
    ancestor = ancestor->parent;
  }

  return ancestor;
}

static void bst_transplant(bst *tree, bst_node *u, bst_node *v) {
  if (u->parent == NULL) {
    tree->root = v;
  } else if (u == u->parent->left) {
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }

  if (v != NULL) {
    v->parent = u->parent;
  }
}

void bst_delete(bst *self, bst_node *node) {
  if (self == NULL || node == NULL) {
    return;
  }

  if (node->left == NULL) {
    bst_transplant(self, node, node->right);
    free(node);
    return;
  }

  if (node->right == NULL) {
    bst_transplant(self, node, node->left);
    free(node);
    return;
  }

  bst_node *y = bst_minimum_node(node->right);

  if (y->parent != node) {
    bst_transplant(self, y, y->right);
    y->right = node->right;
    if (y->right != NULL) {
      y->right->parent = y;
    }
  }

  bst_transplant(self, node, y);
  y->left = node->left;
  if (y->left != NULL) {
    y->left->parent = y;
  }

  free(node);
}
