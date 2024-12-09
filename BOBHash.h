#ifndef _BOBHASH_H
#define _BOBHASH_H
#include <stdio.h>
using namespace std;

typedef unsigned int uint;

#define MAX_PRIME 1229
#define MAX_BIG_PRIME 50

class BOBHash
{
public:
	BOBHash();
	~BOBHash();
	BOBHash(uint primeNum);
	void initialize(uint primeNum);
	uint run(const char * str, uint len);
private:
	uint primeNum;
};

extern uint big_prime[MAX_BIG_PRIME];
extern uint prime[MAX_PRIME];

#define mix(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}


#endif //_BOBHASH_H
