/* MQID-3 and MQID-5 generic interfaces */

#include "field.h"
#include "Whirlpool/Whirlpool.h"

typedef struct mqid *MQIDScheme; /* stores a reference for the scheme and its data */

typedef Field *MQID_key; /* a key is a vector of field elements */


/* returns the size of all commitment strings */
int getComStrSize();

/* initializes a mxn sized scheme with random parameters */
MQIDScheme setupMQIDScheme (int m, int n);

/* returns the number of equations of a scheme */
int MQID_getm(MQIDScheme s);

/* returns the number of unknowns of a scheme */
int MQID_getn(MQIDScheme s);

/* returns the quadratic coefficients */
Field MQID_geta(MQIDScheme s, int k, int i, int j);

/* returns the linear coefficients */
Field MQID_getb(MQIDScheme s, int k, int i);

/* F(x) */
MQID_key MQID_F(MQIDScheme s, MQID_key x);

/* G(x,y) */
MQID_key MQID_G(MQIDScheme s, MQID_key x, MQID_key y);

/* key-generation algorithm */
void MQID_Gen(MQIDScheme s, MQID_key *pk, MQID_key *sk);

/* sorts a uniformly distributed random m-dimentional Field array */
MQID_key MQID_unidrndkey(int m);

// takes as parameters n keys and its respective sizes, computes the commitment scheme function on them
// puts it on comstr and returns it
// sample usage: comstr = MQID_com(comstr, 3, key1, n1, key2, n2, key3, n3)
char * MQID_Com(char * comstr, int n, ...);

// clears memory
void clearMQIDScheme(MQIDScheme s);
