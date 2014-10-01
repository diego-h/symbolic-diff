#include "differentiator.h"
#include "syntax-tree.h"
#include "parser.h"
#include "token.h"
#include "lexer.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Usage: %s INPUT\n", argv[0]);
		return 0;
	}
	
	Lexer* lex = lexer_create(argv[1]);
	Node* ast = parse(lex);
	print_ast(ast);
	//ast = diff(ast);
	//print_ast(ast);
	ast = simplify(ast); // doesn't completely work
	print_ast(ast);
	
	ast_dispose(ast);
	return 0;
}