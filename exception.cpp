#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <stdint.h>
#include <cstring>

#include "rational_prototypes.h"
#include "exception_prototypes.h"
#include "vector_prototypes.h"
#include "matrix_prototypes.h"

#define ERR_INPUT 1
#define ERR_OVERFLOW 2
#define ERR_ZERO_DIVISION 3
#define ERR_MALLOC 4
#define ERR_OUT_OF_RANGE 5
#define ERR_FOPEN 6
#define ERR_DIMENSION 7
#define ERR_IN_FILE 8

using namespace std;

Exception::Exception()
{
    code = 0;
    printf("\x1b[41mERR_DEFAULT:\x1b[0m");
}

Exception::Exception(int e, Rational_number a, Rational_number b)
{
    code = e;
    switch(code)
    {
        case 1: //когда что-то не так ввел юзер
            printf("\x1b[41mERR_INPUT\x1b[0m\n");
            break;
        case 2: //переполнение числа
            printf("\x1b[41mERR_OVERFLOW\x1b[0m\n"); 
            break;
        case 3: //деление на ноль
            printf("\x1b[41mERR_ZERO_DIVISION:\x1b[0m\n"); 
            if (a != "0")
            { 
				a.show(); 
				cout << "/" << endl;
				b.show();
			}
            break;   
        case 4: //проблемы с памятью
            printf("\x1b[41mERR_MALLOC\x1b[0m\n"); 
            break;
        case 5: //выход за диапазон
            printf("\x1b[41mERR_OUT_OF_RANGE\x1b[0m\n"); 
            break;  
        case 6: //невозможно открыть файл
            printf("\x1b[41mERR_FOPEN\x1b[0m\n"); 
            break; 
        case 7: //неправильные размерности
            printf("\x1b[41mERR_DIMENSION\x1b[0m\n"); 
            break;   
        case 8: //неправильный файл
            printf("\x1b[41mERR_IN_FILE\x1b[0m\n"); 
            break;        
    }
}

