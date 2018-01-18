#define HEXHASHSIZE 128

/* computes the hex Whirlpool hash checksum of str and copies it to *sum, returns sum */
char * whirlpoolsum(char * sum, char * const str);