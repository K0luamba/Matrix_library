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

void Rational_number::show() const
{
    if (a == 0) //если 0
    {
        cout << "0 ";
        return;
    }
    if (sign) cout << "-";   //вывод знака
    if (b == 1) //если целое
    {
        cout << a << " ";
        return;
    }
    cout << a << "/" << b << " "; //стандарт
    return;
}

Rational_number& Rational_number::operator = (const Rational_number& arg)
{
    a = arg.a;
    b = arg.b;
    sign = arg.sign;
    return (*this);
}

Rational_number::Rational_number()
{
    a = 0;
    b = 1;
    sign = 0;
}

Rational_number::Rational_number(uint32_t a1, uint32_t b1)
{
    if (b1 == 0)
    {
        Exception ex((int)ERR_ZERO_DIVISION); //ошибка деления на ноль
        throw(ex);
    }
    a = a1;
    b = b1;
    sign = 0;
    make_canonical(); //это добавляем в каждом конструкторе, где могут быть оба числа > 1 или нужно проверить знак 0
}

Rational_number::Rational_number(int a1)
{
    b = 1; //используется только для целых
    sign = 0;
    if (a1 >= 0) a = a1;
    else
    {
        a = -a1;
        sign = 1;
    }
    make_canonical(); 
}

Rational_number::Rational_number(const char* s_a,const char* s_b)
{
    int i = 1;
    uint64_t a1,b1;
    while(s_a[i] != '\0')
    {
		if(isdigit(s_a[i]) == 0)
		{
            Exception ex((int)ERR_INPUT); //ошибка ввода
            throw(ex);
		}
		i++;	
	}
	i = 1;
	while(s_b[i] != '\0')
    {
		if(isdigit(s_b[i]) == 0)
		{
            Exception ex((int)ERR_INPUT); //ошибка ввода
            throw(ex);	
		}	
		i++;
	}
	sign = 0; //проверка знака дроби
	if(s_a[0] == '-')
	{
		sign = (sign + 1)%2; //если '-', надо переводить в число без него
		char* s_a1 = (char*) malloc((strlen(s_a)+1)*sizeof(char));
		strcpy(s_a1,s_a);
		s_a1[0] = '0'; //само число не меняем
		a1 = atoi(s_a1);
		free(s_a1);
	}
	else
	{
		a1 = atoi(s_a); //просто переводим всю строку
	}
    if (a1 > UINT32_MAX)
    {
        Exception ex((int)ERR_OVERFLOW); //ошибка переполнения
        throw(ex);
    }
    a = a1;
	if(s_b[0] == '-')
	{
		sign = (sign + 1)%2;
		char* s_b1 = (char*) malloc((strlen(s_b)+1)*sizeof(char));
		strcpy(s_b1,s_b);
		s_b1[0] = '0';
		b1 = atoi(s_b1);
		free(s_b1);
	}
	else
	{
		b1 = atoi(s_b);
	}
    if (b1 == 0)
    {
        Exception ex((int)ERR_ZERO_DIVISION); //ошибка деления на ноль
        throw(ex);
    }
    if (b1 > UINT32_MAX)
    {
        Exception ex((int)ERR_OVERFLOW); //ошибка переполнения
        throw(ex);
    }
    b = b1;   
    make_canonical();
}

