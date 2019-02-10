struct list_node
{
    Rational_number value; //ненулевой элемент вектора
    int key; //с таким номером
    struct list_node *next;
};

class Vector
{
    int num_elements; //всего элементов в векторе
    struct list_node *head;
public:
	Vector(); //по умолчанию
    Vector(int type, size_t length); //заполнение нулями (type=0) или единицами (type=1)
    Vector(const char* filename); //конструктор из файла
    void set_num(int num); //сеттер длины вектора
    ~Vector();
	void show() const; //вывод: 1 строка = 1 элемент
	void show1() const; //вывод в одну строку c переносом в конце
	Vector& operator=(const Vector& obj); //оператор присваивания
    Vector(const Vector& obj); //конструктор копирования
    char* to_string() const; //преобразование в текст
    Rational_number operator[](int idx) const; //доступ к элементу вектора
    Rational_number& operator ()(int idx); //для добавления элемента или изменение существующего 
    void write(const char* filename) const; //запись содержимого вектора в файл
    Vector operator-() const; //унарный минус
    friend Vector operator+(Vector v, Rational_number arg); //различные операции
    friend Vector operator+(Rational_number arg, Vector v);
    friend Vector operator-(Vector v, Rational_number arg);
    friend Vector operator*(Vector v, Rational_number arg);
    friend Vector operator*(Rational_number arg, Vector v);
    friend Vector operator/(Vector v, Rational_number arg);
    friend Vector operator+(Vector v1, Vector v2);
    friend Vector operator-(Vector v1, Vector v2);
    friend Rational_number operator*(Vector v1, Vector v2);
    friend Matrix;
    friend Vector_cover_row;
    friend Vector_cover_column;
};
