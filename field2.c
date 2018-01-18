/* order 2 implementation of field.h interface */

#include <stdlib.h>
#include "field.h"

static int order = 2;

int getOrder()
{
    return order;
}

Field add(Field a, Field b)
{
    return a ^ b;
}

Field mult(Field a, Field b)
{
    return a & b;
}

Field zero()
{
    return 0;
}

Field neg(Field a)
{
    return a;
}

int isZero(Field a)
{
    return !a;
}

Field * vadd(Field *a, Field *b, int n)
{
    int i;
    
    for (i = 0; i < n; i++)
        a[i] = add(a[i], b[i]);
    
    return a;
}

Field * vsub(Field *a, Field *b, int n)
{
    return vadd(a, b, n); /* addition and subtraction are equivalent in GF(2) */
}

void vmult(Field *a, Field x, int n)
{
    int i;
    
    for(i = 0; i < n; i++)
        a[i] = mult(a[i], x);
}

char * tohexstr(Field *a, int n)
{
    int i, j, m, p;
    char s;
    char *str;
    
    p = m = (n + 3) / 4;
    
    if(m % 2 == 0)
        str = malloc((m + 1) * sizeof(char));        
    else { /* this is because we want an even number of hexadecimal digits */
        m++;
        str = malloc((m + 1) * sizeof(char));
        str[0] = '0';
    }
    str[m] = '\0';
    
    for (i = 0; i < p; ++i) {
        int q = 4*i, t;
        
        s = 0;
        t = 1;
        for (j = 0; j < 4 && (q + j) < n; ++j) {
            s += a[q + j]*t;
            t *= 2;
        }
    
        if (s < 10)
            str[m-(i+1)] = '0' +  s;
        else
            str[m-(i+1)] = 'A' +  s - 10;
    }
    
    return str;
}
