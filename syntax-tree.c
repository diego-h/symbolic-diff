#include "syntax-tree.h"
#include "token.h"
#include <stdlib.h>
#include <stdio.h>

struct NodeImplementation {
	OpType op_type;
	NodeType node_type;
	Node* child_a;
	Node* child_b;
	int val;
};

Node* first_child(Node* n)              { return n->child_a; }
Node* second_child(Node* n)             { return n->child_b; }
int get_val(Node* n)                    { return n->val; }
void set_val(Node* n, int v)            { n->val = v; }
OpType op_type(Node* n)                 { return n->op_type; };
void set_op_type(Node* n, OpType t)     { n->op_type = t; };
NodeType node_type(Node* n)             { return n->node_type; }
void set_node_type(Node* n, NodeType t) { n->node_type = t; }


// Q: How does the NEGATION token come about?
//    Through lexer, or parser? (A: Through the parser.)

Node* node_create(Token* input_token) {
	printf("Creating node of type %s\n", token_type_str(input_token));
	Node* n = malloc(sizeof(Node));
	n->node_type = token_type(input_token);
	if      (n->node_type == NEGATION) n->op_type = UNARY_OP;
	else if (n->node_type == NUMBER) {
		n->op_type = NO_OP;
		sscanf(token_text(input_token), "%d", &(n->val));
	} else if (n->node_type == VARIABLE) n->op_type = NO_OP;
	else n->op_type = BINARY_OP;
	printf("This node is of type %d\n", n->op_type);
	return n;
}

void node_dispose(Node* n) {
	free(n);
}

Node* create_node_from_type(NodeType type) {
	Node* n = malloc(sizeof(Node));
	n->node_type = type;
	if      (n->node_type == NEGATION) n->op_type = UNARY_OP;
	else if (n->node_type == NUMBER)   n->op_type = NO_OP;
	else if (n->node_type == VARIABLE) n->op_type = NO_OP;
	else n->op_type = BINARY_OP;
	return n;
}

Node* negation_node_create() {
	printf("Creating a negation node\n");
	Node* n = malloc(sizeof(Node));
	n->node_type = NEGATION;
	n->op_type = UNARY_OP;
	return n;
}

void ast_dispose(Node* root) {
	if (root->op_type == NO_OP) { // if root has no children
		free(root);
		return;
	}
 	ast_dispose(first_child(root)); // root has at least one child
	if (root->op_type == BINARY_OP) ast_dispose(second_child(root));
}

Node* node_deep_copy(Node* orig) {
	printf("NDC\n");
	Node* copy = malloc(sizeof(Node));
	copy->op_type = orig->op_type;
	copy->node_type = orig->node_type;
	copy->child_a = orig->child_a;
	copy->child_b = orig->child_b;
	copy->val = orig->val;
	printf("NDC bye\n");
	return copy;
}

Node* ast_deep_copy(Node* root) {
	printf("ast deep copy called with node of type %d\n", root->op_type);
	Node* new_root = node_deep_copy(root);
	printf("HI!\n");
	if (new_root->op_type == UNARY_OP) {
		printf("HI again!\n");
		merge_subtrees(new_root, ast_deep_copy(first_child(root)), NULL);
	} else if (new_root->op_type == BINARY_OP) {
		printf("Binary op\n");
		merge_subtrees(new_root, ast_deep_copy(first_child(root)), ast_deep_copy(second_child(root)));
	}
	printf("Bye ADC!\n");
	return new_root;
}

void children_dispose(Node* n) {
	if (n->op_type == NO_OP) return;
	ast_dispose(first_child(n));
	if (n->op_type == BINARY_OP) ast_dispose(second_child(n));
}

void connect_unary_op(Node* new_root, Node* child) {
	new_root->child_a = child;
}

void merge_subtrees(Node* new_root, Node* left_child, Node* right_child) {
	new_root->child_a = left_child;
	new_root->child_b = right_child;
}

bool subtree_is_constant(Node* root) {
	if (root->node_type == NUMBER) return true;
	if (root->node_type == VARIABLE) return false;
	if (root->op_type == UNARY_OP) {
		return subtree_is_constant(first_child(root));
	}
	return subtree_is_constant(first_child(root)) &&
		   subtree_is_constant(second_child(root));
}

