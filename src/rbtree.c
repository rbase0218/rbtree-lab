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

  // RB 트리의 요소를 순회하면서 최하단을 기점으로
  // 동적 메모리 할당을 진행한다.
  // Post Order를 사용하는 것이 Sibling 노드를 처리할 때 적절할 듯?

  free(t);
}

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

void insert_fixup(rbtree* t, node_t *z){

  while (z->parent->color == RBTREE_RED){ // insert_fixup에서 에러가 났다.
    // z is left child
    if(z->parent == z->parent->parent->left){
      node_t* y = z->parent->parent->right; // uncle
      
      // case 1) uncle(red)
      if (y->color == RBTREE_RED){
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else{
        // case 2) uncle(black) + triangle
        if(z==z->parent->right){
          z = z->parent;
          left_rotation(t, z);
        }
        // case 3) uncle(black) + linear
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotation(t, z->parent->parent);
      } 
    }

    // z is right child
    else{
      node_t *y = z->parent->parent->left; // uncle

      // case 1) uncle(red)
      if (y->color == RBTREE_RED){
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else{
        // case 2) uncle(black) + triangle
        if(z==z->parent->left){
          z = z->parent;
          right_rotation(t, z);
        }

        // case 3) uncle(black) + linear
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotation(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}
// RB트리에 삽입해라
node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *y = t->nil;
  node_t *x = t->root;

  // 삽입할 노드 생성
  node_t* node = (node_t*)calloc(1, sizeof(node_t));
  node->key = key;
  node->left = t->nil;
  node->right = t->nil;
  node->color = RBTREE_RED;

  while (x != t->nil){
    y = x;
    x = (key < x->key) ? x->left : x->right;
  }

  node->parent = y;
  if(y==t->nil){
    t->root = node;
    node->color = RBTREE_BLACK;
  }
  else if(key<y->key)
    y->left = node;
  else
    y->right = node;

  insert_fixup(t, node);

  return t->root;
}

// RB트리에서 찾아라
node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *current = t->root;
  while (current != t->nil)
  {
    if (key == current->key)
    {
      return current;
    }
    else if (key < current->key)
    {
      current = current->left;
    }
    else
    {
      current = current->right;
    }
  }

  return NULL; // 키를 찾지 못했을 때
}

// RB트리에서 가장 작은 수
node_t *rbtree_min(const rbtree *t) {
  node_t* min_node = t->root;

  while(min_node->left != t->nil)
    min_node = min_node->left;
  return min_node;
}

// RB트리에서 가장 큰 수
node_t *rbtree_max(const rbtree *t) {
  node_t* max_node = t->root;
  
  while(max_node->right != t->nil)
    max_node = max_node->right;
  return max_node;
}

node_t *rbtree_find_successor(const rbtree *t, node_t *target)
{
  node_t* successor_node = target->right;
  while(successor_node->left != t->nil)
    successor_node = successor_node->left;

  return successor_node;
}

void rb_transplant(rbtree *t, node_t *u, node_t *v)
{
  if(u->parent == t->nil)
    t->root = v;
  else if(u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;
  v->parent = u->parent;
}

void rb_erase_fixup(rbtree *t, node_t *p) {
  node_t *brother;
  while(p != t->root && p->color == RBTREE_BLACK)
  {
    if(p == p->parent->left)
    {
      brother = p->parent->right;
      if(brother->color == RBTREE_RED)
      {
        brother->color = RBTREE_BLACK;
        p->parent->color = RBTREE_RED;
        left_rotate(t, p->parent);
        brother = p->parent->right;
      }

      if(brother->left->color == RBTREE_BLACK && brother->right->color == RBTREE_BLACK)
      {
        brother->color = RBTREE_RED;
        p = p->parent;
      }
      else
      {
        if(brother->right->color == RBTREE_BLACK)
        {
          brother->left->color = RBTREE_BLACK;
          brother->color = RBTREE_RED;

          right_rotate(t, brother);
          brother = p->parent->right;
        }

        brother->color = p->parent->color;
        p->parent->color = RBTREE_BLACK;
        brother->right->color = RBTREE_BLACK;
        left_rotate(t, p->parent);
        p = t->root;
      }
    }
    else
    {
      brother = p->parent->left;

      if(brother->color == RBTREE_RED)
      {
        brother->color = RBTREE_BLACK;
        p->parent->color = RBTREE_RED;
        right_rotate(t, p->parent);
        brother = p->parent->left;
      }

      if(brother->right->color == RBTREE_BLACK && brother->left->color == RBTREE_BLACK)
      {
        brother->color = RBTREE_RED;
        p = p->parent;
      }
      else
      {
        if(brother->left->color == RBTREE_BLACK)
        {
          brother->right->color = RBTREE_BLACK;
          brother->color = RBTREE_RED;

          left_rotate (t, brother);
          brother = p->parent->left;
        }

        brother->color = p->parent->color;
        p->parent->color = RBTREE_BLACK;
        brother->left->color = RBTREE_BLACK;
        right_rotate(t, p->parent);
        p = t->root;
      }
    }
  }
}
// 트리 T에서 P에 해당하는 NODE를 제거해라.
int rbtree_erase(rbtree *t, node_t *p) {
    node_t *child, *temp = p;
    color_t temp_color = p->color;

    if (p->left == t->nil) {
        child = p->right;
        rb_transplant(t, p, p->right);
    } else if (p->right == t->nil) {
        child = p->left;
        rb_transplant(t, p, p->left);
    } else {
        temp = rbtree_find_successor(t, p);
        temp_color = temp->color;
        child = temp->right;

        if (temp->parent == p) {
            child->parent = temp;
        } else {
            rb_transplant(t, temp, temp->right);
            temp->right = p->right;
            temp->right->parent = temp;
        }

        rb_transplant(t, p, temp);
        temp->left = p->left;
        temp->left->parent = temp;
        temp->color = p->color;
    }

    if (temp_color == RBTREE_BLACK)
        rb_erase_fixup(t, child);

    free(p);
    return 0;
}
// RB트리를 배열로 만들.....어? 뭔데 이거
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
