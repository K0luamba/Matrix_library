#include <stdint.h>

class Exception;
class Vector;
class Matrix;
class Matrix_coords;
class Matrix_row_coord;
class Matrix_column_coord;
class Vector_cover_row;
class Vector_cover_column;

class Rational_number
{
    uint32_t a; //числитель
    uint32_t b; //и знаменатель
    int sign; //знак: 0 +, 1 -
public:
    void show() const; //вывод числа
    Rational_number& operator = (const Rational_number& arg); //оператор присваивания
    Rational_number(); //конструктор по умолчанию
    Rational_number(uint32_t a1, uint32_t b1 = 1); //конструктор из 2х беззнаковых чисел
    Rational_number(int a1); //конструктор от целого знакового, нужен для операций
    Rational_number(const char* s_a,const char* s_b); //конструктор из 2х строковых констант
    Rational_number(const char* str); //конструктор из строки
    void make_canonical(); //сокращение дроби
    uint32_t NOD(const uint32_t& a1,const uint32_t& b1) const; //вычисление НОДа
    Rational_number get_number_part() const; //взятие целой части
    Rational_number get_fractional_part() const; //взятие дробной части
    void round(); //округление до ближайшего целого
    void floor(); //округление до меньше или равного
    explicit operator short() const; //приведение к целому [2Б]
    explicit operator int() const; //приведение к целому [4Б]
    explicit operator long int() const; //приведение к целому [8Б]
    explicit operator double() const; //приведение к вещественному
    bool operator < (const Rational_number & f) const; //сравнения на любой вкус
    bool operator <= (const Rational_number & f) const;
    bool operator > (const Rational_number & f) const;
    bool operator == (const Rational_number & f) const;
    bool operator >= (const Rational_number & f) const;
    bool operator != (const Rational_number & f) const;
    Rational_number operator-() const; //унарный минус
    Rational_number& operator ++ (); //+1 префиксный
    Rational_number operator ++ (int); //+1 постиксный
    Rational_number& operator -- (); //-1 префиксный
    Rational_number operator -- (int); //-1 постиксный
    friend Rational_number operator + (Rational_number arg1, Rational_number arg2); //еще всякие операции
    Rational_number& operator += (Rational_number arg);
    friend Rational_number operator - (Rational_number arg1, Rational_number arg2);
    Rational_number& operator -= (Rational_number arg);
    friend Rational_number operator * (Rational_number arg1, Rational_number arg2);
    Rational_number& operator *= (Rational_number arg);
    friend Rational_number operator / (Rational_number arg1, Rational_number arg2);
    Rational_number& operator /= (Rational_number arg);
    char* to_string() const; //преобразование в текст
    friend Vector;
	friend Matrix;
};
