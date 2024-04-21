#include "util.h"

#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include <raylib.h>

#define ROTLEFT(a, b) (((a) << (b)) | ((a) >> (32 - (b))))
#define ROTRIGHT(a, b) (((a) >> (b)) | ((a) << (32 - (b))))

#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x, 2) ^ ROTRIGHT(x, 13) ^ ROTRIGHT(x, 22))
#define EP1(x) (ROTRIGHT(x, 6) ^ ROTRIGHT(x, 11) ^ ROTRIGHT(x, 25))
#define SIG0(x) (ROTRIGHT(x, 7) ^ ROTRIGHT(x, 18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x, 17) ^ ROTRIGHT(x, 19) ^ ((x) >> 10))

static int SEED = 2004;

void loadKeys(map_int_t* keys)
{
    map_init(keys);

    map_set(keys, "apostrophe", KEY_APOSTROPHE);
    map_set(keys, "comma", KEY_COMMA);
    map_set(keys, "minus", KEY_MINUS);
    map_set(keys, "period", KEY_PERIOD);
    map_set(keys, "slash", KEY_SLASH);
    map_set(keys, "zero", KEY_ZERO);
    map_set(keys, "one", KEY_ONE);
    map_set(keys, "two", KEY_TWO);
    map_set(keys, "three", KEY_THREE);
    map_set(keys, "four", KEY_FOUR);
    map_set(keys, "five", KEY_FIVE);
    map_set(keys, "six", KEY_SIX);
    map_set(keys, "seven", KEY_SEVEN);
    map_set(keys, "eight", KEY_EIGHT);
    map_set(keys, "nine", KEY_NINE);
    map_set(keys, "semicolon", KEY_SEMICOLON);
    map_set(keys, "equal", KEY_EQUAL);
    map_set(keys, "a", KEY_A);
    map_set(keys, "b", KEY_B);
    map_set(keys, "c", KEY_C);
    map_set(keys, "d", KEY_D);
    map_set(keys, "e", KEY_E);
    map_set(keys, "f", KEY_F);
    map_set(keys, "g", KEY_G);
    map_set(keys, "h", KEY_H);
    map_set(keys, "i", KEY_I);
    map_set(keys, "j", KEY_J);
    map_set(keys, "k", KEY_K);
    map_set(keys, "l", KEY_L);
    map_set(keys, "m", KEY_M);
    map_set(keys, "n", KEY_N);
    map_set(keys, "o", KEY_O);
    map_set(keys, "p", KEY_P);
    map_set(keys, "q", KEY_Q);
    map_set(keys, "r", KEY_R);
    map_set(keys, "s", KEY_S);
    map_set(keys, "t", KEY_T);
    map_set(keys, "u", KEY_U);
    map_set(keys, "v", KEY_V);
    map_set(keys, "w", KEY_W);
    map_set(keys, "x", KEY_X);
    map_set(keys, "y", KEY_Y);
    map_set(keys, "z", KEY_Z);
    map_set(keys, "leftBracket", KEY_LEFT_BRACKET);
    map_set(keys, "backslash", KEY_BACKSLASH);
    map_set(keys, "rightBracket", KEY_RIGHT_BRACKET);
    map_set(keys, "grave", KEY_GRAVE);
    map_set(keys, "space", KEY_SPACE);
    map_set(keys, "escape", KEY_ESCAPE);
    map_set(keys, "enter", KEY_ENTER);
    map_set(keys, "tab", KEY_TAB);
    map_set(keys, "backspace", KEY_BACKSPACE);
    map_set(keys, "insert", KEY_INSERT);
    map_set(keys, "delete", KEY_DELETE);
    map_set(keys, "right", KEY_RIGHT);
    map_set(keys, "left", KEY_LEFT);
    map_set(keys, "down", KEY_DOWN);
    map_set(keys, "up", KEY_UP);
    map_set(keys, "pageUp", KEY_PAGE_UP);
    map_set(keys, "pageDown", KEY_PAGE_DOWN);
    map_set(keys, "home", KEY_HOME);
    map_set(keys, "end", KEY_END);
    map_set(keys, "capsLock", KEY_CAPS_LOCK);
    map_set(keys, "scrollLock", KEY_SCROLL_LOCK);
    map_set(keys, "numLock", KEY_NUM_LOCK);
    map_set(keys, "printScreen", KEY_PRINT_SCREEN);
    map_set(keys, "pause", KEY_PAUSE);
    map_set(keys, "f1", KEY_F1);
    map_set(keys, "f2", KEY_F2);
    map_set(keys, "f3", KEY_F3);
    map_set(keys, "f4", KEY_F4);
    map_set(keys, "f5", KEY_F5);
    map_set(keys, "f6", KEY_F6);
    map_set(keys, "f7", KEY_F7);
    map_set(keys, "f8", KEY_F8);
    map_set(keys, "f9", KEY_F9);
    map_set(keys, "f10", KEY_F10);
    map_set(keys, "f11", KEY_F11);
    map_set(keys, "f12", KEY_F12);
    map_set(keys, "leftShift", KEY_LEFT_SHIFT);
    map_set(keys, "leftControl", KEY_LEFT_CONTROL);
    map_set(keys, "leftAlt", KEY_LEFT_ALT);
    map_set(keys, "leftSuper", KEY_LEFT_SUPER);
    map_set(keys, "rightShift", KEY_RIGHT_SHIFT);
    map_set(keys, "rightControl", KEY_RIGHT_CONTROL);
    map_set(keys, "rightAlt", KEY_RIGHT_ALT);
    map_set(keys, "rightSuper", KEY_RIGHT_SUPER);
    map_set(keys, "kbMenu", KEY_KB_MENU);
    map_set(keys, "kp0", KEY_KP_0);
    map_set(keys, "kp1", KEY_KP_1);
    map_set(keys, "kp2", KEY_KP_2);
    map_set(keys, "kp3", KEY_KP_3);
    map_set(keys, "kp4", KEY_KP_4);
    map_set(keys, "kp5", KEY_KP_5);
    map_set(keys, "kp6", KEY_KP_6);
    map_set(keys, "kp7", KEY_KP_7);
    map_set(keys, "kp8", KEY_KP_8);
    map_set(keys, "kp9", KEY_KP_9);
    map_set(keys, "kpDecimal", KEY_KP_DECIMAL);
    map_set(keys, "kpDivide", KEY_KP_DIVIDE);
    map_set(keys, "kpMultiply", KEY_KP_MULTIPLY);
    map_set(keys, "kpSubtract", KEY_KP_SUBTRACT);
    map_set(keys, "kpAdd", KEY_KP_ADD);
    map_set(keys, "kpEnter", KEY_KP_ENTER);
    map_set(keys, "kpEqual", KEY_KP_EQUAL);
}

