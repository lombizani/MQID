/* sample naive implementation of Sakumoto, Shirai and Hiwatari's
 * public-key id. scheme based on the MQ problem
 * 
 * WARNING: this is a naive implementation made for academic purposes and
 * shouldn't be put to security sensitive use
 * 
 * NOTE: this implementation does not provide a mechanism to store scheme system
 * parameter data
 * 
 * by: Carlos De Bernardis
 */

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "mqid.h"

struct mqid {
    /* system parameters */
    int m; /* equations */
    int n; /* unknowns */
    
    /* coefficient matrixes */
    Field ***a; /* quadratic */
    Field **b; /* linear */
};

int getComStrSize(){
    return HEXHASHSIZE;
}

/* initialization */
MQIDScheme setupMQIDScheme (int m, int n)
{
    int i, j, l, row;
    int sizeA, sizeB;
    MQIDScheme s;
    
    sizeA = m * (n * n + n)/2;
    sizeB = n * m;
    
    /* alocacao de espaco e configuracao de ponteiros */
    s = malloc(sizeof(struct mqid));
    s->n = n;
    s->m = m;
    
    s->a = malloc(m * sizeof(Field **));
    s->a[0] = malloc(sizeB * sizeof(Field *));
    
    for (i = 1; i < m; i++) {
        s->a[i] = s->a[i-1] + n;
    }
    
    s->a[0][0] = malloc(sizeA * sizeof(Field));
    
    j = 1;
    row = 0;
    for (i = 0; i < m; i++) {
        while (j < n) {
            int set = i;
            if (row == n - 1)
                set--;
            s->a[i][j] = s->a[set][row] + (row + 1);
            if (row == n - 1)
                set++;
            j++; row++;
            row = row % n;
        }
        j = 0;
    }
    
    s->b = malloc(m * sizeof(Field *));
    s->b[0] = malloc(sizeB * sizeof(Field));
    
    for (i = 1; i < m; i++) {
        s->b[i] = s->b[i-1] + n;
    }

    for (l = 0; l < m; l++)
        for (i = 0; i < n; i++) {
            for (j = 0; j <= i; j++) {
                s->a[l][i][j] = rand() % getOrder();
            }
            s->b[l][i] = rand() % getOrder();
        }
    
    return s;
}

int MQID_getm(MQIDScheme s)
{
    return s->m;
}

int MQID_getn(MQIDScheme s)
{
    return s->n;
}

Field MQID_geta(MQIDScheme s, int l, int i, int j)
{
    /* boundcheck */
    if (l >= s->m || i >= s-> n || j > i)
        return -1;
    
    return s->a[l][i][j];
}

Field MQID_getb(MQIDScheme s, int l, int i)
{
    /* boundcheck */
    if (l >= s->m || i >= s->n)
        return -1;
    
    return s->b[l][i];
}

/* calcula y = F(x) */
MQID_key MQID_F(MQIDScheme s, MQID_key x)
{
    int l, i, j;
    MQID_key y;
    
    y = malloc(s->m * sizeof(Field));
    
    for (l = 0; l < s->m; l++) {
        y[l] = zero();
        for (i = 0; i < s->n; i++) {
            y[l] = add(y[l], mult(s->b[l][i], x[i]));
            for (j = 0; j <= i; j++)
                y[l] = add(y[l], mult(s->a[l][i][j], mult(x[i], x[j])));
        }
    }
    
    return y;
}

/* computes the polar form F(x+y)-F(x)-F(y) = G(x,y) */
MQID_key MQID_G(MQIDScheme s, MQID_key x, MQID_key y)
{
    int l, i, j;
    MQID_key g;
    
    g = malloc(s->m * sizeof(Field));
    
    for (l = 0; l < s->m; l++) {
        g[l] = zero();
        for (i = 0; i < s->n; i++)
            for (j = 0; j <= i; j++)
                g[l] = add(g[l], mult(s->a[l][i][j], add(mult(x[j], y[i]), mult(x[i], y[j]))));
    }
    
    return g;    
}

void MQID_Gen(MQIDScheme s, MQID_key *pk, MQID_key *sk)
{    
    *sk = MQID_unidrndkey(s->n);
    *pk = MQID_F(s, *sk);
}

MQID_key MQID_unidrndkey(int m)
{
    MQID_key key;
    int l;
    
    key = malloc(m * sizeof(Field));
        
    for (l = 0; l < m; l++)
        key[l] = rand() % getOrder();
    
    return key;
}

/* String commitment scheme */
char * MQID_Com(char * comstr, int n, ...)
{
    va_list ap;
    int i, m;
    MQID_key k;
    char * auxstr, * tmpstr, * hex;
    
    auxstr = malloc(sizeof(char));
    auxstr[0] = '\0';
    
    va_start(ap, n);
    for (i = 0; i < n; i++) {
        k = va_arg(ap, MQID_key);
        m = va_arg(ap, int);
        
        hex = tohexstr(k, m);
        tmpstr = malloc((strlen(auxstr) + strlen(hex) + 1) * sizeof(char));
        tmpstr[0] = '\0';
        tmpstr = strcat(tmpstr, auxstr);
        tmpstr = strcat(tmpstr, hex);
        
        free(auxstr);
        auxstr = tmpstr;
    }

    comstr = whirlpoolsum(comstr, tmpstr);
    
    free(tmpstr);
    
    return comstr;
}

void clearMQIDScheme(MQIDScheme s)
{
    free(s->b[0]);
    free(s->b);
    free(s->a[0][0]);
    free(s->a[0]);
    free(s->a);
    free(s);
}
