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

Matrix_column_coord::Matrix_column_coord()
{
    col = 0;
}

Matrix_column_coord::Matrix_column_coord(int arg)
{
    col = arg;
}

Matrix_row_coord::Matrix_row_coord()
{
    row = 0;
}

Matrix_row_coord::Matrix_row_coord(int arg)
{
    row = arg;
}

Matrix_coords::Matrix_coords()
{
    row = 0;
    col = 0;
}

Matrix_coords::Matrix_coords(int x, int y)
{
    row = x;
    col = y;
}

Vector_cover_row::Vector_cover_row(Matrix* arg, int x)
{
    m = arg;
    if ((x < 0)||(x >= m->length_x))
    {
        Exception ex((int)ERR_OUT_OF_RANGE); 
        throw(ex);
    }
    row = x;
}

void Vector_cover_row::sync_from()
{
    v = (*m)[Matrix_row_coord(row)]; //пользуемся нашей операцией взятия строки
    return;
}

void Vector_cover_row::sync_to()
{
    for(int i=0; i < v.num_elements; i++) //проходимся по элементам вектора
    {
        if (((*m)[Matrix_coords(row, i)] == 0) && (v[i] == 0))
        {
            continue; //чтобы не выделять места на нулевые
        }
        (*m)(row, i) = v[i]; //непосредственно вставка элемента
    }
    m -> make_canonical(); //если нужно, убираем нулевые элементы
    return;
}

Vector_cover_row::~Vector_cover_row()
{
    m=NULL;
}

Vector_cover_column::Vector_cover_column(Matrix* arg, int y)
{
    m = arg;
    if ((y < 0)||(y >= m->length_y))
    {
        Exception ex((int)ERR_OUT_OF_RANGE); 
        throw(ex);
    }
    col = y;
}

void Vector_cover_column::sync_from()
{
    v = (*m)[Matrix_column_coord(col)]; //пользуемся нашей операцией взятия строки
    return;
}

void Vector_cover_column::sync_to()
{
    for(int i=0; i < v.num_elements; i++) //проходимся по элементам вектора
    {
        if (((*m)[Matrix_coords(i, col)] == 0) && (v[i] == 0))
        {
            continue; //чтобы не выделять места на нулевые
        }
        (*m)(i, col) = v[i]; //непосредственно вставка элемента
    }
    m -> make_canonical(); //если нужно, убираем нулевые элементы
    return;
}

Vector_cover_column::~Vector_cover_column()
{
    m=NULL;
}

Matrix::Matrix()
{
    length_x = 0;
    length_y = 0;
    num_elements = 0;
    x = (int*)malloc(sizeof(int)); //выделяем сразу, чтобы можно было реаллочить
    y = (int*)malloc(sizeof(int));
    mas = (Rational_number*)malloc(sizeof(Rational_number));
}

Matrix::Matrix(int type, size_t len_x, size_t len_y)
{
    if ((type != 0) && (type != 1))
	{
        Exception ex((int)ERR_INPUT); //ошибка ввода
        throw(ex);	
	}
    length_x = len_x;
    length_y = len_y;
    if (type == 0)
    {
        num_elements = 0;
        x = (int*)malloc(sizeof(int)); 
        y = (int*)malloc(sizeof(int));
        mas = (Rational_number*)malloc(sizeof(Rational_number)); 
        if ((x == NULL) || (y == NULL) || (mas == NULL))
        {
            Exception ex((int)ERR_MALLOC); //ошибка выделения памяти
            throw(ex);	
	    }       
    }
    if (type == 1)
    {
        num_elements = len_x * len_y;
        x = (int*)malloc((num_elements+1)*sizeof(int));  //кол-во элементов в массивах одинаковое
        y = (int*)malloc((num_elements+1)*sizeof(int));
        mas = (Rational_number*)malloc((num_elements+1)*sizeof(Rational_number)); 
        if ((x == NULL) || (y == NULL) || (mas == NULL))
        {
            Exception ex((int)ERR_MALLOC); //ошибка выделения памяти
            throw(ex);	
	    }
        int k = 0; //по этому индексу проходимся паралельно по всем массивам
        for (int i = 0; i < len_x; i++)
        {
            for (int j = 0; j < len_y; j++)
            {
                x[k] = i;
                y[k] = j;
                mas[k] = 1;
                k++;
            }        
        }
    }
}

