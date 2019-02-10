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

Vector::Vector()
{
	num_elements = 0;
	head = NULL;
}

Vector::Vector(int type, size_t length)
{
    if ((type != 0) && (type != 1))
	{
        Exception ex((int)ERR_INPUT); //ошибка ввода
        throw(ex);	
	}	
    num_elements = length;
    if (type == 1) //заполняем единицами
    {
        head = (list_node*)malloc(sizeof(list_node));
        if (head == NULL)
        {
            Exception ex((int)ERR_MALLOC); //ошибка выделения памяти
            throw(ex);	
		}
        list_node* tmp = head;
        for (size_t i=0; i<length; i++)
        {
			tmp -> key = i;
			tmp -> value = "1/1"; //делаем через конструктор рационального числа
			tmp -> next = (list_node*)malloc(sizeof(list_node));
			if (tmp -> next == NULL)
			{
				Exception ex((int)ERR_MALLOC); //ошибка выделения памяти
				throw(ex);
			}
			tmp = tmp -> next;
		}
		tmp -> next = NULL;
    }
    else head = NULL; //нули не пишем
}

Vector::Vector(const char* filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
         Exception ex((int)ERR_FOPEN); //ошибка открытия файла
         throw(ex);	
	}
    Vector a; //вспомогательный вектор, сначала пустой
    Rational_number val; //буфер для считывания значений вектора
    char *str = NULL, c;
    char check[7];
    int i = 0, pos = 0, cnt, k = 0;	
    bool first_string = 1;
    num_elements = 0;
    head = NULL;
	while((c = (fgetc(fp))) != EOF)
	{	
		i++;
		if (c == '\n')
		{ 	
            cnt = 0; //нужен для прохождения по текущей строке
			str = (char*)malloc(i * sizeof(char) + sizeof(char)); //обрабатываем файл построчно
			fseek(fp, pos, SEEK_SET);
			fgets(str, i + 1, fp);
            if (first_string) //если еще не прочитали строку с кол-вом элементов
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
					if ((str[cnt] == '\n')||(str[cnt] == '\0'))
					{
						 fclose(fp);
						 free(str);
						 Exception ex((int)ERR_IN_FILE);  //ошибка в файле ввода
						 throw(ex);		
					}
                    check[j] = str[cnt];
                    cnt++;
                }
                check[6] = '\0';
                if (strcmp(check, "vector")!=0) 
                {
                     fclose(fp);
                     free(str);
                     Exception ex((int)ERR_IN_FILE);  //ошибка в файле ввода
                     throw(ex);	        
                }
                while ((str[cnt] == ' ') || (str[cnt] == '\t')) cnt++; //пропускаем пустые места
                while ((str[cnt] != '\n') && (str[cnt]  != ' ') && (str[cnt] != '\t') && (str[cnt] != '#'))  //подсчет числа элементов
                {
                    if (isdigit(str[cnt]) == 0)
                    {
                        fclose(fp);
                        free(str);
                        Exception ex((int)ERR_IN_FILE); //ошибка в файле ввода
                        throw(ex);  
                    }
                    first_string = 0;
                    num_elements = num_elements*10 + (str[cnt] - '0');
                    cnt++;
                }
                while ((str[cnt] == ' ') || (str[cnt] == '\t')) cnt++; //пропускаем пустые места
                if(((str[cnt] == '#') || (str[cnt] == '\n')) && (first_string == 0))
                {
                    a.set_num(num_elements); //меняем вспомогательный вектор     
                }
                else
                {   
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
                k = 0;
                while ((str[cnt] != '\n') && (str[cnt]  != ' ') && (str[cnt] != '\t') && (str[cnt] != '#'))  //подсчет ключа
                {
                    if (isdigit(str[cnt]) == 0)
                    {
                        fclose(fp);
                        free(str);
                        Exception ex((int)ERR_IN_FILE); //ошибка в файле ввода
                        throw(ex);  
                    }
                    k = k*10 + (str[cnt] - '0');
                    str[cnt] = ' '; //параллельно затираем ключ для передачи в конструктор числа
                    cnt++;
                }
                try
				{
					val = str;
				}
                catch(...) //если в конструкторе что-то не так, надо все равно очистить текущие ресурсы
                {
					fclose(fp);
                    free(str);
                    throw; 	  
                }
				a(k) = val; //заносим найденное число на нужное место
            }
            free(str);
			i = 0;
			pos = ftell(fp);
		}	
	}    
    (*this) = a;
	str = (char*)malloc(i * sizeof(char) + 2 * sizeof(char)); //доходим до конца файла
	fseek(fp, pos, SEEK_SET);
	fgets(str, i + 1, fp);
    free(str);	  
    fclose(fp);  
}