char* readLine()
{
    int bufferSize = 10;
    char* buffer = (char*)malloc(bufferSize);
    if (buffer == NULL)
        return NULL;

    int index = 0;
    char c;
    while ((c = getchar()) != '\n' && c != EOF) {
        buffer[index++] = c;

        if (index >= bufferSize) {
            bufferSize *= 2;
            buffer = realloc(buffer, bufferSize);
            if (buffer == NULL)
                return NULL;
        }
    }

    buffer[index] = '\0';

    return buffer;
}

void setSeed(int seed)
{
    SEED = seed;
}

// Perlin noise from: https://gist.github.com/nowl/828013

static const unsigned char HASH[] = {
    208, 34, 231, 213, 32, 248, 233, 56, 161, 78, 24, 140, 71, 48, 140, 254, 245, 255, 247, 247, 40,
    185, 248, 251, 245, 28, 124, 204, 204, 76, 36, 1, 107, 28, 234, 163, 202, 224, 245, 128, 167, 204,
    9, 92, 217, 54, 239, 174, 173, 102, 193, 189, 190, 121, 100, 108, 167, 44, 43, 77, 180, 204, 8, 81,
    70, 223, 11, 38, 24, 254, 210, 210, 177, 32, 81, 195, 243, 125, 8, 169, 112, 32, 97, 53, 195, 13,
    203, 9, 47, 104, 125, 117, 114, 124, 165, 203, 181, 235, 193, 206, 70, 180, 174, 0, 167, 181, 41,
    164, 30, 116, 127, 198, 245, 146, 87, 224, 149, 206, 57, 4, 192, 210, 65, 210, 129, 240, 178, 105,
    228, 108, 245, 148, 140, 40, 35, 195, 38, 58, 65, 207, 215, 253, 65, 85, 208, 76, 62, 3, 237, 55, 89,
    232, 50, 217, 64, 244, 157, 199, 121, 252, 90, 17, 212, 203, 149, 152, 140, 187, 234, 177, 73, 174,
    193, 100, 192, 143, 97, 53, 145, 135, 19, 103, 13, 90, 135, 151, 199, 91, 239, 247, 33, 39, 145,
    101, 120, 99, 3, 186, 86, 99, 41, 237, 203, 111, 79, 220, 135, 158, 42, 30, 154, 120, 67, 87, 167,
    135, 176, 183, 191, 253, 115, 184, 21, 233, 58, 129, 233, 142, 39, 128, 211, 118, 137, 139, 255,
    114, 20, 218, 113, 154, 27, 127, 246, 250, 1, 8, 198, 250, 209, 92, 222, 173, 21, 88, 102, 219
};

static int noise2(int x, int y)
{
    int yindex = (y + SEED) % 256;
    if (yindex < 0)
        yindex += 256;

    int xindex = (HASH[yindex] + x) % 256;
    if (xindex < 0)
        xindex += 256;

    const int result = HASH[xindex];
    return result;
}

static double lin_inter(double x, double y, double s)
{
    return x + s * (y - x);
}

static double smooth_inter(double x, double y, double s)
{
    return lin_inter(x, y, s * s * (3 - 2 * s));
}

