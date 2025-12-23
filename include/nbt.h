#include <stdbool.h>
#include <util.h>

char* makeNBTEndEntry(char* buffer);

char* makeNBTByteEntry(char* buffer,char* name, char payload, bool inList);

char* makeNBTShortEntry(char* buffer,char* name, short int payload, bool inList);

char* makeNBTIntEntry(char* buffer,char* name, int payload, bool inList);

char* makeNBTLongEntry(char* buffer,char* name, long int payload, bool inList);

char* makeNBTFloatEntry(char* buffer,char* name, float payload, bool inList);

char* makeNBTDoubleEntry(char* buffer,char* name, double payload, bool inList);

char* makeNBTByteArrayEntry(char* buffer,char* name,char* payload,int plSize, bool inList);

char* makeNBTStringEntry(char* buffer,char* name,char* payload, bool inList);

char* makeNBTListEntry(char* buffer,char* name, int plSize, enum nbtType listType, bool inList);

char* makeNBTCompoundEntry(char* buffer,char* name,bool inList);

char* makeNBTIntArrayEntry(char* buffer,char* name,int* payload,int plSize, bool inList);

char* makeNBTLongArrayEntry(char* buffer,char* name,long int* payload,int plSize, bool inList);

char* skipNBTListEntry(char* nbtData);

char* skipNBTCompoundEntry(char* nbtData);

char* getNextNBTEntry(char* nbtData,bool inList,enum nbtType listType);

char* findNBTEntry(char* nbtData,enum nbtType type,char* name);