Vector::~Vector()
{
    if (head)
    {
	    struct list_node* cash = head; 
	    struct list_node* next = head;
	    while(cash != NULL)
	    {
		    next = next -> next;
		    free(cash);
		    cash = next;	
	    }
	    head = NULL;
    }	
}

void Vector::set_num(int num)
{
    num_elements = num;
    return;
}

void Vector::show() const
{
	if (num_elements == 0)
	{
		cout << "empty_vector" << endl;
		return;
	}
    cout << "вектор длины " << num_elements << ":"<< endl;
	list_node* tmp = head;
	int count = 0; //для подсчета числа выведенных элементов
	if (tmp != NULL)
	while(tmp -> next != NULL)
	{
		if (count < tmp -> key) //если нули перед ненулевым
		{
			while (count < tmp -> key)
			{
				cout << "[" << count << "]"<<'0'<<endl;
				count++;
			}
		}
		cout << "[" << tmp -> key << "]";
		tmp -> value.show(); //пользуемся выводом класса rational_number
		cout << endl;
		count++; //отметили, что вывели
		tmp = tmp -> next;
	}
	if (count < num_elements) //если в конце нули
	{
		while (count < num_elements)
		{
			cout << "[" << count << "]"<<'0'<<endl;
			count++;
		}
	}
	return;
}

void Vector::show1() const
{
	if (num_elements == 0)
	{
		cout << endl;
		return;
	}
	list_node* tmp = head;
	int count = 0; //для подсчета числа выведенных элементов
	if (tmp != NULL)
	while(tmp -> next != NULL)
	{
		if (count < tmp -> key) //если нули перед ненулевым
		{
			while (count < tmp -> key)
			{
				cout <<"0 ";
				count++;
			}
		}
		tmp -> value.show(); //пользуемся выводом класса rational_number
		count++; //отметили, что вывели
		tmp = tmp -> next;
	}
	if (count < num_elements) //если в конце нули
	{
		while (count < num_elements)
		{
			cout <<"0 ";
			count++;
		}
	}
	cout << endl;
	return;
}

Vector& Vector::operator=(const Vector& obj)
{
	(*this).~Vector(); //зачищаем предыдущее выделенное место
    num_elements = obj.num_elements;
    if (obj.head == NULL) 
    {
        head = NULL;
        return (*this); //для нулевых векторов
    }
    head = (list_node*)malloc(sizeof(list_node));
    if (head == NULL)
    {
        Exception ex((int)ERR_MALLOC); //ошибка выделения памяти
        throw(ex);	
	}
    list_node* tmp = head;
    list_node* tmp2 = obj.head;
    while (tmp2 -> next != NULL) //идем до конца obj
    {
        tmp -> key = tmp2 -> key;
        tmp -> value = tmp2 -> value;
        tmp -> next = (list_node*)malloc(sizeof(list_node)); //+1 элемент
        if (tmp -> next == NULL)
        {
            Exception ex((int)ERR_MALLOC); //ошибка выделения памяти
            throw(ex);	
	    }
        tmp = tmp -> next; //делаем сдвиг по обеим векторам
        tmp2 = tmp2 -> next;
    }
    tmp -> next = NULL;
    return (*this);
}

Vector::Vector(const Vector& obj)
{
    num_elements = obj.num_elements;
    if (obj.head == NULL) 
    {
        head = NULL;
        return; //для нулевых векторов
    }
    head = (list_node*)malloc(sizeof(list_node));
    if (head == NULL)
    {
        Exception ex((int)ERR_MALLOC); //ошибка выделения памяти
        throw(ex);	
	}
    list_node* tmp = head;
    list_node* tmp2 = obj.head;
    while (tmp2 -> next != NULL) //идем до конца obj
    {
        tmp -> key = tmp2 -> key;
        tmp -> value = tmp2 -> value;
        tmp -> next = (list_node*)malloc(sizeof(list_node)); //+1 элемент
        if (tmp -> next == NULL)
        {
            Exception ex((int)ERR_MALLOC); //ошибка выделения памяти
            throw(ex);	
	    }
        tmp = tmp -> next; //делаем сдвиг по обеим векторам
        tmp2 = tmp2 -> next;
    }
    tmp -> next = NULL;
}

