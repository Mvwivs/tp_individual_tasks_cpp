/*
	Автор: Мазов Владимир
	Группа:АПО-11

	Задача B-3. Парсер

	Требуется написать программу, которая способна вычислять арифметические
	выражения. Выражения могут содержать:
	1) знаки операций '+', '-', '/', '*'
	2) Скобки '(', ')'
	3) Целые и вещественные числа, в нотации '123', '123.345', все
	операции должны быть вещественны, а результаты выведены с точностю до
	двух знаков после запятой в том числе целые '2.00' 
	4) необходимо учитывать приоритеты операций, и возможность унарного минуса, пробелы ничего
    не значат 
    5) Если в выражении встретилась ошибка требуется вывести в
    стандартный поток вывода "[error]" (без кавычек)

*/
/*
	Recursive descent parser for a following EBNF:
		expression	-> term {+term | -term}
		term		-> factor {*factor | /factor}
		factor		-> (expression) | number
*/

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
	NUMBER = 0,
	PLUS,
	MINUS,
	DIVIDE,
	MULTIPLY,
	LEFT_BRACKET,
	RIGHT_BRACKET,
	EOL,
	ERROR
} TokenType_t;

typedef struct {
	char* expression; // input expression
	size_t pos;       // current character position
	TokenType_t type; // current token type
	double value;     // floating point value of input character
} Token_t;

void parse();
void error();
void getToken();
void match(TokenType_t tokenType);
double expression();
double term();
double factor();
char getChar();
void unGetChar();

Token_t token;

int main() {
	if (scanf("%ms", &token.expression) != 1) {
		printf("[error]");
		exit(0);
	}
	parse();
	return 0;
}

/* Parse arithmetic expression from stdio */
void parse() {
	getToken();
	double result = expression();

	if (token.type == EOL) {
		printf("%.2f", result);
	} else {
		error();
	}
}

/* Read character from stdio and determine token */
void getToken() {
	char currentChar = '\n';
	while ((currentChar = getChar()) == ' ')
		; // skip spaces

	if (isdigit(currentChar)) { // convert input number to float
		token.type = NUMBER;
		token.value = 0;
		double divider = 1;
		unsigned int multiplier = 10;

		while (isdigit(currentChar) || (currentChar == '.')) {
			if (currentChar == '.') {
				multiplier = 1;
				divider = 10;
			} else {
				token.value = multiplier * token.value +
					      (currentChar - '0') / divider;
				if (multiplier == 1) {
					divider *= 10;
				}
			}
			currentChar = getChar();
		}
		unGetChar();

	} else {
		switch (currentChar) {
		case '+':
			token.type = PLUS;
			break;
		case '-':
			token.type = MINUS;
			break;
		case '/':
			token.type = DIVIDE;
			break;
		case '*':
			token.type = MULTIPLY;
			break;
		case '(':
			token.type = LEFT_BRACKET;
			break;
		case ')':
			token.type = RIGHT_BRACKET;
			break;
		case '\0':
			token.type = EOL;
			break;
		default:
			token.type = ERROR;
			break;
		}
	}
}

/* Parse EBFL: expression -> term {+term | -term} */
double expression() {
	double result = term();

	while (token.type == PLUS || token.type == MINUS) {
		if (token.type == PLUS) {
			match(PLUS);
			result += term();
		} else {
			match(MINUS);
			result = result - term();
		}
	}
	return result;
}

/* Parse EBFL: term -> factor {*factor | /factor} */
double term() {
	double result = factor();

	while (token.type == MULTIPLY || token.type == DIVIDE) {
		if (token.type == MULTIPLY) {
			match(MULTIPLY);
			result = result * factor();
		} else {
			match(DIVIDE);
			result = result / factor();
		}
	}
	return result;
}

/* Parse EBFL: factor -> (expression)|Number */
double factor() {
	double result;

	if (token.type == LEFT_BRACKET) {
		match(LEFT_BRACKET);
		result = expression();
		match(RIGHT_BRACKET);
	} else {
		getToken();
		result = token.value;
	}
	return result;
}

/* Get next char from input */
char getChar() {
	char currentChar = token.expression[token.pos];
	++token.pos;
	return currentChar;
}

void unGetChar() {
	--token.pos;
}

/* Exit program on error */
void error() {
	printf("[error]");
	free(token.expression);
	exit(0);
}

/* Match token type */
void match(TokenType_t tokenType) {
	if (token.type == tokenType) {
		getToken();
	} else {
		error();
	}
}
