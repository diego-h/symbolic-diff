#include "token.h"
#include "lexer.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Function prototypes
bool e1(Lexer* lexer, size_t depth);
bool e2(Lexer* lexer, size_t depth);
bool e(Lexer* lexer, size_t depth);
bool t1(Lexer* lexer, size_t depth);
bool t2(Lexer* lexer, size_t depth);
bool t3(Lexer* lexer, size_t depth);
bool t(Lexer* lexer, size_t depth);

bool parse(Lexer* lexer) {
	return e(lexer, 0);
}

// Advances the lexer's cursor one token,
// and returns whether the overtaken token matches the given type.
bool term(Lexer* lexer, TokenType type, size_t depth) {
	Token* next = lexer_next(lexer);
	if (next == NULL) {
		printf("term() called with null token, failure.\n");
		return false;
	}
	printf("term() called with token (%s, \"%s\") at a depth of %zd\n",
		token_type_str(next), token_text(next), depth);
	if (token_type(next) == type) {
		printf("success\n");
		free(next);
		return true;
	}
	printf("failure\n");
	free(next);
	return false;
}

bool e1(Lexer* lexer, size_t depth) {
	printf("e1, depth %zd\n", depth);
	return t(lexer, depth + 1);
}

bool e2(Lexer* lexer, size_t depth) {
	printf("e2, depth %zd\n", depth);
	return t(lexer, depth + 1) && term(lexer, PLUS, depth) && e(lexer, depth + 1);
}

bool e(Lexer* lexer, size_t depth) {
	printf("e, depth %zd\n", depth);
	char* saved_cursor = lexer_get_cursor(lexer);
	
	bool e2_result = e2(lexer, depth + 1);
	size_t len_rest = strlen(lexer_get_cursor(lexer));
	if ((depth > 0 && e2_result) || (depth == 0 && e2_result && len_rest == 0)) return true;
	lexer_set_cursor(lexer, saved_cursor);
	
	bool e1_result = e1(lexer, depth + 1);
	len_rest = strlen(lexer_get_cursor(lexer));
	if ((depth > 0 && e1_result) || (depth == 0 && e1_result && len_rest == 0)) return true;
	lexer_set_cursor(lexer, saved_cursor);
	
	return false;
}

bool t1(Lexer* lexer, size_t depth) {
	printf("t1, depth %zd\n", depth);
	return term(lexer, NUMBER, depth);
}

bool t2(Lexer* lexer, size_t depth) {
	printf("t2, depth %zd\n", depth);
	return term(lexer, NUMBER, depth) && term(lexer, TIMES, depth) && t(lexer, depth + 1);
}

bool t3(Lexer* lexer, size_t depth) {
	printf("t3, depth %zd\n", depth);
	return term(lexer, OPEN, depth) && e(lexer, depth + 1) && term(lexer, CLOSE, depth);
}

bool t(Lexer* lexer, size_t depth) {
	printf("t, depth %zd\n", depth);
	char* saved_cursor = lexer_get_cursor(lexer);
	if (t2(lexer, depth + 1)) return true;
	lexer_set_cursor(lexer, saved_cursor);
	if (t1(lexer, depth + 1)) return true;
	lexer_set_cursor(lexer, saved_cursor);
	if (t3(lexer, depth + 1)) return true;
	lexer_set_cursor(lexer, saved_cursor);
	return false;
}

