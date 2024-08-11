#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable: 4996)

#define HASH_TABLE_SIZE 127

typedef struct Parcel {
    char* destination;
    int weight;
    float value;
} Parcel;