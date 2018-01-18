/* MQID-3 specific interface */

#include "mqid.h"

/* store prover and verifier data */
typedef struct mqid3_prover *MQID3_prover;
typedef struct mqid3_verifier *MQID3_verifier;

/* step-by-step protocol algorithm implementation */

MQID3_prover prover1stStep(MQIDScheme s, MQID_key sk, char * c0, char * c1, char * c2);

MQID_key * prover2ndStep(MQID3_prover p, char ch);

MQID3_verifier verifier1stStep(MQIDScheme s, MQID_key pk, char * c0, char * c1, char * c2, char * ch);

int verifier2ndStep(MQID3_verifier v, MQID_key * rsp);

/* clears everything up */
void clearMQID3(MQID3_prover p, MQID3_verifier v);