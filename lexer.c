// token.h is #include-d in lexer.h
#include "lexer.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Subject to change, esp. as multi-character operators come in.
#define OPERATORS_LIST "+-*/^"

struct LexerImplementation {
	char* start; // points to the beginning of a dynamically allocated string
	char* cursor; // points to current position in the same string
};

Lexer* lexer_create(char* input) {
	Lexer* lexer = malloc(sizeof(Lexer));
	lexer->start = strdup(input); // should be free()-d in lexer_dispose()
	lexer->cursor = lexer->start;
	return lexer;
}

void lexer_set_cursor(Lexer* lexer, char* new_cursor) {
	lexer->cursor = new_cursor;
}

char* lexer_get_cursor(Lexer* lexer) {
	return lexer->cursor;
}

void lexer_dispose(Lexer* lexer) {
	free(lexer->start);
	free(lexer);
}

// False = 0, True != 0
int is_operator(char* s) {
	if (strlen(s) == 0) return 0;
	return (strstr(OPERATORS_LIST, s) == NULL) ? 0 : 1;
}

// Advances one token!
Token* lexer_next(Lexer* lexer) {
	// If no tokens remain, return a sentinel
	if (strlen(lexer->cursor) == 0) return NULL;

	// Advance past any leading whitespace.
	while (isspace(*(lexer->cursor))) {
		(lexer->cursor)++;
	}
	
	// Tokenize.
	char curr_str[] = {*(lexer->cursor), 0};
	
	// Create a variable token?
	if (isalpha(curr_str[0])) {
		(lexer->cursor)++;
		return token_create(VARIABLE, curr_str);
	}
	
	// Create a plus token?
	if (curr_str[0] == '+') {
		(lexer->cursor)++;
		return token_create(PLUS, curr_str);
	}
	
	// Create a minus token?
	if (curr_str[0] == '-') {
		(lexer->cursor)++;
		return token_create(MINUS, curr_str);
	}
	
	// Create a times token?
	if (curr_str[0] == '*') {
		(lexer->cursor)++;
		return token_create(TIMES, curr_str);
	}
	
	// Create a divide token?
	if (curr_str[0] == '/') {
		(lexer->cursor)++;
		return token_create(DIVIDE, curr_str);
	}
	
	// Create a power token?
	if (curr_str[0] == '^') {
		(lexer->cursor)++;
		return token_create(POWER, curr_str);
	}
	
	// Create a number token?
	int value;
	int conversions = sscanf(lexer->cursor, "%d", &value);
	if (conversions > 0) {
		char value_str[strlen(lexer->cursor) + 1];
		sprintf(value_str, "%d", value);
		(lexer->cursor) += strlen(value_str);
		return token_create(NUMBER, value_str);
	}
	
	// Create an open-paren token?
	if (curr_str[0] == '(') {
		(lexer->cursor)++;
		return token_create(OPEN, curr_str);
	}
	
	// Create a close-paren token?
	if (curr_str[0] == ')') {
		(lexer->cursor)++;
		return token_create(CLOSE, curr_str);
	}
	
	return NULL;
}