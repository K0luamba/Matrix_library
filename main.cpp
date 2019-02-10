#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <stdint.h>
#include <cstring>

#include "rational_prototypes.h"
#include "exception_prototypes.h"
#include "vector_prototypes.h"
#include "matrix_prototypes.h"

using namespace std;

int main()
{
    try
    {
        Rational_number a("1/5");
        a = (a + "2/5") * 5;
        a.show();
        cout << endl;
        Vector r(0, 3);
        r(0) = 1;
        r(1) = 2;
        r(2) = 3;
        Vector r1(0, 3);
        r1(0) = 4;
        r1(1) = 5;
        r1(2) = 6;
        a = r * r1;
        a.show();
        cout << endl;
        Matrix f(4);
        f.show();
        f = f - 5;
        f.show();
    }   
    catch(...)
    {
    }
    return 0;
}
