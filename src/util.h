#ifndef UTIL_H
#define UTIL_H

#include "lib/map/map.h"

void loadKeys(map_int_t* keys);
char* readLine();
void setSeed(int seed);
double perlin2d(double x, double y, double freq, int depth);

#endif
