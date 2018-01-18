/* MQID-3 specific interface */

#include "mqid.h"

/* store prover and verifier data */
typedef struct mqid5_prover *MQID5_prover;
typedef struct mqid5_verifier *MQID5_verifier;

/* step-by-step protocol algorithm implementation */

MQID5_prover prover1stStep(MQIDScheme s, MQID_key sk, char * c0, char * c1);

void prover2ndStep(MQID5_prover p, Field alpha, MQID_key * t1, MQID_key * e1);

MQID_key prover3rdStep(MQID5_prover p, char ch);

MQID5_verifier verifier1stStep(MQIDScheme s, MQID_key pk, char * c0, char * c1, Field * alpha);

char verifier2ndStep(MQID5_verifier v, MQID_key t1, MQID_key e1);

int verifier3rdStep(MQID5_verifier v, MQID_key rsp);

/* clears everything up */
void clearMQID5(MQID5_prover p, MQID5_verifier v);