Matrix::Matrix(size_t len)
{
    length_x = len;
    length_y = len;
    num_elements = len;
    x = (int*)malloc((num_elements+1)*sizeof(int));  
    y = (int*)malloc((num_elements+1)*sizeof(int));
    mas = (Rational_number*)malloc((num_elements+1)*sizeof(Rational_number));
    if ((x == NULL) || (y == NULL) || (mas == NULL))
    {
        Exception ex((int)ERR_MALLOC); //ошибка выделения памяти
        throw(ex);	
	} 
    for(int i=0; i < len; i++) 
    {
        x[i] = i; //движемся по диагональным элементам
        y[i] = i;
        mas[i] = 1;
    }
}

Matrix::Matrix(Vector arg, int type)
{
    if ((type != 0) && (type != 1))
	{
        Exception ex((int)ERR_INPUT); //ошибка ввода
        throw(ex);	
	}
    num_elements = 0; //по умолчанию пустой
    x = (int*)malloc(sizeof(int));  
    y = (int*)malloc(sizeof(int));
    mas = (Rational_number*)malloc(sizeof(Rational_number));
    if(type == 0)
    {
        length_y = 1; //и больше не меняем здесь
        length_x = arg.num_elements;
        for(int i=0; i < arg.num_elements; i++)
        {
            if(arg[i] != 0) //заносим только ненулевые
            {
                num_elements++; //отмечаем добавление
                x = (int*)realloc(x,(num_elements+1)*sizeof(int));  
                y = (int*)realloc(y,(num_elements+1)*sizeof(int));
                mas = (Rational_number*)realloc(mas,(num_elements+1)*sizeof(Rational_number));
                if ((x == NULL) || (y == NULL) || (mas == NULL))
                {
                    Exception ex((int)ERR_MALLOC); //ошибка выделения памяти
                    throw(ex);	
	            } 
                y[num_elements-1] = 0; //изменяем всегда последний элемент в каждом
                x[num_elements-1] = i;
                mas[num_elements-1] = arg[i];
            }
        }
    }
    if(type == 1)
    {
        length_x = 1; //и больше не меняем здесь
        length_y = arg.num_elements;
        for(int i=0; i < arg.num_elements; i++)
        {
            if(arg[i] != 0) //заносим только ненулевые
            {
                num_elements++; //отмечаем добавление
                x = (int*)realloc(x,(num_elements+1)*sizeof(int));  
                y = (int*)realloc(y,(num_elements+1)*sizeof(int));
                mas = (Rational_number*)realloc(mas,(num_elements+1)*sizeof(Rational_number));
                if ((x == NULL) || (y == NULL) || (mas == NULL))
                {
                    Exception ex((int)ERR_MALLOC); //ошибка выделения памяти
                    throw(ex);	
	            } 
                x[num_elements-1] = 0; //изменяем всегда последний элемент в каждом
                y[num_elements-1] = i;
                mas[num_elements-1] = arg[i];
            }
        }
    }
}

