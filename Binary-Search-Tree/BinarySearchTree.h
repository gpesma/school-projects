/*
 * BinarySearchTree.h
 * interface of a Binary Search Tree
 * Created on: Oct 13, 2014
 */

#ifndef BINARYSEARCHTREE_H_
#define BINARYSEARCHTREE_H_

#include <iostream>

/* Binary Search Tree Node:
 * - data is the value
 * - count is the number of times the data has been inserted
 *   into the tree (minus removals)
 */
struct Node {
        int   data;
        int   count; /* 1 means 1 time */
        Node *left;
        Node *right;
};

// defined in pretty_print.cpp
void printPretty(Node *root, int level, int indentSpace, std::ostream &out);

class BinarySearchTree {
public:

        BinarySearchTree();
        ~BinarySearchTree();
        BinarySearchTree(const BinarySearchTree &source);
        BinarySearchTree &operator= (const BinarySearchTree &source);

        int find_min() const;

        int find_max() const;

        /* return true if value is in the tree */
        bool contains(int value) const;

        /*
         * inserts the value into the Binary Search Tree.
         * If the value is already in the tree, the node's count
         * is incremented by 1
         */
        void insert(int value);

        /*
         *  If a node's count is greater than one, the count is
         * decremented, and the node is not removed.
         * Else fully removes the node
         */
        bool remove(int value);

        /*
         * returns the maximum distance from the root to it's farthest
         * leaf.  Note:  An empty tree has a height of -1, a tree with
         * just a root has a height of 0, and a tree with a root and
         * one or two children has a height of 1, etc.
         */
        int tree_height() const;

        /* returns the total number of nodes (NOT including duplicates) */
        int node_count() const;

        /* return the sum of all the node values (including duplicates)*/
        int count_total() const;

        // print the tree (not written by author)
        void print_tree() const;

private:
        /* the root of the tree (starts as NULL) */
        Node *root;

        Node *find_min(Node *node) const;
        Node *find_max(Node *node) const;
        bool  contains(Node *node, int value) const;
        void  insert(Node *node,Node *parent, int value);
        bool  remove(Node *node, Node *parent, int value);
        int   tree_height(Node *node) const;
        int   node_count(Node *node) const;
        int   count_total(Node *node) const;
        Node *pre_order_copy(Node *node) const;
	bool remove_helper(Node *node, Node *parent);

        /* used by the destructor to delete nodes recursively */
        void  post_order_delete(Node *node);

        Node *find_parent(Node *node, Node *child) const;
};


#endif /* BINARYSEARCHTREE_H_ */
