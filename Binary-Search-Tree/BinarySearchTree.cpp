/*
 * BinarySearchTree.cpp
 * Georgios Pesmazoglou
 * Implementation of BinarySearchTree.h
 */

#include <iostream>
#include <climits>
#include <fstream>
#include <sstream>

#include "BinarySearchTree.h"

using namespace std;

/* Sets root to NULL */
BinarySearchTree::BinarySearchTree()
{
	root = NULL;
}

BinarySearchTree::~BinarySearchTree()
{
    /* walk tree in post-order traversal and delete */
    post_order_delete(root);
}

/* uses post order tranversal to delete all nodes */
void BinarySearchTree::post_order_delete(Node *node)
{
	if(node == NULL) return;
	post_order_delete(node->left);
	post_order_delete(node->right);
	delete node;
}

/* copy constructor */
BinarySearchTree::BinarySearchTree(const BinarySearchTree &source)
{
    root = pre_order_copy(source.root);
}

/*
 * assignment overload
 * check for self-assignment
 * delete current tree if it exists
 * use pre-order traversal to copy the tree
 */
BinarySearchTree &BinarySearchTree::operator= (const BinarySearchTree &source)
{
	if(this != &source){
		post_order_delete(root);
		root = pre_order_copy(source.root);
	}
	return *this;
}

Node *BinarySearchTree::pre_order_copy(Node *node) const
{
    if (node == NULL)  return NULL;
        
    Node *new_node = new Node();
	new_node->data = node->data;
	new_node->count = node->count;
	
	new_node->left = pre_order_copy(node->left);
	new_node->right= pre_order_copy(node->right);
	
	return new_node;
}

int BinarySearchTree::find_min() const
{
        if (root == NULL)
                return INT_MIN;
        return find_min(root)->data;
}

Node *BinarySearchTree::find_min(Node *node) const
{
	if(node->left == NULL) return node;
	return find_min(node->left);
}

int BinarySearchTree::find_max() const
{
	if(root == NULL) return INT_MAX;
	return find_max(root)->data;	
}

Node *BinarySearchTree::find_max(Node *node) const
{
	if(node->right == NULL) return node;
	return find_max(node->right);
}


bool BinarySearchTree::contains(int value) const
{
	if(root == NULL) return false;
	return contains(root,value);
}

bool BinarySearchTree::contains(Node *node, int value) const
{
        // TODO: Students write code here
	if(node == NULL) return false;
	if(node->data == value) return true;
	if(node->data > value) return contains(node->left, value);
	if(node->data < value) return contains(node->right, value);
	
	return false;
}

void BinarySearchTree::insert(int value)
{
        insert(root, NULL, value);
}

/*
 * if tree is empty inserts node with the value and makes the root point
 *  at empty children
 * if value already exists increments count
 * else creates a new node which holds the new value
 */
void BinarySearchTree::insert(Node *node, Node *parent, int value)
{
	if(root ==NULL){
		Node *n_node = new Node;
		n_node->data = value;
		n_node->count = 1;
		n_node->left = NULL;
		n_node->right =NULL;
		root = n_node;
		return;
	}
	if(node ==NULL){
		Node *n_node = new Node();
		n_node->data = value;
		n_node->count = 1;
		n_node->left = NULL;
		n_node->right =NULL;
		node = n_node;
	}else if(node->data == value){
		node->count++;
		return;
	}
	
	if(node->data > value) insert(node->left, node,value);
	if(node->data < value) insert(node->right, node, value);
	
	if(parent != NULL){
		if(parent->data > node->data) parent->left = node;
		if(parent->data < node->data) parent->right = node;
	}
}

bool BinarySearchTree::remove(int value)
{
        return remove(root, NULL, value);
}

/*
 * if node is null returns false
 * if count > 1 decrements
 * if node has no children deletes node_count
 * if node has one child deletes child and the parent of that node
 * points at its child
 * if node has two children replaces the value with the smalest of
 * the right subtree and deletes that node
 */
bool BinarySearchTree::remove(Node *node, Node *parent, int value)
{	
	if (node == NULL) return false;

	if((node->data) > value) 
		return remove(node->left, node, value);
	if ((node->data) < value)
		return remove(node->right,node,value);
	
	
	if (node->data == value){
		if(node->count > 1){
			node->count--;
			return true;
		}
		if(node->left == NULL && node->right== NULL){
			if(parent != NULL){
				if(node->data < parent->data) 
					parent->left = NULL;
				else
					parent->right = NULL;
			} else root = NULL;
			delete node;
			return true;
		}else if (node->left == NULL || node->right== NULL){
			return remove_helper(node,parent);
		} else{
			Node *temp;
			temp = find_min(node->right);
			int replace_data = temp->data;
			int replace_count = temp->count;
			for(int i = 0; i < replace_count; i++){
				remove(node,parent,temp->data);
			}
			node->data = replace_data;
			node->count = replace_count;
			return true;
		}
	} 
	return false;
}

/*
 * helper function for removing a node with a child
 */
bool BinarySearchTree::remove_helper(Node *node, Node *parent)
{
	if(node->left == NULL){
		if(parent == NULL){
			root = node->right;
			delete node;
			return true;
		}
		if(parent->data > node->data){
			parent->left = node->right;
			delete node;
			return true;
		}
		if(parent->data < node->data){
			parent->right = node->right;
			delete node;
			return true;
		}
	} else if (node->right == NULL){
		if(parent == NULL){
			root = node->left;
			delete node;
			return true;
		}
		if(parent->data > node->data){
			parent->left = node->left;
			delete node;
			return true;
		}
		if(parent->data < node->data){
			parent->right = node->left;
			delete node;
			return true;
		}
	}
	return false;
}


int BinarySearchTree::tree_height() const
{
        return tree_height(root);
}

int BinarySearchTree::tree_height(Node *node) const
{
	if(node == NULL) return -1;
	else {
		int left_t = tree_height(node->left);
		int right_t = tree_height(node->right);
		
		if(left_t > right_t) return (left_t) + 1;
		else return right_t +1;
	}	
}

int BinarySearchTree::node_count() const
{
        return node_count(root);
}

int BinarySearchTree::node_count(Node *node) const
{
	int count = 0;
	if(node!= NULL) 
		count = 1+ node_count(node->left) + node_count(node->right);
	return count;
}

int BinarySearchTree::count_total() const
{
        return count_total(root);
}

int BinarySearchTree::count_total(Node *node) const
{
	if(node == NULL) return 0;
	return (node->count*node->data) + count_total(node->left) +count_total(node->right); 
}

void BinarySearchTree::print_tree() const
{
        printPretty(root, 1, 0, std::cout);
}

Node *BinarySearchTree::find_parent(Node *node, Node *child) const
{
    if (node == NULL) return NULL;

    if (node->left==child or node->right == child) {
        return node;
    }
     
    if (child->data > node->data) {
        return find_parent(node->right, child);
    } else {
        return find_parent(node->left, child);
    }
}
