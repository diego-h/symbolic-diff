// Parser version 3.0 (with AST building)
#include "lexer.h"
#include "token.h"
#include "syntax-tree.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define printf if(DEBUG_MODE)printf
#define DEBUG_MODE false

// Function prototypes
Node* e(Lexer* lex, size_t depth);
Node* t(Lexer* lex, size_t depth);
Node* f(Lexer* lex, size_t depth);
Node* p(Lexer* lex, size_t depth);

Node* parse(Lexer* lex) {
	return e(lex, 0);
}

void print_token(Token* token) {
	printf("Token (%s, \"%s\")\n", token_type_str(token), token_text(token));
}

Node* term(Lexer* lexer, TokenType type, size_t depth) {
	Token* next = lexer_next(lexer);
	if (next == NULL) {
		printf("term() called with null token, failure.\n");
		return NULL;
	}
	printf("term() called with token (%s, \"%s\") at a depth of %zd\n",
		token_type_str(next), token_text(next), depth);
	if (token_type(next) == type) {
		printf("success\n");
		Node* n = node_create(next);
		free(next);
		return n;
	}
	if (token_type(next) == MINUS && type == NEGATION) {
		printf("success, creating Negation token\n");
		return negation_node_create();
	}
	printf("failure\n");
	free(next);
	return NULL;
}


// TODO: Memory leaks due to tokens returned by lexer_next()
Node* e(Lexer* lex, size_t depth) {
	printf("e, depth %zd\n", depth);

	Node* t_subtree_1 = t(lex, depth + 1);	
	if (t_subtree_1 == NULL) return NULL;
	
	char* prev = lexer_get_cursor(lex); // for purposes of backtracking by one token
	while (true) {
		Token* next = lexer_next(lex);
		if (next == NULL) break;
		
		printf("In e: ");
		print_token(next);
		Node* t_subtree_2;
		if (token_type(next) == PLUS || token_type(next) == MINUS) {
			t_subtree_2 = t(lex, depth + 1);
			if (t_subtree_2 == NULL) return NULL; // no T expansion after + or - operator
		} else {
			break; // expansion of E nonterminal is complete
		}
		
		Node* root = node_create(next);
		merge_subtrees(root, t_subtree_1, t_subtree_2);
		t_subtree_1 = root;
		
		prev = lexer_get_cursor(lex);
	}
	printf("cursor is %s\n", lexer_get_cursor(lex));
	
	lexer_set_cursor(lex, prev);
	if (depth == 0 && strlen(lexer_get_cursor(lex)) != 0) return NULL;
	return t_subtree_1;
}

Node* t(Lexer* lex, size_t depth) {
	printf("t, depth %zd\n", depth);

	Node* f_subtree_1 = f(lex, depth + 1);
	if (f_subtree_1 == NULL) return false;
	
	char* prev = lexer_get_cursor(lex); // for purposes of backtracking by one token
	while (true) {
		Token* next = lexer_next(lex);
		if (next == NULL) break;
		
		printf("In t: ");
		print_token(next);
		Node* f_subtree_2;
		if (token_type(next) == TIMES || token_type(next) == DIVIDE) {
			f_subtree_2 = f(lex, depth + 1); 
			if (f_subtree_2 == NULL) return NULL; // no F expansion after * or / operator
		} else {
			break; // expansion of T nonterminal is complete
		}
		
		Node* root = node_create(next);
		merge_subtrees(root, f_subtree_1, f_subtree_2);
		f_subtree_1 = root;
		
		prev = lexer_get_cursor(lex);
	}
	printf("t returns true\n");
	lexer_set_cursor(lex, prev); // backtrack by the one token that didn't match * or /
	return f_subtree_1;
}

// Right-recursive because ^ is right-associative
Node* f(Lexer* lex, size_t depth) {
	printf("f, depth %zd\n", depth);

	Node* first = p(lex, depth + 1);
	char* save = lexer_get_cursor(lex);
	Node* second = term(lex, POWER, depth);
	
	if (first != NULL && second == NULL) {
		lexer_set_cursor(lex, save); // Backtrack by the one token that didn't match ^
		return first;
	}
	if (first && second) {
		Node* f_subtree = f(lex, depth + 1);
		if (f_subtree == NULL) return NULL;
		merge_subtrees(second, first, f_subtree);
		return second;
	}
	return NULL;
}

Node* p(Lexer* lex, size_t depth) {
	printf("p, depth %zd\n", depth);

	// Production rule P => number | variable
	// Reproduce the functionality of term() but check if NUMBER or VARIABLE instead of just one of the two.
	char* save = lexer_get_cursor(lex);
	printf("Current cursor is %s\n", save);
	Token* next = lexer_next(lex);
	printf("In p: "); print_token(next);
	if (next == NULL) return false;
	if (token_type(next) == NUMBER || token_type(next) == VARIABLE) {
		return node_create(next);
	}
	lexer_set_cursor(lex, save);
	
	// Production rule P => (E)
	save = lexer_get_cursor(lex);
	printf("Current cursor is %s\n", save);
	Node* e_subtree;
	if (term(lex, OPEN, depth) != NULL && (e_subtree = e(lex, depth + 1)) != NULL && term(lex, CLOSE, depth) != NULL) {
		return e_subtree;
	}
	lexer_set_cursor(lex, save);
	
	// Production rule P => -P
	// TODO: Lexer recognizes the - as MINUS. Must override to NEGATION.
	save = lexer_get_cursor(lex);
	Node* p_subtree;
	if (term(lex, MINUS, depth) != NULL && (p_subtree = p(lex, depth + 1)) != NULL) {
		Node* negation_node = negation_node_create();
		connect_unary_op(negation_node, p_subtree);
		return negation_node;
	}
	lexer_set_cursor(lex, save);
	
	return NULL;
}