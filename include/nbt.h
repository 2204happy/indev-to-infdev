#include <stdbool.h>

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

int makeNBTHeader(char* buffer, enum nbtType type,char* name, int size, enum nbtType listType, bool inList,bool nameOnly);

int makeNBTEndEntry(char* buffer);

int makeNBTByteEntry(char* buffer,char* name, char payload, bool inList);

int makeNBTShortEntry(char* buffer,char* name, short int payload, bool inList);

int makeNBTIntEntry(char* buffer,char* name, int payload, bool inList);

int makeNBTLongEntry(char* buffer,char* name, long int payload, bool inList);

int makeNBTFloatEntry(char* buffer,char* name, float payload, bool inList);

int makeNBTDoubleEntry(char* buffer,char* name, double payload, bool inList);

int makeNBTByteArrayEntry(char* buffer,char* name,char* payload,int plSize, bool inList);

int makeNBTStringEntry(char* buffer,char* name,char* payload, bool inList);

int makeNBTListEntry(char* buffer,char* name, int plSize, enum nbtType listType, bool inList);

int makeNBTCompoundEntry(char* buffer,char* name,bool inList);

int makeNBTIntArrayEntry(char* buffer,char* name,int* payload,int plSize, bool inList);

int makeNBTLongArrayEntry(char* buffer,char* name,long int* payload,int plSize, bool inList);

char* skipNBTCompoundEntry(char* nbtData);

char* getNextNBTEntry(char* nbtData,bool inList,enum nbtType listType);

char* findNBTEntry(char* nbtData,enum nbtType type,char* name);
