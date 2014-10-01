#include "lexer.h"
#include "token.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s \"INPUT\"\n", argv[0]);
		return 1;
	}
	char* input = argv[1];
	printf("The input is the following: %s\n", input);
	Lexer* lexer = lexer_create(input);
	
	// Read all tokens from the lexer.
	Token* curr;
	while((curr = lexer_next(lexer))) {
		printf("Read token: (%s, \"%s\")\n", token_type_str(curr), token_text(curr));
		token_dispose(curr);
	}
	
	lexer_dispose(lexer);
	return 0;
}