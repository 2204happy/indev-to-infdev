#include <stdbool.h>

#ifndef ITOI_TYPES
#define ITOI_TYPES

enum nbtType {
    END = 0,
    BYTE = 1,
    SHORT = 2,
    INT = 3,
    LONG = 4,
    FLOAT = 5,
    DOUBLE = 6,
    BYTE_ARRAY = 7,
    STRING = 8,
    LIST = 9,
    COMPOUND = 10,
    INT_ARRAY = 11,
    LONG_ARRAY = 12
};

struct coordinates3D {
    int x;
    int y;
    int z;
};

struct coordinates2D {
    int x;
    int z;
};

#endif


char* getb36(char* buffer, int n);

bool dirExists(char* dir);

void mkdirIfNotExists(char* dir);

int flipIntEndian(int n);

short int flipShortEndian(short int n);

long int flipLongEndian(long int n);
