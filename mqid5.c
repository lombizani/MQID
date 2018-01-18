/* sample naive implementation of Sakumoto, Shirai and Hiwatari's
 * 5-pass public-key id. scheme based on the MQ problem
 * by: Carlos De Bernardis
 */

#include <stdlib.h>
#include <string.h>
#include "mqid5.h"

struct mqid5_prover {
    MQIDScheme s;
    MQID_key r0, t0, e0, r1, t1, e1;
};

struct mqid5_verifier {
    Field alpha;
    char ch;
    char * com0;
    char * com1;
    MQIDScheme s;
    MQID_key pk, t1, e1;
};

MQID5_prover prover1stStep(MQIDScheme s, MQID_key sk, char * c0, char * c1)
{    
    MQID5_prover p;
    MQID_key gt0r1_e0;
    
    p = malloc(sizeof(struct mqid5_prover));
    
    p->s = s;
    
    p->r0 = MQID_unidrndkey(MQID_getn(s));
    p->t0 = MQID_unidrndkey(MQID_getn(s));
    p->e0 = MQID_unidrndkey(MQID_getm(s));
    
    p->r1 = malloc(MQID_getn(s) * sizeof(Field));    
    p->r1 = memcpy(p->r1, sk, MQID_getn(s) * sizeof(Field));    
    p->r1 = vsub(p->r1, p->r0, MQID_getn(s));
    
    gt0r1_e0 = MQID_G(s, p->t0, p->r1);
    gt0r1_e0 = vadd(gt0r1_e0, p->e0, MQID_getm(s));
    
    c0 = MQID_Com(c0, 3, p->r0, MQID_getn(s), p->t0, MQID_getn(s), p->e0, MQID_getm(s));
    c1 = MQID_Com(c1, 2, p->r1, MQID_getn(s), gt0r1_e0, MQID_getm(s));
    
    return p;
}

void prover2ndStep(MQID5_prover p, Field alpha, MQID_key * t1, MQID_key * e1)
{
    MQID_key f;
    
    p->t1 = malloc(MQID_getn(p->s) * sizeof(Field));
    p->t1 = memcpy(p->t1, p->r0, MQID_getn(p->s) * sizeof(Field));
    vmult(p->t1, alpha, MQID_getn(p->s));
    p->t1 = vsub(p->t1, p->t0, MQID_getn(p->s));
    
    f = MQID_F(p->s, p->r0);
    p->e1 = malloc(MQID_getm(p->s) * sizeof(Field));
    p->e1 = memcpy(p->e1, f, MQID_getm(p->s) * sizeof(Field));
    vmult(p->e1, alpha, MQID_getm(p->s));
    p->e1 = vsub(p->e1, p->e0, MQID_getm(p->s));
    
    *t1 = p->t1;
    *e1 = p->e1;
    
    free(f);
}

MQID_key prover3rdStep(MQID5_prover p, char ch)
{
    MQID_key rsp;
    
    switch (ch) {
        case 0:
            rsp = p->r0;
            p->r0 = NULL;
            break;
        case 1:
            rsp = p->r1;
            p->r1 = NULL;
            break;
        default:
            rsp = NULL;
    }
    
    return rsp;
}

MQID5_verifier verifier1stStep(MQIDScheme s, MQID_key pk, char * c0, char * c1, Field * alpha)
{
    MQID5_verifier v;
    
    v = malloc(sizeof(struct mqid5_verifier));
    
    *alpha = v->alpha = rand() % getOrder();
    
    v->s = s;
    v->pk = pk;
    
    v->com0 = c0;
    v->com1 = c1;
            
    return v;
}

char verifier2ndStep(MQID5_verifier v, MQID_key t1, MQID_key e1)
{
    v->t1 = t1;
    v->e1 = e1;
    v->ch = rand() % 2;
    return v->ch;
}

int verifier3rdStep(MQID5_verifier v, MQID_key rsp)
{
    int verified;
    char * c0;
    MQID_key f, g, second;
    
    f = NULL;
    g = NULL;
    second = NULL;
        
    switch(v->ch) {
        case 0:
            second = malloc(MQID_getn(v->s) * sizeof(Field));
            second = memcpy(second, rsp, MQID_getn(v->s) * sizeof(Field));
            vmult(second, v->alpha, MQID_getn(v->s));
            second = vsub(second, v->t1, MQID_getn(v->s));
            
            f = MQID_F(v->s, rsp);
            vmult(f, v->alpha, MQID_getm(v->s));
            f = vsub(f, v->e1, MQID_getm(v->s));
            
            c0 = malloc(sizeof(char) * (getComStrSize() + 1)); 
            c0 = MQID_Com(c0, 3, rsp, MQID_getn(v->s), second, MQID_getn(v->s), f, MQID_getm(v->s));
            
            verified = (strcmp(c0, v->com0) == 0);
            break;
        case 1:
            f = MQID_F(v->s, rsp);
            g = MQID_G(v->s, v->t1, rsp);
            vadd(g, v->e1, MQID_getm(v->s));
            vsub(v->pk, f, MQID_getm(v->s));
            vmult(v->pk, v->alpha, MQID_getm(v->s));
            vsub(v->pk, g, MQID_getm(v->s));
            
            c0 = malloc(sizeof(char) * (getComStrSize() + 1)); 
            c0 = MQID_Com(c0, 2, rsp, MQID_getn(v->s), v->pk, MQID_getm(v->s));
            
            verified = (strcmp(c0, v->com1) == 0);
            break;
    }
    
    free(f);
    free(g);
    free(second);
    free(c0);
    
    return verified;
}

void clearMQID5(MQID5_prover p, MQID5_verifier v)
{
    free(v);
    free(p->r0);
    free(p->t0);
    free(p->e0);
    free(p->r1);
    free(p->t1);
    free(p->e1);
    free(p);
}
