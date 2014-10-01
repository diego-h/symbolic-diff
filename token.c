#include "token.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Types of tokens:
// Integer Literal
// Variable: Simply x
// Operator: +-*/^
// Whitespace: May have added functionality later. As per Alex, group them together.

struct TokenImplementation {
	TokenType type;
	char* text; // should be dynamically allocated
};

Token* token_create(TokenType type, char* token_text) {
	Token* token = malloc(sizeof(Token));
	token->type = type;
	token->text = strdup(token_text); // should be free()-d in token_dispose()
	return token;
}

void token_dispose(Token* token) {
	free(token->text);
	free(token);
}

char* token_text(Token* token) {
	return token->text;
}

bool token_equals(Token* a, Token* b) {
	return (a->type == b->type) && (strcmp(a->text, b->text) == 0);
}

TokenType token_type(Token* token) {
	return token->type;
}

char* token_type_str(Token* token) {
	switch (token->type) {
		case NUMBER:   return "number";
		case VARIABLE: return "variable";
		case PLUS:     return "plus";
		case MINUS:    return "minus";
		case TIMES:    return "times";
		case DIVIDE:   return "divide";
		case POWER:    return "power";
		case OPEN:     return "open";
		case CLOSE:    return "close";
		default:       return "unknown (should fix)";
	}
}
