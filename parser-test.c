#include "lexer.h"
#include "token.h"
#include "parser.h"
#include "syntax-tree.h"
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

// Remember not to put a semicolon!
#define BASH_TEST_MODE true

// The number of tests for each success/failure case
#define NUM_TESTS 8
// The number of operands in a test
#define TEST_LEN 10


void run_test(char* input) {
	Lexer* lexer = lexer_create(input);
	Node* ast = parse(lexer);
	printf("%s Input: %s\n", ast != NULL ? "Success!" : "Parse failed.", input);
	printf("AST: ");
	print_ast(ast);
	if (ast != NULL) printf("Evaluated at x=6: %d\n", eval(ast, 6));
	lexer_dispose(lexer);
}

int user_test(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s \"INPUT\"\n", argv[0]);
		return 1;
	}
	char* input = argv[1];
	run_test(input);
	return 0;
}


char random_digit() {
	int digit = rand() % 10;
	return (char)(((int)'0') + digit);
}

char random_operator() {
	char* operators = "+-*/^";
	int num_ops = strlen(operators);
	return operators[rand() % num_ops];
}

char* random_test(int len) { // len is number of operands (single-digit)
	char buf[2*len - 1 + 1];
	for (int i = 0; i < 2*len - 1; i++) {
		if (i % 2 == 0) buf[i] = random_digit();
		else            buf[i] = random_operator();
	}
	buf[2*len - 1] = '\0';
	return strdup(buf);
}

void disfigure(char* t) {
	//printf("Before: %s\n", t);
	int a = rand() % strlen(t);
	int b;
	do { b = rand() % strlen(t); } while ((b % 2) == (a % 2));
	char temp = t[a];
	t[a] = t[b];
	t[b] = temp;
	//printf("After:   %s\n", t);
}

int generate_tests(int argc, char* argv[]) {
	srand(time(NULL));
	
	printf("Expected success:\n");
	for (int i = 0; i < NUM_TESTS; i++) {
		char* t = random_test(TEST_LEN);
		run_test(t);
		free(t);
	}
	
	printf("Expected failure:\n");
	for (int i = 0; i < NUM_TESTS; i++) {
		char* t = random_test(TEST_LEN);
		disfigure(t);
		run_test(t);
		free(t);
	}
	return 0;
}

int main(int argc, char *argv[]) {
	return BASH_TEST_MODE ? user_test(argc, argv) : generate_tests(argc, argv);
}

