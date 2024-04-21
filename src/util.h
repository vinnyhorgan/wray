#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

#include "lib/map/map.h"

#define SHA256_BLOCK_SIZE 32

typedef unsigned char BYTE;
typedef unsigned int WORD;

typedef struct {
    BYTE data[64];
    WORD datalen;
    unsigned long long bitlen;
    WORD state[8];
} SHA256_CTX;

void loadKeys(map_int_t* keys);
char* readLine();
void setSeed(int seed);
double perlin2d(double x, double y, double freq, int depth);
char* bytesToHex(const unsigned char* src, size_t srclen, size_t* dstlen);
unsigned char* hexToBytes(const char* src, size_t srclen, size_t* dstlen);
void sha256_init(SHA256_CTX* ctx);
void sha256_update(SHA256_CTX* ctx, const BYTE data[], size_t len);
void sha256_final(SHA256_CTX* ctx, BYTE hash[]);

#endif