Matrix::Matrix(const char* filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
         Exception ex((int)ERR_FOPEN); //ошибка открытия файла
         throw(ex);	
	}
    Rational_number val; //буфер для считывания значений матрицы
    char *str = NULL, c;
    char check[7];
    int i = 0, pos = 0, cnt, k = 0;	
    bool first_string = 1;
    num_elements = 0;
    length_x = 0;
    length_y = 0;
    x = (int*)malloc(sizeof(int));  
    y = (int*)malloc(sizeof(int));
    mas = (Rational_number*)malloc(sizeof(Rational_number));
	while((c = (fgetc(fp))) != EOF)
	{	
		i++;
		if (c == '\n')
		{ 	
            cnt = 0; //нужен для прохождения по текущей строке
			str = (char*)malloc(i * sizeof(char) + sizeof(char)); //обрабатываем файл построчно
			fseek(fp, pos, SEEK_SET);
			fgets(str, i + 1, fp);
            if (first_string) //если еще не прочитали строку с кол-вом строк и столбцов
            {               
                while ((str[cnt] == ' ') || (str[cnt] == '\t')) cnt++; //пропускаем пустые места
                if ((str[cnt] == '#') || (str[cnt] == '\n')) //проверка на комментарий || пустую строку
                {
                    free(str);
			        i = 0;
			        pos = ftell(fp);
                    continue;
                }
                for(int j=0; j<6; j++)
                {
					if ((str[cnt] == '\0')||(str[cnt] == '\n')) //если не дочитали слово
					{
						free(x);
						free(y);
						free(mas);
						fclose(fp);
						free(str);
						Exception ex((int)ERR_IN_FILE);  //ошибка в файле ввода
						throw(ex);	        
					}
                    check[j] = str[cnt];
                    cnt++;
                }
                check[6] = '\0';
                if (strcmp(check, "matrix")!=0) 
                {
					 free(x);
					 free(y);
					 free(mas);
                     fclose(fp);
                     free(str);
                     Exception ex((int)ERR_IN_FILE);  //ошибка в файле ввода
                     throw(ex);	        
                }
                while ((str[cnt] == ' ') || (str[cnt] == '\t')) cnt++; //пропускаем пустые места
                while ((str[cnt] != '\n') && (str[cnt]  != ' ') && (str[cnt] != '\t') && (str[cnt] != '#'))  //здесь подсчет числа строк
                {
                    if (isdigit(str[cnt]) == 0)
                    {
					    free(x);
					    free(y);
					    free(mas);
                        fclose(fp);
                        free(str);
                        Exception ex((int)ERR_IN_FILE); //ошибка в файле ввода
                        throw(ex);  
                    }
                    first_string = 0;
                    length_x = length_x*10 + (str[cnt] - '0');
                    cnt++;
                }
                while ((str[cnt] == ' ') || (str[cnt] == '\t')) cnt++; //пропускаем пустые места
                while ((str[cnt] != '\n') && (str[cnt]  != ' ') && (str[cnt] != '\t') && (str[cnt] != '#'))  //и тут подсчет числа столбцов
                {
                    if (isdigit(str[cnt]) == 0)
                    {
					    free(x);
					    free(y);
					    free(mas);
                        fclose(fp);
                        free(str);
                        Exception ex((int)ERR_IN_FILE); //ошибка в файле ввода
                        throw(ex);  
                    }
                    first_string = 0;
                    length_y = length_y*10 + (str[cnt] - '0');
                    cnt++;
                }
                while ((str[cnt] == ' ') || (str[cnt] == '\t')) cnt++; //пропускаем пустые места
                if(!((str[cnt] == '#') || (str[cnt] == '\n')) && (first_string == 0))
                {   
					free(x);
					free(y);
					free(mas);
                    fclose(fp);
                    free(str);
                    Exception ex((int)ERR_IN_FILE);  //ошибка в файле ввода
                    throw(ex);	
                }
            }
            else
			{
                while ((str[cnt] == ' ') || (str[cnt] == '\t')) cnt++; //пропускаем пустые места
                if ((str[cnt] == '#') || (str[cnt] == '\n')) //проверка на комментарий || пустую строку
                {
                    free(str);
			        i = 0;
			        pos = ftell(fp);
                    continue; //пропускаем ее, смотрим следующую
                }
                num_elements++; //ожидаем, что здесь еще один элемент
                x = (int*)realloc(x,(num_elements+1)*sizeof(int));  
                y = (int*)realloc(y,(num_elements+1)*sizeof(int));
                mas = (Rational_number*)realloc(mas,(num_elements+1)*sizeof(Rational_number));
                x[num_elements-1] = 0;
                y[num_elements-1] = 0;
                k = 0;
                while ((str[cnt] != '\n') && (str[cnt]  != ' ') && (str[cnt] != '\t') && (str[cnt] != '#'))  //подсчет x координаты
                {
                    if (isdigit(str[cnt]) == 0)
                    {
					    free(x);
					    free(y);
					    free(mas);
                        fclose(fp);
                        free(str);
                        Exception ex((int)ERR_IN_FILE); //ошибка в файле ввода
                        throw(ex);  
                    }
                    x[num_elements-1] = x[num_elements-1]*10 + (str[cnt] - '0');
                    str[cnt] = ' '; //параллельно затираем ключ для передачи в конструктор числа
                    cnt++;
                }
                while ((str[cnt] == ' ') || (str[cnt] == '\t')) cnt++; //пропускаем пустые места
                while ((str[cnt] != '\n') && (str[cnt]  != ' ') && (str[cnt] != '\t') && (str[cnt] != '#'))  //подсчет y координаты
                {
                    if (isdigit(str[cnt]) == 0)
                    {
					    free(x);
					    free(y);
					    free(mas);
                        fclose(fp);
                        free(str);
                        Exception ex((int)ERR_IN_FILE); //ошибка в файле ввода
                        throw(ex);  
                    }
                    y[num_elements-1] = y[num_elements-1]*10 + (str[cnt] - '0');
                    str[cnt] = ' '; //параллельно затираем ключ для передачи в конструктор числа
                    cnt++;
                }  
                if ((x[num_elements-1] > length_x-1) || (y[num_elements-1] > length_y-1)) 
				{
					free(x);
					free(y);
					free(mas);
                    fclose(fp);
                    free(str);
                    Exception ex((int)ERR_OUT_OF_RANGE); //ошибка выхода за диапазон
                    throw(ex);
				}       
                try
				{
					mas[num_elements-1] = str; //вызов конструктора от строки для данного числа
				}
                catch(...) //если в конструкторе что-то не так, надо все равно очистить текущие ресурсы
                {
					free(x);
					free(y);
					free(mas);
					fclose(fp);
                    free(str);
                    throw; 	  
                }
            }
            free(str);
			i = 0;
			pos = ftell(fp);
        }
    }
	str = (char*)malloc(i * sizeof(char) + 2 * sizeof(char)); //доходим до конца файла
	fseek(fp, pos, SEEK_SET);
	fgets(str, i + 1, fp);
    free(str);	  
    fclose(fp); 
}