char* Vector::to_string() const
{
    if (head == NULL)
    {
        char* str = (char*) malloc(sizeof(char));
        str[0] = '\0';
        return str;
    }
    char* strings[num_elements];
    list_node* tmp = head;
    int i = 0;
    long long int a1;
    long long int b1;
    while (tmp -> next != NULL) //идем до конца
    {
        a1 = tmp -> value.a;
        b1 = tmp -> value.b;
        strings[i] = (char*)malloc(64*sizeof(char)); //переводя каждое число в свою строку
        if (strings[i] == NULL)
        {
            Exception ex((int)ERR_MALLOC); //ошибка выделения памяти
            throw(ex);	
	    }
	    if (tmp -> value.b == 1) 
	    {
			if (tmp -> value.sign) sprintf(strings[i],"%d -%lld\n",tmp -> key, a1);
			else sprintf(strings[i],"%d %lld\n",tmp -> key, a1); 
		}
	    else
	    {
			if (tmp -> value.sign) sprintf(strings[i],"%d -%lld/%lld\n",tmp -> key, a1, b1);
			else sprintf(strings[i],"%d %lld/%lld\n",tmp -> key, a1, b1);
		}
        i++;
        tmp = tmp -> next;
    }
    int idx; //для прохождения по "внутреним" строкам
    int index = 0; //для прохождения по "длинной" строке
    char* str = (char*) malloc(i*64*sizeof(char));
    for (int j=0; j<i; j++) //массив строк переводим посимвольно в одну строку
    {
        idx = 0;
        while(strings[j][idx] != '\0')
        {
            str[index] = strings[j][idx];
            index++;
            idx++;
        }
    }
    str[index] = '\0';
    for (idx=0; idx<i; idx++)
    {
        free(strings[idx]);
    }
    return str;
}

Rational_number Vector::operator[](int idx) const
{
    if ((idx >= num_elements)||(idx < 0))
    {
        Exception ex((int)ERR_OUT_OF_RANGE); //ошибка выхода за диапазон
        throw(ex);
    }
    Rational_number res = "0";
    if (head == NULL) return res;
    list_node* tmp = head;
    while(tmp -> next != NULL)
    {
        if(tmp -> key == idx) //если уже нашли этот элемент
        {
            res = tmp -> value;
            break;
        }
        if (tmp -> key > idx) break; //незачем больше искать
        tmp = tmp -> next;
    }
    return res;
}

Rational_number& Vector::operator ()(int idx)
{
    if ((idx >= num_elements)||(idx < 0))
    {
        Exception ex((int)ERR_OUT_OF_RANGE); //ошибка выхода за диапазон
        throw(ex);
    } 
    list_node* tmp = head;
    if (tmp == NULL) //просто добавляем этот элемент первым
    {
        head = (list_node*)malloc(sizeof(list_node));
        if (head == NULL)
        {
            Exception ex((int)ERR_MALLOC); //ошибка выделения памяти
            throw(ex);	
	    }
        head -> key = idx;
        head -> value = "0";
        head -> next = (list_node*)malloc(sizeof(list_node));
        tmp = head -> next;
        tmp -> next = NULL;
        return head -> value;
    }
    list_node* prev = head;
    while(tmp -> next != NULL)   
    {
        if(idx == tmp -> key) //если такой элемент уже есть
        {
            return tmp -> value;
        }
        if (idx < tmp -> key) //если надо вставить новый между существующими
        {
            list_node* cash = tmp;
            tmp = (list_node*)malloc(sizeof(list_node));
            tmp -> key = idx;
            tmp -> value = "0";
            tmp -> next = cash;
            if (head == cash) head = tmp; //связка с предыдущим
            else prev -> next = tmp;
            return tmp -> value;
        }
        prev = tmp; //запоминаем предшествующий элмемент
        tmp = tmp -> next;
    }
    list_node* cash = tmp; //если надо привязать в конце списка
    tmp = (list_node*)malloc(sizeof(list_node));
    tmp -> key = idx;
    tmp -> value = "0";
    tmp -> next = cash;
    if (head == cash) head = tmp; //связка с предыдущим
    else prev -> next = tmp;
    return tmp -> value;
}

