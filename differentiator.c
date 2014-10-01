#include "syntax-tree.h"
#include "token.h"
#include <stdlib.h>
#include <stdio.h>

/* Differentiation rules used in diff():
 * 1. D(x) = 1
 * 2. D(c) = 0
 * 3. D(f(x) +/- g(x)) = D(f(x)) +/- D(g(x))
 * 4. D(c*f(x)) = c * D(f(x))
 */


Node* diff(Node* root) {
	NodeType type = node_type(root);
	Node* c_1 = first_child(root);
	Node* c_2 = second_child(root);
	
	if (type == VARIABLE) { // D(x) = 1
		set_node_type(root, NUMBER);
		set_val(root, 1);
		return root;
	}
	
	else if (type == NUMBER) { // D(x) = 0
		set_val(root, 0);
		return root;
	}
	
	else if (type == PLUS || type == MINUS) { // D(f +/- g) = D(f) +/- D(g)
		merge_subtrees(root, diff(c_1), diff(c_2));
		return root;
	}
	
	else if (type == NEGATION) {
		connect_unary_op(root, diff(c_1));
		return root;
	}
	
	else if (type == TIMES) {
		bool first = subtree_is_constant(c_1);
		bool second = subtree_is_constant(c_2);
		if (first && !second) {
			merge_subtrees(root, c_1, diff(c_2));
			return root;
		} else if (!first && second) {
			merge_subtrees(root, diff(c_1), c_2);
			return root;
		} else if (first && second) {
			// Change the root node to a terminal NUMBER node of value 0.
			children_dispose(root);
			set_op_type(root, NO_OP);
			set_node_type(root, NUMBER);
			set_val(root, 0);
			return root;
		} else { // Traditional product rule
			printf("Product rule!\n");
			Node* c_2_copy = ast_deep_copy(c_2);
			diff(c_2);
			print_ast(c_2);
			Node* plus_node = create_node_from_type(PLUS);
			Node* times_node = create_node_from_type(TIMES);
			Node* c_1_prime = diff(ast_deep_copy(c_1));
			print_ast(c_1_prime);
			printf("Hi hello\n");
			print_ast(c_2_copy);
			merge_subtrees(times_node, c_1_prime, c_2_copy);
			merge_subtrees(plus_node, times_node, root);
			printf("Done!\n");
			return plus_node;
		}
	}
	
	else if (type == POWER) {
		bool first = subtree_is_constant(c_1);
		bool second = subtree_is_constant(c_2);
		if (first && !second) {
			// coming soon... (this requires logarithms)
			return NULL;
		} else if (!first && second) {
			
		}
	}
	
	return NULL;
}