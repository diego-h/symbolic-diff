// File: parser.h
// Diego Hernandez
// 19 August 2014

#ifndef _parser_h
#define _parser_h

#include "lexer.h"
#include "syntax-tree.h"
#include <stdbool.h>

Node* parse(Lexer* lexer);

#endif