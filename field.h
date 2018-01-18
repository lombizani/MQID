/* generic interface for finite-field operations */

typedef int Field;

/* returns the order in use */
int getOrder();

/* adds two field elements (modulo order addition) */
Field add(Field a, Field b);

/* multiplies two field elements (modulo order multiplication) */
Field mult(Field a, Field b);

/* returns the zero element of the field (s.t. x+0 = x, x*0 = 0) */
Field zero();

/* returns -a (s.t. a + (-a) = 0) */
Field neg(Field a);

/* whether a is zero or not */
int isZero(Field a);

/* adds two n-dimentional vectors on the field */
Field * vadd(Field *a, Field *b, int n);

/* subtracts two n-dimentional vectors on the field */
Field * vsub(Field *a, Field *b, int n);

/* performs scalar multiplication on a vector */
void vmult(Field *a, Field x, int n);

/* returns a string on the heap containing a hex representation of a vector */
char * tohexstr(Field *a, int n);
