
#ifndef _differentiator_h
#define _differentiator_h

#include "syntax-tree.h"
#include "token.h"

/* Function: diff
 * Given the root of an AST, transforms that AST into an
 * AST that represents the derivative of the original.
 */
Node* diff(Node* root);

#endif