Matrix& Matrix::operator = (const Matrix &arg)
{
    length_x = arg.length_x;
    length_y = arg.length_y;
    num_elements = arg.num_elements;
    x = (int*)realloc(x, (num_elements+1)*sizeof(int));
    y = (int*)realloc(y, (num_elements+1)*sizeof(int));
    mas = (Rational_number*)realloc(mas, (num_elements+1)*sizeof(Rational_number));
    if ((x == NULL) || (y == NULL) || (mas == NULL))
    {
        Exception ex((int)ERR_MALLOC); //ошибка выделения памяти
        throw(ex);	
	} 
    for (int i = 0; i < num_elements; i++)
    {
        x[i] = arg.x[i]; //основная часть в копировании 3х массивов
        y[i] = arg.y[i];
        mas[i] = arg.mas[i];
    }
    return *this;
}

Matrix::Matrix(const Matrix &arg)
{
    length_x = arg.length_x;
    length_y = arg.length_y;
    num_elements = arg.num_elements;
    x = (int*)malloc((num_elements+1)*sizeof(int));
    y = (int*)malloc((num_elements+1)*sizeof(int));
    mas = (Rational_number*)malloc((num_elements+1)*sizeof(Rational_number));
    if ((x == NULL) || (y == NULL) || (mas == NULL))
    {
        Exception ex((int)ERR_MALLOC); //ошибка выделения памяти
        throw(ex);	
	} 
    for (int i = 0; i < num_elements; i++)
    {
        x[i] = arg.x[i]; //основная часть в копировании 3х массивов
        y[i] = arg.y[i];
        mas[i] = arg.mas[i];
    }
}

