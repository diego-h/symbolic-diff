#include "syntax-tree.h"
#include "lexer.h"
#include "token.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
	if (argc < 3) {
		printf("Usage: %s \"IBIBI\" x\n", argv[0]);
		return 0;
	}
	
	Lexer* lex = lexer_create(argv[1]);
	Token* first = lexer_next(lex);
	Token* second = lexer_next(lex);
	Token* third = lexer_next(lex);
	Token* fourth = lexer_next(lex);
	Token* fifth = lexer_next(lex);
	
	Node* root = node_create(fourth);
	merge_subtrees(root, node_create(third), node_create(fifth));
	
	Node* root_2 = node_create(second);
	merge_subtrees(root_2, node_create(first), root);

	print_ast(root_2);
	printf("Evaluates to: %d\n", eval(root_2, argv[2][0] - '0'));
	
	printf("Subtree is %sa constant value\n", subtree_is_constant(root) ? "" : "not ");
	printf("Overall AST is %sa constant value\n", subtree_is_constant(root_2) ? "" : "not ");
	
	token_dispose(first);
	token_dispose(second);
	token_dispose(third);
	token_dispose(fourth);
	token_dispose(fifth);
	ast_dispose(root);
	lexer_dispose(lex);
	return 0;
}