Rational_number::Rational_number(const char* str)
{
    int i = 0; //индекс, по которому ориентируемся
    sign = 0;
    while ((str[i] == ' ') || (str[i] == '\t')) i++; //пропускаем пустые места
    if (str[i] == '-')
    {
        sign = 1;
        i++;
    }
    while ((str[i] == ' ') || (str[i] == '\t')) i++; //пропускаем пустые места
    if (str[i] == '/') 
    {
        Exception ex((int)ERR_INPUT); //ошибка ввода
        throw(ex);
    }
    a = 0;
    uint64_t a1 = 0;
    if (str[i] == '#') //если не написано число - допустим, это 0
    {
        b = 1;
        return;
    }
    while((str[i] != ' ') && (str[i] != '\t') && (str[i] != '\0') && (str[i] != '/') && (str[i] != '\n')) //подсчет числителя
    {
        if (isdigit(str[i]) == 0)
        {
            Exception ex((int)ERR_INPUT); //ошибка ввода
            throw(ex);
        }
        a1 = a1*10 + (str[i] - '0');
        i++;
    }
    if (a1 > UINT32_MAX)
    {
        Exception ex((int)ERR_OVERFLOW); //ошибка переполнения
        throw(ex);
    }
    a = a1;
    while ((str[i] == ' ') || (str[i] == '\t')) i++; //пропускаем пустые места
    if (str[i] == '/')
    {
        i++;
        while ((str[i] == ' ') || (str[i] == '\t')) i++; //пропускаем пустые места
        if ((str[i] == '\0') || (str[i] == '\n')) //если строка вида "123/"
        {
            Exception ex((int)ERR_INPUT); //ошибка ввода
            throw(ex);
        }
        b = 0;
        uint64_t b1 = 0;
        while ((str[i] != '\0') && (str[i]  != ' ') && (str[i] != '\t') && (str[i] != '\n'))  //подсчет знаменателя
        {
            if (isdigit(str[i]) == 0)
            {
                Exception ex((int)ERR_INPUT); //ошибка ввода
                throw(ex);  
            }
            b1 = b1*10 + (str[i] - '0');
            i++;
        } 
        if (b1 == 0)
        {
            Exception ex((int)ERR_ZERO_DIVISION); //ошибка деления на ноль
            throw(ex);
        } 
        if (b1 > UINT32_MAX)
        {
            Exception ex((int)ERR_OVERFLOW); //ошибка переполнения
            throw(ex);
        }
        b = b1;
        while ((str[i] == ' ') || (str[i] == '\t')) i++; //пропускаем пустые места
        if (str[i] == '#')
        {while((str[i] != '\0') && (str[i] != '\n')) i++;} //пропускаем комменты целиком
        else if ((str[i] != '\0') && (str[i] != '\n')) //если какие-то другие символы
        {
            Exception ex((int)ERR_INPUT); //ошибка ввода
            throw(ex);  
        }
        make_canonical();
    }
    else //если нет знаменателя
    { 
		b = 1;
        if (str[i] == '#')
        {while(str[i] != '\0') i++;} //пропускаем комменты целиком
        else if ((str[i] != '\0') && (str[i] != '\n')) //если какие-то другие символы
        {
            Exception ex((int)ERR_INPUT); //ошибка ввода
            throw(ex);  
        }
        make_canonical();
	} 
}

void Rational_number::make_canonical() 
{
    uint32_t nod = NOD(a,b);
    a = a / nod;
    b = b / nod;
    if (a == 0) //для однозначности - ноль = + 0/1
    {
		sign = 0; 
		b = 1;
	} 
    return;
}

uint32_t Rational_number::NOD(const uint32_t& a1,const uint32_t& b1) const
{
    uint32_t x = a1;
    uint32_t y = b1;
    while((x != 0) && (y != 0)) //по стандартной схеме сокращаем числа
    {
        if (x >= y) {x = x % y;}
        else {y = y % x;}
	}
	return x + y;
}

Rational_number Rational_number::get_number_part() const
{
    uint32_t num;
    num = a / b;
    Rational_number res; //создаем новое число
    res.sign = sign; //с тем же знаком
    res.a = num; //только из целой части
    res.b = 1;
    return res;
}

Rational_number Rational_number::get_fractional_part() const
{
    uint32_t num;
    num = a / b; //это целая часть
    Rational_number res; //создаем новое число
    res.a = a - num*b; //вычитание целой части
    res.b = b; //знаменатель не трогаем
    res.sign = 0;   
    return res;  
}

void Rational_number::round()
{
    Rational_number num = get_number_part();
    Rational_number fract =  get_fractional_part();
    double a1 = fract.a; //переведем в десятичную дробь для простого сравнения
    double b1 = fract.b;
    double x = a1/b1; 
    b = 1;
    if (x >= 0.5){a = num.a + 1;} //к большему по модулю
    else {a = num.a;} //к меньшему по модулю
    return;
}

void Rational_number::floor()
{
    Rational_number num = get_number_part();
    b = 1;
    if (sign == 0){a = num.a;} //просто берем целую часть
    else {a = num.a + 1;} //если число < 0, то, напротив, округляем до большего по модулю
    return;
}

