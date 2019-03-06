/*
	Автор: Мазов Владимир
	Группа: АПО-11

	Задача A-2. Задача о системах счисления

	Составить программу для перевода чисел из системы счисления по основанию P в
	систему счисления по основанию Q, где 2 меньше или равно Q меньше P меньше или
	равно 36. Значения P, Q и строковое представление S числа в исходной системе
	счисления вводятся с клавиатуры в следующем порядке: P Q S

	S не выходит за границы size_t.

	Процедура перевода не может использовать соответствующей функции системной
	библиотеки и должна быть оформлена в виде отдельной функции, на вход которой
	подаются значения P, Q и S, размещенные в динамической памяти. На выход функция
	должна возвращать строковое представление S1 числа в целевой системе счисления.

	Программа должна уметь обрабатывать ошибки во входных данных.
	В случае возникновения ошибки нужно вывести в поток стандартного вывода
	сообщение "[error]" (без кавычек) и завершить выполнение программы.
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

char* convert(const unsigned int* from, const unsigned int* to, const char* number);
unsigned int fromChar(char n);
char toChar(unsigned int n);
unsigned long toDec(const char* number, unsigned int base);
void fromDec(unsigned long number, unsigned int base, char* result);
void reverseString(char* str);
unsigned int validate(const char* number, unsigned int from, unsigned int to);

int main() {

	unsigned int P = 0;
	unsigned int Q = 0;
	char* S;

	if (scanf("%u%u%ms", &P, &Q, &S) != 3) {
		printf("[error]");
		return 0;
	}

	if (!validate(S, P, Q)) {
		printf("[error]");
		free(S);
		return 0;
	}

	char* S1 = convert(&P, &Q, S);
	if (S1 == NULL) {
		printf("[error]");
		free(S);
		return 0;
	}
	printf("%s", S1);

	free(S1);
	free(S);

	return 0;
}

/* Convert number from one base to another */
char* convert(const unsigned int* from, const unsigned int* to, const char* number) {
	if ((from == NULL)|| (to == NULL) || (number == NULL)) {
		return NULL;
	}
	unsigned long dec = toDec(number, *from);
	const size_t maxLength = log(dec) / log(*to) + 2;
	char* result = (char*)malloc(maxLength);
	if (result == NULL) {
		return NULL;
	}
	fromDec(dec, *to, result);
	return result;
}

/* Get value of a character */
unsigned int fromChar(char n) {
	if ((n >= '0') && (n <= '9')) {
		return n - '0';
	} else if ((n >= 'a') && (n <= 'z')) {
		return n - 'a' + 10;
	} else if ((n >= 'A') && (n <= 'Z')) {
		return n - 'A' + 10;
	} else {
		return 100; // error
	}
}

/* Get character of a value */
char toChar(unsigned int n) {
	if ((n >= 0) && (n <= 9)) {
		return (char)(n + '0');
	} else if ((n >= 10) && (n <= 35)) {
		return (char)(n + 'A');
	} else {
		return 0; // error
	}
}

/* Convert number from any base to decimal */
unsigned long toDec(const char* number, unsigned int base) {
	if (number == NULL) {
		return 0;
	}
	unsigned long result = 0;
	unsigned long power = 1;

	size_t i = strlen(number);
	do {
		--i;
		result += fromChar(number[i]) * power;
		power *= base;
	} while (i > 0);

	return result;
}

/* Convert number from decimal to any base */
void fromDec(unsigned long number, unsigned int base, char* result) {
	if (result == NULL) {
		return;
	}

	size_t i = 0;
	while (number != 0) {
		unsigned int digit = number % base;
		number /= base;
		result[i] = toChar(digit);
		++i;
	}
	result[i] = '\0';
	reverseString(result);
}

/* Reverse character order in a string */
void reverseString(char* str) {
	if (str == NULL) {
		return;
	}
	
	char* start = str;
	char* end = start + strlen(str) - 1;
	char temp;

	while (end > start) {
		temp = *start;
		*start = *end;
		*end = temp;

		++start;
		--end;
	}
}

/* Validate input string and bases */
unsigned int validate(const char* number, unsigned int from, unsigned int to) {
	if (number == NULL) {
		return 0; //error
	}
	if (!((2 <= to) && (to < from) && (from <= 36)))
		return 0;	//error
	size_t i = 0;
	while (number[i] != '\0') {
		unsigned int value = fromChar(number[i]);
		if (value >= from) {
			return 0; //error
		}

		++i;
	}
	return 1;
}