Rational_number& Matrix::operator () (int m, int n)
{
    if ((m < 0) || (n < 0) || (m >= length_x) || (n >= length_y))
    {
        Exception ex((int)ERR_OUT_OF_RANGE); //ошибка выхода за диапазон
        throw(ex);
    }
    for(int k = 0; k < num_elements; k++)
    {
        if ((x[k] == m) && (y[k] == n)) return mas[k]; //если нашли элемент, возвращаем сразу ссылку
    }
    num_elements++; //иначе надо добавить еще один элемент
    x = (int*)realloc(x, (num_elements+1)*sizeof(int));
    y = (int*)realloc(y, (num_elements+1)*sizeof(int));
    mas = (Rational_number*)realloc(mas, (num_elements+1)*sizeof(Rational_number));
    if ((x == NULL) || (y == NULL) || (mas == NULL))
    {
        Exception ex((int)ERR_MALLOC); //ошибка выделения памяти
        throw(ex);	
	} 
    x[num_elements-1] = m;
    y[num_elements-1] = n;
    mas[num_elements-1] = 0; //по умолчанию нулевой
    return mas[num_elements-1]; 
}

char* Matrix::to_string() const
{
    char* str = (char*)malloc(sizeof(char)); 
    str[0] = '\0';
    char* tmp = (char*)malloc(64*sizeof(char)); //временный буфер для строки
    long long int a1;
    long long int b1;
    for (int i = 0; i < length_x; i++) //для удобства выводим последовательно
    {
        for (int j = 0; j < length_y; j++)
        {
            for(int k = 0; k < num_elements; k++)
            {
                if((x[k] == i) && (y[k] == j)) //если элемент такой есть - пишем его в строку
                {
                    a1 = mas[k].a;
                    b1 = mas[k].b;
	                if (mas[k].b == 1) //запись по аналогии с переводом в Rational_number, но с 2мя ключами
	                {
			            if (mas[k].sign) sprintf(tmp,"%d %d -%lld\n", x[k], y[k], a1);
			            else sprintf(tmp,"%d %d %lld\n",x[k], y[k], a1); 
		            }
	                else
	                {
			            if (mas[k].sign) sprintf(tmp,"%d %d -%lld/%lld\n",x[k], y[k], a1, b1);
			            else sprintf(tmp,"%d %d %lld/%lld\n",x[k], y[k], a1, b1);
		            }  
                    str = (char*) realloc(str, (strlen(str) + strlen(tmp) + 1)*sizeof(char));
                    strcat(str,tmp); //прикрепляем к итоговой строке
                    str[strlen(str)] = '\0'; 
                    break;
                }
            }
        }
    }
    free(tmp);
    return str;
}

void Matrix::write(const char* filename) const
{
    FILE *fd = fopen(filename, "w");
    if (fd == NULL)
    {
         Exception ex((int)ERR_FOPEN); //ошибка открытия файла
         throw(ex);	
	}
    char* s = (char*)malloc(20*sizeof(char));
    sprintf(s, "matrix %d %d\n", length_x, length_y);
    fprintf(fd, "%s", s);
    free(s);
    long long int a1;
    long long int b1;
    s = to_string(); //запись текущей матрицы в строку по стандарту
    fprintf(fd, "%s", s);
	free(s);
    fclose(fd);
    return;
}