Rational_number::operator int() const
{
    Rational_number num = get_number_part();
    if (num.a > 247483647)
    {
        Exception ex((int)ERR_OVERFLOW); //ошибка переполнения
        throw(ex);
    }
    int a1 = num.a;
    if (sign) a1 = -a1; //просто отбрасываем дробное и учитываем знак
    return a1;
}

Rational_number::operator long int() const
{
    Rational_number num = get_number_part();
    long int a1 = num.a; //нет ограничений, т.к. размер больше
    if (sign) a1 = -a1; 
    return a1;
}

Rational_number::operator short() const
{
    Rational_number num = get_number_part();
    if (num.a > 32767)
    {
        Exception ex((int)ERR_OVERFLOW); //ошибка переполнения
        throw(ex);
    }
    short a1 = num.a;
    if (sign) a1 = -a1; //просто отбрасываем дробное и учитываем знак
    return a1;
}

Rational_number::operator double() const
{
    double b1 = b; //точно поместится, а дробь правильная после сборки
    return (sign == 0) ? (a/b1):-(a/b1); //чтобы операция давала результат в double
}

bool Rational_number::operator < (const Rational_number & f) const 
{
    int32_t w1,w2;
    if (sign == 0) w1 = a * f.b; //считаем левую часть после "убирания" дробей при сравнении
    else  w1 = (-1) * a * f.b;
    if (f.sign == 0) w2 = f.a * b; //считаем правую часть после "убирания" дробей при сравнении
    else w2 = (-1) * f.a * b;
    return w1 < w2;
}

bool Rational_number::operator <= (const Rational_number & f) const 
{
    int32_t w1,w2;
    if (sign == 0) w1 = a * f.b; //считаем левую часть после "убирания" дробей при сравнении
    else  w1 = (-1) * a * f.b;
    if (f.sign == 0) w2 = f.a * b; //считаем правую часть после "убирания" дробей при сравнении
    else w2 = (-1) * f.a * b;
    return w1 <= w2;
}

bool Rational_number::operator > (const Rational_number & f) const 
{
    int32_t w1,w2;
    if (sign == 0) w1 = a * f.b; //считаем левую часть после "убирания" дробей при сравнении
    else  w1 = (-1) * a * f.b;
    if (f.sign == 0) w2 = f.a * b; //считаем правую часть после "убирания" дробей при сравнении
    else w2 = (-1) * f.a * b;
    return w1 > w2;
}

bool Rational_number::operator >= (const Rational_number & f) const 
{
    int32_t w1,w2;
    if (sign == 0) w1 = a * f.b; //считаем левую часть после "убирания" дробей при сравнении
    else  w1 = (-1) * a * f.b;
    if (f.sign == 0) w2 = f.a * b; //считаем правую часть после "убирания" дробей при сравнении
    else w2 = (-1) * f.a * b;
    return w1 >= w2;
}

bool Rational_number::operator == (const Rational_number & f) const 
{
    if (a + f.a) return (a == f.a) && (b == f.b) && (sign == f.sign);
    else return 1; //если оба нулевые, то равны однозначно
}

bool Rational_number::operator != (const Rational_number & f) const 
{
    return (a != f.a) || (b != f.b) || (sign != f.sign);
}

Rational_number Rational_number::operator-() const
{
	Rational_number res = (*this);
	res.sign = !res.sign;
	res.make_canonical();
	return res;
}

Rational_number& Rational_number::operator ++ ()
{
    (*this) += 1;
    make_canonical();
	return (*this);
}

Rational_number Rational_number::operator ++ (int)
{
	Rational_number obj(a,b); //равен текущему
	(*this) += 1;
    make_canonical();
    return (obj);
}

Rational_number& Rational_number::operator -- ()
{
    (*this) -= 1;
    make_canonical();
    return (*this);
}

Rational_number Rational_number::operator -- (int)
{
	Rational_number obj(a,b);
    (*this) -= 1;
    make_canonical();
    return (obj);
}

