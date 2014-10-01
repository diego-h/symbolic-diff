// Parser version 2.0
#include "lexer.h"
#include "token.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define printf if(DEBUG_MODE)printf
#define DEBUG_MODE false

// Function prototypes
bool e(Lexer* lex, size_t depth);
bool t(Lexer* lex, size_t depth);
bool f(Lexer* lex, size_t depth);
bool p(Lexer* lex, size_t depth);

bool parse(Lexer* lex) {
	return e(lex, 0);
}

void print_token(Token* token) {
	printf("Token (%s, \"%s\")\n", token_type_str(token), token_text(token));
}

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


// TODO: Memory leaks due to tokens returned by lexer_next()
bool e(Lexer* lex, size_t depth) {
	printf("e, depth %zd\n", depth);

	if (!t(lex, depth + 1)) return false;
	char* prev = lexer_get_cursor(lex); // for purposes of backtracking by one token
	while (true) {
		Token* next = lexer_next(lex);
		if (next == NULL) break;
		
		printf("In e: ");
		print_token(next);
		if (token_type(next) == PLUS || token_type(next) == MINUS) {
			if (!t(lex, depth + 1)) return false; // no T expansion after + or - operator
		} else {
			break; // expansion of E nonterminal is complete
		}
		prev = lexer_get_cursor(lex);
	}
	printf("cursor is %s\n", lexer_get_cursor(lex));
	
	lexer_set_cursor(lex, prev);
	if (depth == 0) return strlen(lexer_get_cursor(lex)) == 0;
	return true;
}

bool t(Lexer* lex, size_t depth) {
	printf("t, depth %zd\n", depth);

	if (!f(lex, depth + 1)) return false;
	char* prev = lexer_get_cursor(lex); // for purposes of backtracking by one token
	while (true) {
		Token* next = lexer_next(lex);
		if (next == NULL) break;
		
		printf("In t: ");
		print_token(next);
		if (token_type(next) == TIMES || token_type(next) == DIVIDE) {
			if (!f(lex, depth + 1)) return false; // no F expansion after * or / operator
		} else {
			break; // expansion of T nonterminal is complete
		}
		prev = lexer_get_cursor(lex);
	}
	printf("t returns true\n");
	lexer_set_cursor(lex, prev); // backtrack by the one token that didn't match * or /
	return true;
}

// Right-recursive because ^ is right-associative
bool f(Lexer* lex, size_t depth) {
	printf("f, depth %zd\n", depth);

	bool first = p(lex, depth + 1);
	char* save = lexer_get_cursor(lex);
	bool second = term(lex, POWER, depth);
	
	if (first && !second) {
		lexer_set_cursor(lex, save); // Backtrack by the one token that didn't match ^
		return true;
	}
	if (first && second) return f(lex, depth + 1);
	return false;
}

bool p(Lexer* lex, size_t depth) {
	printf("p, depth %zd\n", depth);

	// Production rule P => number | variable
	// Reproduce the functionality of term() but check if NUMBER or VARIABLE instead of just one of the two.
	char* save = lexer_get_cursor(lex);
	printf("Current cursor is %s\n", save);
	Token* next = lexer_next(lex);
	printf("In p: "); print_token(next);
	if (next == NULL) return false;
	if (token_type(next) == NUMBER || token_type(next) == VARIABLE) return true;
	lexer_set_cursor(lex, save);
	
	// Production rule P => (E)
	save = lexer_get_cursor(lex);
	printf("Current cursor is %s\n", save);
	if (term(lex, OPEN, depth) && e(lex, depth + 1) && term(lex, CLOSE, depth)) return true;
	lexer_set_cursor(lex, save);
	
	// Production rule P => -P
	// TODO: Lexer recognizes the - as MINUS. Must override to NEGATION.
	save = lexer_get_cursor(lex);
	if (term(lex, MINUS, depth) && p(lex, depth + 1)) return true;
	lexer_set_cursor(lex, save);
	
	return false;
}