void Matrix::write_long(const char* filename) const
{
    FILE *fd = fopen(filename, "wb"); 
    if (fd == NULL)
    {
         Exception ex((int)ERR_FOPEN); //ошибка открытия файла
         throw(ex);	
	}
    double xx = (double)length_x;
    double yy = (double)length_y;
    fwrite(&xx, sizeof(double), 1, fd); //сначала идут размеры матрицы
    fwrite(&yy, sizeof(double), 1, fd);
    double null = 0;
    double cash;
    bool found;
    for (int i = 0; i < length_x; i++)
    {
        for (int j = 0; j < length_y; j++)
        {
            found = 0;
            for(int k = 0; k < num_elements; k++) //ищем этот элемент
            {
                if((x[k] == i) && (y[k] == j)) //нашли - пишем в файл
                {   
                    cash = (double)mas[k]; //пользуемся нашим преобразованием
                    fwrite(&cash, sizeof(double), 1, fd);
                    found = 1;
                    break; 
                } 
            }
            if (found == 0) fwrite(&null, sizeof(double), 1, fd); //или записываем ноль
        }
    }
    fclose(fd);
    return;
}

Rational_number Matrix::operator[](Matrix_coords arg) const
{
    if ((arg.row >= length_x)||(arg.col >= length_y))
    {
        Exception ex((int)ERR_OUT_OF_RANGE); //ошибка выхода за диапазон
        throw(ex);
    }
    Rational_number res = (uint32_t)0; //если не найдем, то так и вернем 0
    for (int i = 0; i < num_elements; i++)
    {
        if ((x[i] == arg.row) && (y[i] == arg.col)) res = mas[i];
    }
    return res;
}

Vector Matrix::operator[](Matrix_row_coord arg) const
{
    Vector res(0,length_y); //пользуемся нашим конструктором вектора с нулями
    if ((arg.row < 0) || (arg.row >= length_x))
    {
        Exception ex((int)ERR_OUT_OF_RANGE); //ошибка выхода за диапазон
        throw(ex);
    }
    for(int i=0; i < length_y; i++) //проходимся по нужной строке
    {   
        if ((*this)[Matrix_coords(arg.row, i)] != 0)
        {
            res(i) = (*this)[Matrix_coords(arg.row, i)];
        }
    }
    return res;
}

Vector Matrix::operator[](Matrix_column_coord arg) const
{
    Vector res(0,length_x); //пользуемся нашим конструктором вектора с нулями
    if ((arg.col < 0) || (arg.col >= length_y))
    {
        Exception ex((int)ERR_OUT_OF_RANGE); //ошибка выхода за диапазон
        throw(ex);
    }
    for(int i=0; i < length_x; i++) //проходимся по нужному столбцу
    {   
        if ((*this)[Matrix_coords(i, arg.col)] != 0)
        {
            res(i) = (*this)[Matrix_coords(i, arg.col)];
        }
    }
    return res;
}

Matrix Matrix::operator-() const
{
	Matrix res = (*this);
	for(int k = 0; k < num_elements; k++)
	{   
		res.mas[k] = -res.mas[k]; //пользуемся унарным минусом из Rational number
	}	
	return res;
}

Matrix operator+ (Matrix arg1, Matrix arg2)
{
    if((arg1.length_x != arg2.length_x) || (arg1.length_y != arg2.length_y))
    {
         Exception ex((int)ERR_DIMENSION); //ошибка размерностей
         throw(ex);	   
    }
    Matrix res(0, arg1.length_x, arg1.length_y); //сохдаем нулевую матрицу без выделения памяти
    for (int i = 0; i < arg1.length_x; i++)
    {
        for (int j = 0; j < arg1.length_y; j++)
        {
            if(arg1[Matrix_coords(i,j)] + arg2[Matrix_coords(i,j)] != 0) //только на ненулевые выделяем память
            res(i,j) = arg1[Matrix_coords(i,j)] + arg2[Matrix_coords(i,j)]; 
        }
    }
    return res;
}

Matrix operator+ (Matrix arg1, Rational_number arg2)
{
    Matrix res(0, arg1.length_x, arg1.length_y); //сохдаем нулевую матрицу без выделения памяти
    for (int i = 0; i < arg1.length_x; i++)
    {
        for (int j = 0; j < arg1.length_y; j++)
        {
            if(arg1[Matrix_coords(i,j)] + arg2 != 0) //только на ненулевые выделяем память
            res(i,j) = arg1[Matrix_coords(i,j)] + arg2; 
        }
    }
    return res;
}