void Vector::write(const char* filename) const
{
    FILE *fd = fopen(filename, "w");
    if (fd == NULL)
    {
         Exception ex((int)ERR_FOPEN); //ошибка открытия файла
         throw(ex);	
	}
    char* s = (char*)malloc(20*sizeof(char));
    sprintf(s, "vector %d\n", num_elements);
    fprintf(fd, "%s", s);
    free(s);
    long long int a1;
    long long int b1;
    s = to_string(); //запись текущего вектора в строку по стандарту
    fprintf(fd, "%s", s);
	free(s);
    fclose(fd);
}

Vector Vector::operator-() const
{
	Vector res = (*this);
	list_node* tmp = res.head;
	while (tmp -> next != NULL)
	{
		tmp -> value = -tmp -> value; //тут используем унарный минус из Rational_number
		tmp = tmp -> next;
	}
	return res;
}

Vector operator+(Vector v, Rational_number arg)
{
	Vector res(0,v.num_elements);
	for(int i=0; i < v.num_elements; i++)
	{  
		if ((v[i] + arg) != "0") res(i) = v[i] + arg; //на нулевые место не выделяем 
	}
	return res;
}

Vector operator+(Rational_number arg, Vector v) //нужна для коммутативности сложения
{
    return v + arg;
}

Vector operator-(Vector v, Rational_number arg)
{
	Vector res(0,v.num_elements);
	for(int i=0; i < v.num_elements; i++)
	{
		if ((v[i] - arg) != "0") res(i) = v[i] - arg; //на нулевые место не выделяем 
	}
	return res;
}

Vector operator*(Vector v, Rational_number arg)
{
	Vector res(0,v.num_elements);
	for(int i=0; i < v.num_elements; i++)
	{
		if ((v[i] * arg) != "0") res(i) = v[i] * arg; //на нулевые место не выделяем 
	}
	return res;
}

Vector operator*(Rational_number arg, Vector v) //нужна для коммутативности умножения
{
    return v * arg; 
}

Vector operator/(Vector v, Rational_number arg)
{
	if (arg == "0") 
	{
        Exception ex((int)ERR_ZERO_DIVISION, v[0], arg); //ошибка деления на ноль
        throw(ex);
    }
	Vector res(0,v.num_elements);
	for(int i=0; i < v.num_elements; i++)
	{
		if ((v[i] / arg) != "0") res(i) = v[i] / arg; //на нулевые место не выделяем 
	}
	return res;
}

Vector operator+(Vector v1, Vector v2)
{
	if (v1.num_elements != v2.num_elements)
    {
         Exception ex((int)ERR_DIMENSION); //ошибка размерностей
         throw(ex);	
	}
	Vector res(0,v1.num_elements);
	for(int i=0; i < v1.num_elements; i++)
	{
		if ((v1[i] + v2[i]) != "0") res(i) = v1[i] + v2[i]; //на нулевые место не выделяем 
	}
	return res;
}

Vector operator-(Vector v1, Vector v2)
{
	if (v1.num_elements != v2.num_elements)
    {
         Exception ex((int)ERR_DIMENSION); //ошибка размерностей
         throw(ex);	
	}
	Vector res(0,v1.num_elements);
	for(int i=0; i < v1.num_elements; i++)
	{
		if ((v1[i] - v2[i]) != "0") res(i) = v1[i] - v2[i]; //на нулевые место не выделяем 
	}
	return res;
}

Rational_number operator*(Vector v1, Vector v2)
{
	if (v1.num_elements != v2.num_elements)
    {
         Exception ex((int)ERR_DIMENSION); //ошибка размерностей
         throw(ex);	
	}
	Rational_number res = (uint32_t)0;
	for(int i=0; i < v1.num_elements; i++)
	{
	    res += v1[i] * v2[i];
	}
	return res;
}

