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

// RB 트리를 할당 해제할 것.
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
        uncle->color = z->parent->color = RBTREE_BLACK;
        z->parent->parent = RBTREE_RED;
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
        uncle->color = z->parent->color = RBTREE_BLACK;
        z->parent->parent = RBTREE_RED;
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
}
node_t *rbtree_insert(rbtree *t, const key_t key)
{
  if(t == NULL || key == NULL) return NULL;

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
  if(t == NULL) return;
  node_t* searcher = t->root;
  while(searcher != t->nil && searcher->key != key)
    searcher = (searcher->key < key) ? searcher->left : searcher->right;
  
  return (searcher == t->nil) ? NULL : searcher;
}

// RB트리에서 가장 작은 수
node_t *rbtree_min(const rbtree *t)
{
}

// RB트리에서 가장 큰 수
node_t *rbtree_max(const rbtree *t)
{
}

node_t *rbtree_find_successor(const rbtree *t, node_t *target)
{
}

void rb_transplant(rbtree *t, node_t *u, node_t *v)
{
}

void rb_erase_fixup(rbtree *t, node_t *p)
{
}

// 트리 T에서 P에 해당하는 NODE를 제거해라.
int rbtree_erase(rbtree *t, node_t *p)
{
}

// RB트리를 배열로 만들.....어? 뭔데 이거
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  // TODO: implement to_array
  return 0;
}