Rational_number operator + (Rational_number arg1, Rational_number arg2)
{
    uint64_t a,b,c,d;
    int sign1;
    a = arg1.a * arg2.b; //части числителя
    b = arg2.a * arg1.b;
    c = arg1.b * arg2.b; //и знаменатель
    if (c > UINT32_MAX)
    {
        Exception ex((int)ERR_OVERFLOW); //ошибка переполнения
        throw(ex);
    }
    if(arg1.sign == 0)
    {
        if(arg2.sign == 1) 
        {
			d = (a>b) ? a-b:b-a; //берем модуль числителя
			sign1 = (a>b) ? 0:1; //проверяем знак дроби
		} 
        else 
        {
			d = a + b; //просто сложение
			sign1 = 0;
		} 
    }
    else
    {
        if(arg2.sign == 0) 
        {
			d = (a>b) ? a-b:b-a; //ан-ная проверка модуля и знака
			sign1 = (a>b) ? 1:0;
		}
        else 
        {
			d = a + b; //ан-ное суммирование
			sign1 = 1;
		} 
    }
    if (d > UINT32_MAX)
    {
        Exception ex((int)ERR_OVERFLOW); //ошибка переполнения
        throw(ex);
    }
    uint32_t denom = c;
    uint32_t num = d;
    Rational_number obj(num, denom); //сокращение результата здесь же
    obj.sign = sign1;
    obj.make_canonical(); //дополнительная проверка для знака, если 0
    return obj;
}

Rational_number& Rational_number::operator += (Rational_number arg)
{ 
    (*this)=(*this)+arg; //не изобретаем велосипед, а пользуемся нашим методом
    return (*this);
}

Rational_number operator - (Rational_number arg1, Rational_number arg2)
{
    Rational_number arg_2 = arg2;
    if (arg_2.sign) arg_2.sign = 0; //меняем знак
    else arg_2.sign = 1;
    Rational_number obj = arg1 + arg_2; //эквивалентно obj = arg1 + (-arg2)
    return obj;
}

Rational_number& Rational_number::operator -= (Rational_number arg)
{
    (*this)=(*this)-arg;
    return (*this);
}

Rational_number operator * (Rational_number arg1, Rational_number arg2)
{
    uint64_t c,d;
    c = arg1.a * arg2.a; //числитель
    d = arg1.b * arg2.b; //знаменатель
    if ((c > UINT32_MAX)||(d > UINT32_MAX))
    {
        Exception ex((int)ERR_OVERFLOW); //ошибка переполнения
        throw(ex);
    }
    uint32_t num = c;
    uint32_t denom = d;
    Rational_number obj(num, denom); //по умолчанию +
    if (arg1.sign != arg2.sign) obj.sign = 1; //иногда нужен -
    return obj;   
} 

Rational_number& Rational_number::operator *= (Rational_number arg)
{
    (*this)=(*this)*arg;
    return (*this);
}

Rational_number operator / (Rational_number arg1, Rational_number arg2)
{
    if (arg2.a == 0)
    {
        Exception ex((int)ERR_ZERO_DIVISION, arg1, arg2); //ошибка деления на ноль
        throw(ex);
    }
    uint32_t c = arg2.b; //меняем местами части дроби
    arg2.b = arg2.a;
    arg2.a = c;
    Rational_number obj = arg1 * arg2; //эквивалентно obj = arg1 * (1/arg2)
    return obj;
}    

Rational_number& Rational_number::operator /= (Rational_number arg)
{
    (*this)=(*this)/arg;
    return (*this);
}

char* Rational_number::to_string() const
{
    char* str = (char*)malloc(64*sizeof(char));
    if (str == NULL)
    {
        Exception ex((int)ERR_MALLOC); //ошибка выделения памяти
        throw(ex);	
	}
    if (a == 0)
    {
        sprintf(str,"%d ",0);
        return str;
    }
    if (b == 1)
    {
        long long int a1 = a;
        if (sign) sprintf(str,"-%lld ",a1);
        else sprintf(str,"%lld ",a1);
        return str;
    }
    long long int a1 = a;
    long long int b1 = b;
    if (sign) sprintf(str,"-%lld/%lld ", a1, b1);
    else sprintf(str,"%lld/%lld ", a1, b1);
    return str;
}