Matrix operator+ (Matrix arg1, int arg2)
{
    Matrix res(0, arg1.length_x, arg1.length_y); //сохдаем нулевую матрицу без выделения памяти
    for (int i = 0; i < arg1.length_x; i++)
    {
        for (int j = 0; j < arg1.length_y; j++)
        {
            if(arg1[Matrix_coords(i,j)] + arg2 != 0) //только на ненулевые выделяем память
            res(i,j) = arg1[Matrix_coords(i,j)] + arg2; 
        }
    }
    return res;
}

Matrix operator- (Matrix arg1, Matrix arg2)
{
    arg2 = -arg2;
    Matrix res = arg1 + arg2; //пользуемся нашим плюсом
    return res;
}

Matrix operator- (Matrix arg1, Rational_number arg2)
{
    arg2 = -arg2;
    Matrix res = arg1 + arg2; //пользуемся нашим плюсом
    return res;
}

Matrix operator- (Matrix arg1, int arg2)
{
    arg2 = -arg2;
    Matrix res = arg1 + arg2; //пользуемся нашим плюсом
    return res;
}

Matrix operator* (Matrix arg1, Matrix arg2)
{
    if (arg1.length_y != arg2.length_x)
    {
         Exception ex((int)ERR_DIMENSION); //ошибка размерностей
         throw(ex);	   
    }
    Matrix res(0, arg1.length_x, arg2.length_y); //такого размера новая матрица
    Rational_number rat;
    for (int i = 0; i < arg1.length_x; i++) //проход с учетом размеров новой матрицы
    {
        for (int j = 0; j < arg2.length_y; j++) 
        {
            rat = (uint32_t)0;
            for (int k = 0; k < arg1.length_y; k++) rat += arg1[Matrix_coords(i, k)] * arg2[Matrix_coords(k, j)];
            if (rat != 0) res(i,j) = rat; //ненулевые заносим
        }
    }
    return res;
}

Matrix operator* (Vector arg1, Matrix arg2)
{
    Matrix res(arg1,1); //вызов конструктора от вектора, горизонтальный
    res = res * arg2; //проверка размерностей здесь же
    return res;
}

Matrix Matrix::operator ^ (uint32_t k)
{
    if (length_x != length_y)
    {
         Exception ex((int)ERR_DIMENSION); //ошибка размерностей
         throw(ex);	   
    }
    Matrix res(length_x);
    if (k == 0) return res; //матрица в нулевой степени - единичная
    res = *this;
    for (int i = 0; i < k - 1; i++) res = res * res;
    return res;
}

void Matrix::make_canonical()
{
    Matrix cash(0, length_x, length_y); //подготавливаем ресурсы под новую матрицу тех же размеров
    for (int i = 0; i < length_x; i++)
    {
        for (int j = 0; j < length_y; j++)
        {
            if((*this)[Matrix_coords(i,j)] != 0) cash(i,j) = (*this)[Matrix_coords(i,j)]; //в нее добавляем все ненулевые элементы из данной
        }
    }
    (*this) = cash;
    return;
}

void Matrix::show() const
{
    cout << "матрица " << length_x << " x " << length_y << ":" << endl;
    cout << "[System]:выделено памяти на " << num_elements << " элементов" << endl;
    bool found = 0; //флаг вывода для каждого элемента
    for (int i = 0; i < length_x; i++)
    {
        cout << "[" << i << "] ";
        for (int j = 0; j < length_y; j++)
        {
            found = 0;
            for(int k = 0; k < num_elements; k++)
            {   
                if((x[k] == i) && (y[k] == j))
                {
                    mas[k].show(); //делаем вывод элемента без переноса строки
                    found = 1;
                    break;
                }
            }
            if (found == 0) cout << "0 "; //или просто выводим 0
        }
        cout << endl; //готовы печатать новую строку матрицы
    }
    return;
}

Matrix::~Matrix()
{
    if (x) free(x); //уточнение: использовались массивы в т.ч. для простоты очистки
    if (y) free(y);
    if (mas) free(mas);
}
