
#ifndef _syntax_tree_h
#define _syntax_tree_h
#include "token.h"

typedef enum { BINARY_OP, UNARY_OP, NO_OP } OpType;
typedef TokenType NodeType;
typedef struct NodeImplementation Node;

// TODO: Getter and setter functions
Node* first_child(Node* n);
Node* second_child(Node* n);
int get_val(Node* n);
void set_val(Node* n, int v);
OpType op_type(Node* n);
void set_op_type(Node* n, OpType t);
NodeType node_type(Node* n);
void set_node_type(Node* n, NodeType t);



/* Function: node_create
 * Returns a pointer to a dynamically-allocated heap block
 * representing a node of an abstract syntax tree (AST).
 * This function is a client of the Token functionalities
 * in that it derives its information from the corresponding
 * Token provided by the lexical analyzer. Does not initialize
 * any children. Does not free memory allocated for the Token.
 */
Node* node_create(Token* input_token);

/* Function: create_node_from_type
 * Given a NodeType, creates a new Node of that type. If the
 * given type is NUMBER, does not initialize the value of the Node.
 */
Node* create_node_from_type(NodeType type);
 
/* Function: ast_dispose
 * Frees all heap blocks used by the AST whose root is the given
 * Node, to be called when the client no longer needs to use that AST.
 * If called on a subtree of an AST, the parent of its root will still
 * have that Node as a child. This function does not change that parent's
 * bookkeeping at all.
 */
void ast_dispose(Node* n);

/* Function: ast_deep_copy
 * Given the root of an AST, returns the root of a new AST that is a 
 * deep copy of the given AST.
 */
Node* ast_deep_copy(Node* root);

/* Function: children_dispose
 * Disposes of all subtrees stemming from the given node.
 */
void children_dispose(Node* n);

/* Function: connect_unary_op
 * Given a node that represents a unary operation, along with
 * a node that is the root of an AST, sets the latter node to 
 * be the child of the first node.
 * Precondition: The new_root must represent a unary operation.
 */
void connect_unary_op(Node* new_root, Node* child);

/* Function: merge_subtrees
 * Given a node that represents a binary operation, along with
 * two nodes that are each roots of an AST, sets the latter two nodes
 * to be the two children of the first node.
 * Precondition: The new_root must represent a binary operation.
 */
void merge_subtrees(Node* new_root, Node* left_child, Node* right_child);

/* Function: subtree_is_constant
 * Determines if an AST with given root has constant value, 
 * i.e. none of the leaf nodes have type VARIABLE. 
 */
bool subtree_is_constant(Node* root);

/* Function: simplify
 * Given the root of an AST, returns the root of an in-place
 * simplification.
 */
Node* simplify(Node* root);

/* Function: eval
 * Given an AST and a value to substitute in for the variable,
 * returns the result of the expression represented by the AST,
 * evaluated at that value. Precondition: AST is well-formed.
 */
int eval(Node* root, int val);
 
 /* Function: print_ast
  * Given an AST, prints it in human-readable form, e.g.,
  * +(^(x, 2), *(5, x)) for the AST representing x^2 + 5*x.
  * Operates recursively, with the base case being that the root
  * is a number or variable, i.e., a leaf of the AST it belongs to.
  */
void print_ast(Node* root);

/* Function: negation_node_create
 * Creates a Node of type NEGATION and operation type UNARY_OP.
 */
Node* negation_node_create();

#endif
 