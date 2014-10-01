#ifndef _lexer_h
#define _lexer_h

#include "token.h"

typedef struct LexerImplementation Lexer;

Lexer* lexer_create(char* input);
void lexer_dispose(Lexer* lexer);

Token* lexer_next(Lexer* lexer);
char* lexer_get_cursor(Lexer* lexer);
void lexer_set_cursor(Lexer* lexer, char* new_cursor);

#endif