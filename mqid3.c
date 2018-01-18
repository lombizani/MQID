/* sample naive implementation of Sakumoto, Shirai and Hiwatari's
 * 3-pass public-key id. scheme based on the MQ problem
 * by: Carlos De Bernardis
 */

#include <stdlib.h>
#include <string.h>
#include "mqid3.h"

struct mqid3_prover {
    MQID_key r0, t0, e0, r1, t1, e1;
};

struct mqid3_verifier {
    char ch;
    char * com1;
    char * com2;
    MQIDScheme s;
    MQID_key pk;
};

MQID3_prover prover1stStep(MQIDScheme s, MQID_key sk, char * c0, char * c1, char * c2)
{    
    MQID3_prover p;
    size_t unknowns;
    MQID_key fr0, gt0r1_e0;
    
    p = malloc(sizeof(struct mqid3_prover));
    
    p->r0 = MQID_unidrndkey(MQID_getn(s));
    p->t0 = MQID_unidrndkey(MQID_getn(s));
    p->e0 = MQID_unidrndkey(MQID_getm(s));
    
    unknowns = MQID_getn(s) * sizeof(Field);
    fr0 = MQID_F(s, p->r0);
    
    p->r1 = malloc(MQID_getn(s) * sizeof(Field));
    p->t1 = malloc(MQID_getn(s) * sizeof(Field));
    p->e1 = malloc(MQID_getm(s) * sizeof(Field));
    
    p->r1 = memcpy(p->r1, sk, unknowns);
    p->t1 = memcpy(p->t1, p->r0, unknowns);
    p->e1 = memcpy(p->e1, fr0, MQID_getm(s) * sizeof(Field));
    
    p->r1 = vsub(p->r1, p->r0, MQID_getn(s));
    p->t1 = vsub(p->t1, p->t0, MQID_getn(s));
    p->e1 = vsub(p->e1, p->e0, MQID_getm(s));
    
    gt0r1_e0 = MQID_G(s, p->t0, p->r1);
    gt0r1_e0 = vadd(gt0r1_e0, p->e0, MQID_getm(s));
    
    c0 = MQID_Com(c0, 2, p->r1, MQID_getn(s), gt0r1_e0, MQID_getm(s));
    c1 = MQID_Com(c1, 2, p->t0, MQID_getn(s), p->e0, MQID_getm(s));
    c2 = MQID_Com(c2, 2, p->t1, MQID_getn(s), p->e1, MQID_getm(s));
    
    free(fr0);
    free(gt0r1_e0);
    
    return p;
}

MQID_key * prover2ndStep(MQID3_prover p, char ch)
{
    MQID_key * rsp;
    
    rsp = malloc(3 * sizeof(MQID_key));
    
    switch (ch) {
        case 0:
            rsp[0] = p->r0;
            rsp[1] = p->t1;
            rsp[2] = p->e1;
            free(p->t0);
            p->t0 = NULL;
            free(p->e0);
            p->e0 = NULL;
            free(p->r1);
            p->r1 = NULL;
            break;
        case 1:
            rsp[0] = p->r1;
            rsp[1] = p->t1;
            rsp[2] = p->e1;
            free(p->r0);
            p->r0 = NULL;
            free(p->t0);
            p->t0 = NULL;
            free(p->e0);
            p->e0 = NULL;
            break;
        case 2:
            rsp[0] = p->r1;
            rsp[1] = p->t0;
            rsp[2] = p->e0;
            free(p->r0);
            p->r0 = NULL;
            free(p->t1);
            p->t1 = NULL;
            free(p->e1);
            p->e1 = NULL;
            break;
        default:
            free(rsp);
            rsp = NULL;
    }
    
    return rsp;
}

MQID3_verifier verifier1stStep(MQIDScheme s, MQID_key pk, char * c0, char * c1, char * c2, char * ch)
{
    MQID3_verifier v;
    
    v = malloc(sizeof(struct mqid3_verifier));
    
    v->ch = (*ch) = (char)(rand()%3);
    
    v->s = s;
    v->pk = pk;
    
    switch(v->ch) {
        case 0:
            v->com1 = c1;
            v->com2 = c2;
            break;
        case 1:
            v->com1 = c0;
            v->com2 = c2;
            break;
        case 2:
            v->com1 = c0;
            v->com2 = c1;
            break;
    }
    
    return v;
}

int verifier2ndStep(MQID3_verifier v, MQID_key * rsp)
{
    int verified;
    char * c1;
    char * c2;
    MQID_key f, g, second;
    
    f = NULL;
    g = NULL;
    
    switch(v->ch) {
        case 0:
            f = MQID_F(v->s, rsp[0]);
            vsub(f, rsp[2], MQID_getm(v->s));
            vsub(rsp[0], rsp[1], MQID_getn(v->s));
            second = f;
            break;
        case 1:
            f = MQID_F(v->s, rsp[0]);
            g = MQID_G(v->s, rsp[1], rsp[0]);
            vadd(g, rsp[2], MQID_getm(v->s));
            vadd(f, g, MQID_getm(v->s));
            vsub(v->pk, f, MQID_getm(v->s));
            second = v->pk;
            break;
        case 2:
            g = MQID_G(v->s, rsp[1], rsp[0]);
            vadd(g, rsp[2], MQID_getm(v->s));
            second = g;
            break;
    }
    
    c1 = malloc(sizeof(unsigned char) * (getComStrSize() + 1));
    c2 = malloc(sizeof(unsigned char) * (getComStrSize() + 1));
    c1 = MQID_Com(c1, 2, rsp[0], MQID_getn(v->s), second, MQID_getm(v->s));
    c2 = MQID_Com(c2, 2, rsp[1], MQID_getn(v->s), rsp[2], MQID_getm(v->s));
    
    verified = (strcmp(c1, v->com1) == 0) && (strcmp(c2, v->com2) == 0);
    
    free(c1);
    free(c2);
    
    free(f);
    free(g);
    
    return verified;
}

void clearMQID3(MQID3_prover p, MQID3_verifier v)
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