static double noise2d(double x, double y)
{
    const int x_int = floor(x);
    const int y_int = floor(y);
    const double x_frac = x - x_int;
    const double y_frac = y - y_int;
    const int s = noise2(x_int, y_int);
    const int t = noise2(x_int + 1, y_int);
    const int u = noise2(x_int, y_int + 1);
    const int v = noise2(x_int + 1, y_int + 1);
    const double low = smooth_inter(s, t, x_frac);
    const double high = smooth_inter(u, v, x_frac);
    const double result = smooth_inter(low, high, y_frac);
    return result;
}

double perlin2d(double x, double y, double freq, int depth)
{
    double xa = x * freq;
    double ya = y * freq;
    double amp = 1.0;
    double fin = 0;
    double div = 0.0;

    for (int i = 0; i < depth; i++) {
        div += 256 * amp;
        fin += noise2d(xa, ya) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }

    return fin / div;
}

// Hex encoding from: https://github.com/love2d/love/blob/main/src/modules/data/DataModule.cpp

static const char hexchars[] = "0123456789abcdef";

char* bytesToHex(const unsigned char* src, size_t srclen, size_t* dstlen)
{
    *dstlen = srclen * 2;
    if (*dstlen == 0)
        return NULL;

    char* dst = NULL;
    dst = (char*)malloc(*dstlen + 1);
    if (dst == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < srclen; i++) {
        unsigned char b = src[i];
        dst[i * 2 + 0] = hexchars[b >> 4];
        dst[i * 2 + 1] = hexchars[b & 0xF];
    }

    dst[*dstlen] = '\0';

    return dst;
}

static unsigned char nibble(char c)
{
    if (c >= '0' && c <= '9')
        return (unsigned char)(c - '0');
    if (c >= 'A' && c <= 'F')
        return (unsigned char)(c - 'A' + 0x0a);
    if (c >= 'a' && c <= 'f')
        return (unsigned char)(c - 'a' + 0x0a);

    return 0;
}

unsigned char* hexToBytes(const char* src, size_t srclen, size_t* dstlen)
{
    if (srclen >= 2 && src[0] == '0' && (src[1] == 'x' || src[1] == 'X')) {
        src += 2;
        srclen -= 2;
    }

    *dstlen = (srclen + 1) / 2;
    if (*dstlen == 0)
        return NULL;

    unsigned char* dst = NULL;
    dst = (unsigned char*)malloc(*dstlen);
    if (dst == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < *dstlen; i++) {
        dst[i] = nibble(src[i * 2]) << 4;

        if (i * 2 + 1 < srclen)
            dst[i] |= nibble(src[i * 2 + 1]);
    }

    return dst;
}

// SHA256 from: https://github.com/B-Con/crypto-algorithms/blob/master/sha256.c

static const WORD k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static void sha256_transform(SHA256_CTX* ctx, const BYTE data[])
{
    WORD a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

    for (i = 0, j = 0; i < 16; ++i, j += 4)
        m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
    for (; i < 64; ++i)
        m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    for (i = 0; i < 64; ++i) {
        t1 = h + EP1(e) + CH(e, f, g) + k[i] + m[i];
        t2 = EP0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

void sha256_init(SHA256_CTX* ctx)
{
    ctx->datalen = 0;
    ctx->bitlen = 0;
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
}

void sha256_update(SHA256_CTX* ctx, const BYTE data[], size_t len)
{
    WORD i;

    for (i = 0; i < len; ++i) {
        ctx->data[ctx->datalen] = data[i];
        ctx->datalen++;
        if (ctx->datalen == 64) {
            sha256_transform(ctx, ctx->data);
            ctx->bitlen += 512;
            ctx->datalen = 0;
        }
    }
}

void sha256_final(SHA256_CTX* ctx, BYTE hash[])
{
    WORD i;

    i = ctx->datalen;

    if (ctx->datalen < 56) {
        ctx->data[i++] = 0x80;
        while (i < 56)
            ctx->data[i++] = 0x00;
    } else {
        ctx->data[i++] = 0x80;
        while (i < 64)
            ctx->data[i++] = 0x00;
        sha256_transform(ctx, ctx->data);
        memset(ctx->data, 0, 56);
    }

    ctx->bitlen += ctx->datalen * 8;
    ctx->data[63] = ctx->bitlen;
    ctx->data[62] = ctx->bitlen >> 8;
    ctx->data[61] = ctx->bitlen >> 16;
    ctx->data[60] = ctx->bitlen >> 24;
    ctx->data[59] = ctx->bitlen >> 32;
    ctx->data[58] = ctx->bitlen >> 40;
    ctx->data[57] = ctx->bitlen >> 48;
    ctx->data[56] = ctx->bitlen >> 56;
    sha256_transform(ctx, ctx->data);

    for (i = 0; i < 4; ++i) {
        hash[i] = (ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 4] = (ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 8] = (ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 20] = (ctx->state[5] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 24] = (ctx->state[6] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 28] = (ctx->state[7] >> (24 - i * 8)) & 0x000000ff;
    }
}