int power(int base, int exp) {
	int result = 1;
	for (int i = 0; i < exp; i++) {
		result *= base;
	}
	return result;
}

Node* simplify_constants(Node* root) {
	// Base case
	if (op_type(root) == NO_OP) {
		if (node_type(root) == NUMBER) return root;
		if (node_type(root) == VARIABLE) return NULL;
	}
	Node* c_1 = first_child(root);
	Node* c_2 = second_child(root);
	if (op_type(root) == UNARY_OP) {
		Node* simplified = simplify_constants(c_1);
		if (simplified != NULL) connect_unary_op(root, simplified);
		return root;
	}
	
	// Else: root is a binary op.	
	Node* simplified_1 = simplify_constants(c_1);
	Node* simplified_2 = simplify_constants(c_2);
	if (simplified_1 != NULL && simplified_2 != NULL) {
		merge_subtrees(root, simplified_1, simplified_2);
		set_val(root, eval(root, 0));
		children_dispose(root);
		set_op_type(root, NO_OP);
		set_node_type(root, NUMBER);
		return root;
	}
	return NULL;
}

bool is_number(Node* n, int val) {
	return node_type(n) == NUMBER && get_val(n) == val;
}

Node* simplify_identities(Node* root) {
	printf("simplify_identities called with root %p\n", root);
	Node* c_1 = first_child(root);
	Node* c_2 = second_child(root);
	printf("Children are %p and %p\n", c_1, c_2);
	if (op_type(root) == NO_OP) return root;
	if (node_type(root) == TIMES) {
		if (is_number(c_1, 1)) { // Multiplying by 1
			node_dispose(c_1);
			node_dispose(root);
			return c_2;
		} else if (is_number(c_2, 1)) {
			node_dispose(c_2);
			node_dispose(root);
			return c_1;
		} else if (is_number(c_1, 0) || is_number(c_2, 0)) { // Multiplying by 0
			children_dispose(root);
			set_op_type(root, NUMBER);
			set_node_type(root, NO_OP);
			set_val(root, 0);
			return root;
		}
	}
	return root;
}

Node* simplify(Node* root) {
	simplify_constants(root);
	return simplify_identities(root);
}

int eval(Node* root, int val) {
	if (root->node_type == NUMBER)   return root->val;
	if (root->node_type == VARIABLE) return val;
	
	// Otherwise, root is an operator.
	switch (root->node_type) {
		case PLUS:     return eval(first_child(root), val) + eval(second_child(root), val); break;
		case MINUS:    return eval(first_child(root), val) - eval(second_child(root), val); break;
		case TIMES:    return eval(first_child(root), val) * eval(second_child(root), val); break;
		case DIVIDE:   return eval(first_child(root), val) / eval(second_child(root), val); break;
		case POWER:    return power(eval(first_child(root), val), eval(second_child(root), val)); break;
		case NEGATION: return -eval(first_child(root), val); break;
		default: printf("Error in eval(): Unrecognized operator\n"); break;
	}
	
	return -1; // shouldn't happen
}

char* type_to_text(NodeType t) {
	switch (t) {
		case PLUS: return "+"; break;
		case MINUS: return "-"; break;
		case TIMES: return "*"; break;
		case DIVIDE: return "/"; break;
		case POWER: return "^"; break;
		case NEGATION: return "-"; break;
		default: return "?"; break; // shouldn't happen
	}
}

void print_ast_helper(Node* root) {
	// if root is a NUMBER or VARIABLE node, print its value/name
	if (root->node_type == NUMBER) {
		printf("%d", root->val);
		return;
	}
	if (root->node_type == VARIABLE) {
		printf("var");
		return;
	}
	// Otherwise, root is an operator P.
	// Print P(print_ast(child), ...)
	if (root->op_type == UNARY_OP) {
		printf("%s", type_to_text(node_type(root)));
	}
	printf("(");
	print_ast_helper(first_child(root));
	if (root->op_type == BINARY_OP) {
		printf("%s", type_to_text(node_type(root)));
		print_ast_helper(second_child(root));
	}
	printf(")");
}

void print_ast(Node* root) {
	print_ast_helper(root);
	printf("\n");
}