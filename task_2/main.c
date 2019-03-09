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
	4) необходимо учитывать приоритеты операций, и возможность унарного
   минуса, пробелы ничего не значат 5) Если в выражении встретилась ошибка
   требуется вывести в стандартный поток вывода "[error]" (без кавычек)

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

typedef enum { ret_OK = 0, ret_ERROR = 1 } Error_t;

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
	double value;     // floating point value of last number
} Token_t;

Error_t parse(Token_t* token);
void getToken(Token_t* token);
Error_t match(Token_t* token, TokenType_t tokenType);
Error_t expression(Token_t* token, double* result);
Error_t term(Token_t* token, double* result);
Error_t factor(Token_t* token, double* result);
char getChar(Token_t* token);
void unGetChar(Token_t* token);

int main() {
	Token_t token = {.pos = 0};

	if (scanf("%ms", &token.expression) != 1) {
		printf("[error]");
		return 0;
	}
	parse(&token);
	free(token.expression);
	return 0;
}

/* Parse arithmetic expression from stdio */
Error_t parse(Token_t* token) {
	double result = 0;
	getToken(token);
	if (expression(token, &result) != ret_OK) {
		printf("[error]");
		return ret_ERROR;
	}

	if (token->type == EOL) {
		printf("%.2f", result);
		return ret_OK;
	} else {
		printf("[error]");
		return ret_ERROR;
	}
}

/* Read character from stdio and determine token */
void getToken(Token_t* token) {
	char currentChar = '\n';
	while ((currentChar = getChar(token)) == ' ')
		; // skip spaces

	if (isdigit(currentChar)) { // convert input number to float
		token->type = NUMBER;
		token->value = 0;
		double divider = 1;
		unsigned int multiplier = 10;

		while (isdigit(currentChar) || (currentChar == '.')) {
			if (currentChar == '.') {
				multiplier = 1;
				divider = 10;
			} else {
				token->value = multiplier * token->value +
					       (currentChar - '0') / divider;
				if (multiplier == 1) {
					divider *= 10;
				}
			}
			currentChar = getChar(token);
		}
		unGetChar(token);

	} else {
		switch (currentChar) {
		case '+':
			token->type = PLUS;
			break;
		case '-':
			token->type = MINUS;
			break;
		case '/':
			token->type = DIVIDE;
			break;
		case '*':
			token->type = MULTIPLY;
			break;
		case '(':
			token->type = LEFT_BRACKET;
			break;
		case ')':
			token->type = RIGHT_BRACKET;
			break;
		case '\0':
			token->type = EOL;
			break;
		default:
			token->type = ERROR;
			break;
		}
	}
}

/* Parse EBFL: expression -> term {+term | -term} */
Error_t expression(Token_t* token, double* result) {
	//*result = 0;
	double calculate = 0;
	Error_t err = term(token, &calculate);
	if (err == ret_ERROR) {
		return ret_ERROR;
	}
	*result = calculate;

	while (token->type == PLUS || token->type == MINUS) {
		if (token->type == PLUS) {
			err = match(token, PLUS);
			if (err == ret_ERROR) {
				return ret_ERROR;
			}
			err = term(token, &calculate);
			if (err == ret_ERROR) {
				return ret_ERROR;
			}
			*result += calculate;
		} else {
			err = match(token, MINUS);
			if (err == ret_ERROR) {
				return ret_ERROR;
			}
			err = term(token, &calculate);
			if (err == ret_ERROR) {
				return ret_ERROR;
			}
			*result = *result - calculate;
		}
	}
	return ret_OK;
}

/* Parse EBFL: term -> factor {*factor | /factor} */
Error_t term(Token_t* token, double* result) {
	double calculate = 0;

	Error_t err = factor(token, result);
	if (err == ret_ERROR) {
		return ret_ERROR;
	}

	while (token->type == MULTIPLY || token->type == DIVIDE) {
		if (token->type == MULTIPLY) {
			err = match(token, MULTIPLY);
			if (err == ret_ERROR) {
				return ret_ERROR;
			}
			err = factor(token, &calculate);
			if (err == ret_ERROR) {
				return ret_ERROR;
			}
			*result = *result * calculate;
		} else {
			err = match(token, DIVIDE);
			if (err == ret_ERROR) {
				return ret_ERROR;
			}
			err = factor(token, &calculate);
			if (err == ret_ERROR) {
				return ret_ERROR;
			}
			*result = *result / calculate;
		}
	}
	return ret_OK;
}

/* Parse EBFL: factor -> (expression)|Number */
Error_t factor(Token_t* token, double* result) {
	*result = 0;
	Error_t err = ret_OK;

	if (token->type == LEFT_BRACKET) {
		err = match(token, LEFT_BRACKET);
		if (err == ret_ERROR) {
			return ret_ERROR;
		}
		err = expression(token, result);
		if (err == ret_ERROR) {
			return ret_ERROR;
		}
		err = match(token, RIGHT_BRACKET);
		if (err == ret_ERROR) {
			return ret_ERROR;
		}
	} else {
		getToken(token);
		*result = token->value;
	}
	return ret_OK;
}

/* Get next char from input */
char getChar(Token_t* token) {
	char currentChar = token->expression[token->pos];
	++token->pos;
	return currentChar;
}

void unGetChar(Token_t* token) {
	--token->pos;
}

/* Match token type */
Error_t match(Token_t* token, TokenType_t tokenType) {
	if (token->type == tokenType) {
		getToken(token);
		return ret_OK;
	} else {
		return ret_ERROR;
	}
}
