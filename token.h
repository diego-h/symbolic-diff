// File: token.h
// Defines the interface for the Token type.
// Author: Diego Hernandez
// 19 August 2014

#ifndef _token_h
#define _token_h
#include <stdbool.h>

// Type: TokenType
typedef enum {
	NUMBER, VARIABLE,
	PLUS, MINUS, TIMES, DIVIDE, POWER,
	NEGATION,
	OPEN, CLOSE
} TokenType;

// Type: Token
typedef struct TokenImplementation Token;

// Function: token_create
// Allocates heap memory to store a token struct.
// Returns a pointer to the token on the heap.
Token* token_create(TokenType type, char* token_text);

// Function: token_dispose
// Disposes of any dynamically allocated memory used by the token.
void token_dispose(Token* token);

char* token_text(Token* token);
char* token_type_str(Token* token);
TokenType token_type(Token* token);
bool token_equals(Token* a, Token* b); // case-sensitive with alphabetic text

#endif

