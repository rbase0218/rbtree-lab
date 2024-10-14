#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void) {
  rbtree* p = (rbtree*)calloc(1, sizeof(rbtree));
  if(p == NULL) return NULL;
  
  p->nil = (node_t*)calloc(1, sizeof(node_t));
  if(p->nil == NULL) { free(p); return NULL; }

  // NIL 노드 데이터 설정
  p->nil->color = RBTREE_BLACK;
  p->nil->parent = p->nil->left = p->nil->right = p->nil;

  // Root는 곧 NILL
  p->root = p->nil;
  return p;
}

// Delete
// Tree 안의 모든 노드를 삭제하기.
void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}

// Rotate -> 2024.10.13
void left_rotate(rbtree* t, node_t* node)
{
  node_t* y = node->right;
  node->right = y->left;
  
  if(y->left != t->nil)
    y->left->parent = node;
  
  y->parent = node->parent;
  
  if(node->parent == t->nil)          t->root = y;
  else if(node == node->parent->left) node->parent->left = y;
  else                                node->parent->right = y;
  
  y->left = node;
  node->parent = y;
}
void right_rotate(rbtree* t, node_t* node)
{
  node_t* y = node->left;
  node->left = y->right;
  
  if(y->right != t->nil)
    y->right->parent = node;
  
  y->parent = node->parent;
  
  if(node->parent == t->nil)            t->root = y;
  else if(node == node->parent->right)  node->parent->right = y;
  else                                  node->parent->left = y;
  
  y->right = node;
  node->parent = y;
}

// Insert -> 2024.10.14
void rb_insert_fixup(rbtree *t, node_t *z)
{
  node_t* uncle;
  while(z->parent->color == RBTREE_RED)
  {
    if(z->parent->parent->left == z->parent)
    {
      uncle = z->parent->parent->right;
      if(uncle->color == RBTREE_RED)
      {
        // CASE 1
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;

        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else  // Uncle의 Color가 Black인 경우.
      {
        // CASE 2
        if(z == z->parent->right)
        {
          z = z->parent;
          left_rotate(t, z);
        }
        // CASE 3
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }
    else
    {
      uncle = z->parent->parent->left;
      if(uncle->color == RBTREE_RED)
      {
        // CASE 1
        uncle->color = RBTREE_BLACK;
        z->parent->color = RBTREE_BLACK;

        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else  // Uncle의 Color가 Black인 경우.
      {
        // CASE 2
        if(z == z->parent->left)
        {
          z = z->parent;
          right_rotate(t, z);
        }
        // CASE 3
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}
node_t *rbtree_insert(rbtree *t, const key_t key)
{
  node_t *searcher = t->root;
  node_t *parent = t->nil;

  while(searcher != t->nil)
  {
    parent = searcher;

    if(searcher->key > key) searcher = searcher->left;
    else                    searcher = searcher->right;
  }

  node_t *new_node = (node_t*)calloc(1, sizeof(node_t));
  new_node->key = key;
  new_node->left = new_node->right = new_node->parent = t->nil;
  new_node->color = RBTREE_RED;
  
  // 1. Parent가 NULL이다.
  // 2. Parent Key보다 New Key가 더 작다.
  // 3. 이외
  if(parent == t->nil)
    t->root = new_node;
  else if(parent->key > new_node->key)
  {
    new_node->parent = parent;
    parent->left = new_node;
  }
  else
  {
    new_node->parent = parent;
    parent->right = new_node;
  }
  
  rb_insert_fixup(t, new_node);
  return t->root;
}

// Find -> 2024.10.14
node_t *rbtree_find(const rbtree *t, const key_t key)
{
  if(t == NULL) return NULL;
  node_t* searcher = t->root;
  while(searcher != t->nil && searcher->key != key)
    searcher = (searcher->key < key) ? searcher->right : searcher->left;
  
  return (searcher == t->nil) ? NULL : searcher;
}

// Min Num -> 2024.10.14
node_t *rbtree_min(const rbtree *t)
{
  if(t == NULL) return NULL;
  node_t* searcher = t->root;
  while(searcher->left != t->nil)
      searcher = searcher->left;
  
  return searcher;
}

// Max Num -> 2024.10.14
node_t *rbtree_max(const rbtree *t)
{
  if(t == NULL) return NULL;
  node_t* searcher = t->root;
  while(searcher->right != t->nil)
    searcher = searcher->right;

  return searcher;
}

// Successor -> 2024.10.14
node_t *rbtree_find_successor(const rbtree *t, node_t *target)
{
  if(t == NULL) return NULL;
  node_t* searcher = target->right;
  while(searcher->left != t->nil)
    searcher = searcher->left;
  return searcher;
}

// Transplant -> 2024.10.14
void rb_transplant(rbtree *t, node_t *u, node_t *v)
{
  if(u->parent == t->nil)       t->root = v;
  else if(u == u->parent->left) u->parent->left = v;
  else                          u->parent->right = v;
  v->parent = u->parent;
}

// Erase -> 2024.10.14
void rb_erase_fixup(rbtree *t, node_t *x)
{
  while(x!=t->root && x->color == RBTREE_BLACK){
    if (x== x->parent->left){
      node_t* w = x->parent->right;

      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      
      if (w->left->color == RBTREE_BLACK && w->right->color ==RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else{
        if (w->right->color == RBTREE_BLACK){
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }

        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    else{
      node_t* w = x->parent->left;

      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }

      if (w->left->color == RBTREE_BLACK && w->right->color ==RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else{
        if (w->left->color == RBTREE_BLACK){
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }

  x->color = RBTREE_BLACK;
}
int rbtree_erase(rbtree *t, node_t *p)
{
  node_t* y = p;
  color_t origin_color = p->color;

  node_t* temp;
  // 자식이 1개
  if(p->left == t->nil)
  {
    temp = p->right;
    rb_transplant(t,p, p->right);
  }
  else if(p->right == t->nil)
  {
    temp = p->left;
    rb_transplant(t, p, p->left);
  }
  else  // 자식이 2개
  {
    y = rbtree_find_successor(t, p);
    origin_color = y->color;
    temp = y->right;

    if(y != p->right)
    {
      rb_transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    else
      temp->parent = y;

    rb_transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }

  if(origin_color == RBTREE_BLACK)
    rb_erase_fixup(t, temp);
  
  free(p);
  return 0;
}

// TreeToArray -> 2024.10.14
key_t *inorder(const rbtree *t, node_t *p, key_t *arr)
{
  if (p == t->nil)
    return arr;

  arr = inorder(t, p->left, arr);
  
  *arr = p->key;
  arr += 1;

  arr = inorder(t, p->right, arr);

  return arr;
}
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  inorder(t, t->root, arr);
  return 0;
}
