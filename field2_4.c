/* order 16 implementation of field.h interface */
/* GF(2^4) with decimal 19 as primitive */

#include <stdlib.h>
#include "field.h"

static int order = 16;

int getOrder()
{
    return order;
}

Field add(Field a, Field b)
{
    return (a ^ b) % order;
}

Field mult(Field a, Field b)
{
    int p = 0;
    int i;
    int carry;
    for (i = 0; i < 4; i++) {
        if (b & 1)  
            p ^= a;
        carry = (a & 8); 
        a <<= 1;
        if (carry) 
            a ^= 19; /* x^4 + x + 1 */
        b >>= 1;
    }   
    return p % order;
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
    return (a == 0);
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
    int i;
    
    for (i = 0; i < n; i++)
        a[i] = add(a[i], neg(b[i]));
    
    return a;
}

void vmult(Field *a, Field x, int n)
{
    int i;
    
    for(i = 0; i < n; i++)
        a[i] = mult(a[i], x);
}

char * tohexstr(Field *a, int n)
{
    int i, m;
    char *str;
    
    if(n % 2 == 0) {
        m = n + 1;
        str = malloc(m * sizeof(char));
        str[n] = '\0';
        
    } else {  /* this is because we want an even number of hexadecimal digits */
        m = n + 2;
        str = malloc(m * sizeof(char));
        str[0] = '0';
        str[n+1] = '\0';
    }
    
    for (i = 0; i < n; ++i) {
        if (a[i] < 10)
            str[m-(i+2)] = '0' +  (char)(a[i]);
        else
            str[m-(i+2)] = 'A' +  (char)(a[i]) - 10;
    }
    
    return str;
}
