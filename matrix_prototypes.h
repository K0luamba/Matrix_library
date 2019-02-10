class Matrix
{
    int length_x, length_y; //кол-во строк(x) и столбцов(y)
    int num_elements; //на столько элементов выделена память
    int *x, *y; //координаты
    Rational_number *mas; //и элементы по ним
public:
	Matrix(); //конструктор по умолчанию
	Matrix(int type, size_t len_x, size_t len_y); //конструктор для нулей и единиц
    Matrix(size_t len); //конструктор единичной матрицы
    Matrix(Vector arg, int type = 0); //конструктор из вектора, 0 = вертикальный, 1 = горизонтальный
    Matrix(const char* filename); //конструктор из файла
    Matrix& operator = (const Matrix &arg); //присваивание
    Matrix(const Matrix &arg); //и конструктор копирования
    Rational_number& operator () (int m, int n); //взятие ссылки на элемент
    char* to_string() const; //преобразование в текст
    void write(const char* filename) const; //запись содержимого матрицы в файл
    void write_long(const char* filename) const; //"конвертер" - запись в бинарник с нулями
    Rational_number operator [] (Matrix_coords arg) const; //взятие одного элемента
    Vector operator[](Matrix_row_coord arg) const; //взятие строки
    Vector operator[](Matrix_column_coord arg) const; //взятиие столбца
    Matrix operator- () const; //унарный минус
    friend Matrix operator+ (Matrix arg1, Matrix arg2); //и прочие операции
    friend Matrix operator+ (Matrix arg1, Rational_number arg2);
    friend Matrix operator+ (Matrix arg1, int arg2);
    friend Matrix operator- (Matrix arg1, Matrix arg2);
    friend Matrix operator- (Matrix arg1, Rational_number arg2);
    friend Matrix operator- (Matrix arg1, int arg2);
    friend Matrix operator* (Matrix arg1, Matrix arg2);
    friend Matrix operator* (Vector arg1, Matrix arg2);
    Matrix operator ^ (uint32_t k);
    void make_canonical(); //убирает место, выделенное под нули
    void show() const; //вывод в прямоугольной форме
	~Matrix(); 
    friend Vector_cover_row;
    friend Vector_cover_column;
};

class Matrix_coords //набор классов для корректной работы операции []
{
    int row, col;
public:
    Matrix_coords();
    Matrix_coords(int x, int y);
    friend Matrix;
};

class Matrix_row_coord
{
    int row;
public:
    Matrix_row_coord();
    Matrix_row_coord(int arg);
    friend Matrix;
};

class Matrix_column_coord
{
    int col;
public:
    Matrix_column_coord();
    Matrix_column_coord(int arg);
    friend Matrix;
};

class Vector_cover_row
{
    Matrix* m; //обертка для этой матрицы
    int row; //ассоциированная с этим столбцом
public:
    Vector v; //и ее значения
    Vector_cover_row(Matrix* arg, int x); //конструктор для матрицы и ее строки
    void sync_from(); //запись в обертку
    void sync_to(); //запись в матрицу
    ~Vector_cover_row();
};

class Vector_cover_column
{
    Matrix* m; //обертка для этой матрицы
    int col; //ассоциированная с этим столбцом
public:
    Vector v; //и ее значения
    Vector_cover_column(Matrix* arg, int y); //конструктор для матрицы и ее строки
    void sync_from(); //запись в обертку
    void sync_to(); //запись в матрицу
    ~Vector_cover_column();
};
