/*
 * main.cpp
 *
 * Georgios Pesmazoglou
 * Simple test of BinarySearchTree
 * 
 */

#include <iostream>
#include "BinarySearchTree.h"

using namespace std;

void print_tree_details(BinarySearchTree &bst) {
        bst.print_tree();
        cout << "\n";
        cout << "min: " << bst.find_min() << "\n";
        cout << "max: " << bst.find_max() << "\n";
        cout << "nodes: " << bst.node_count() << "\n";
        cout << "count total: " << bst.count_total() << "\n";
        cout << "tree height: " << bst.tree_height() << "\n";
        cout << "\n";
}

int main() {

        BinarySearchTree bst;

        bst.insert(12);
        bst.print_tree();
        cout << "Removing 12.\n";
        bst.remove(12);
        bst.print_tree();

        return 0;       
}
