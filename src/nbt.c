#include <string.h>
#include <stdlib.h>

#include <util.h>
#include <nbt.h>

const int typeLength[] = {
    0,//end
    1,//byte
    2,//short
    4,//int
    8,//long
    4,//float
    8,//double
    1,//byte_array
    1,//string
    0,//list
    0,//compound
    4,//int_array
    8,//long_array
};

char* makeNBTHeader(char* buffer, enum nbtType type,char* name, int size, enum nbtType listType, bool inList, bool nameOnly) {
    if(type == END) {
        *buffer = 0;
        buffer++;
        return buffer;
    }
    
    if(!inList) {
        *buffer = (char)type;
        buffer++;
        int nameLen = strlen(name);
        short int nameLenBE = flipShortEndian((short int) nameLen);
        *((short int*)buffer) = nameLenBE;
        buffer+=2;
        memcpy(buffer,name,nameLen);
        buffer+=nameLen;
    }
    if(!nameOnly) {
        if(type==LIST) {
            *(buffer) = (char)listType;
            buffer+=1;
        }
        if(type>=7 && type!=STRING && type!=COMPOUND) {
            *((int*)(buffer)) = flipIntEndian(size);
            buffer+=4;
        }
        else if(type==STRING) {
            *((short int*)(buffer)) = flipShortEndian((short int)size);
            buffer+=2;        
        }
    }
    return buffer;
    
}

char* makeNBTEndEntry(char* buffer) {
    return makeNBTHeader(buffer, END,"", 0, 0, false,false);
}

char* makeNBTNumEntry(char* buffer,char* name, void* payload, bool inList,enum nbtType type) {
    buffer = makeNBTHeader(buffer,type,name,0,0,inList,false);
    switch (type) {
        case BYTE:
            *buffer = *((char*)payload);
            buffer++;
            break;
        case SHORT:
            *((short int*)buffer) = flipShortEndian(*((short int*)payload));
            buffer+=2;
            break;
        case INT:
        case FLOAT:
            *((int*)buffer) = flipIntEndian(*((int*)payload));
            buffer+=4;
            break;
        case LONG:
        case DOUBLE:
            *((long int*)buffer) = flipLongEndian(*((long int*)payload));
            buffer+=4;
            break;
    }
}

char* makeNBTByteEntry(char* buffer,char* name, char payload, bool inList) {
    buffer = makeNBTNumEntry(buffer,name,(void*)(&payload),inList,BYTE);
    return buffer;
}

char* makeNBTShortEntry(char* buffer,char* name, short int payload, bool inList) {
    buffer = makeNBTNumEntry(buffer,name,(void*)(&payload),inList,SHORT);
    return buffer;
}

char* makeNBTIntEntry(char* buffer,char* name, int payload, bool inList) {
    buffer = makeNBTNumEntry(buffer,name,(void*)(&payload),inList,INT);
    return buffer;
}

char* makeNBTLongEntry(char* buffer,char* name, long int payload, bool inList) {
    buffer = makeNBTNumEntry(buffer,name,(void*)(&payload),inList,LONG);
    return buffer;
}


char* makeNBTFloatEntry(char* buffer,char* name, float payload, bool inList) {
    buffer = makeNBTNumEntry(buffer,name,(void*)(&payload),inList,FLOAT);
    return buffer;
}

char* makeNBTDoubleEntry(char* buffer,char* name, double payload, bool inList) {
    buffer = makeNBTNumEntry(buffer,name,(void*)(&payload),inList,DOUBLE);
    return buffer;
}

char* makeNBTByteArrayEntry(char* buffer,char* name,char* payload,int plSize, bool inList) {
    buffer = makeNBTHeader(buffer,BYTE_ARRAY,name,plSize,0,inList,false);
    memcpy(buffer,payload,plSize);
    buffer+=plSize;
    return buffer;
}

char* makeNBTStringEntry(char* buffer,char* name,char* payload, bool inList) {
    int plSize = strlen(payload);
    buffer = makeNBTHeader(buffer,STRING,name,plSize,0,inList,false);
    memcpy(buffer,payload,plSize);
    buffer+=plSize;
    return buffer;
}

char* makeNBTListEntry(char* buffer,char* name, int plSize, enum nbtType listType, bool inList) {
    return makeNBTHeader(buffer,LIST,name,plSize,listType,inList,false);
}

char* makeNBTCompoundEntry(char* buffer,char* name,bool inList) {
    return makeNBTHeader(buffer,COMPOUND,name,0,0,inList,false);
}

char* makeNBTIntArrayEntry(char* buffer,char* name,int* payload,int plSize, bool inList) {
    buffer = makeNBTHeader(buffer,INT_ARRAY,name,plSize,0,inList,false);
    int i = 0;
    while(i<plSize) {
        *((int*)buffer) = flipIntEndian(*payload);
        buffer+=4;
    }
    return buffer;
}

char* makeNBTLongArrayEntry(char* buffer,char* name,long int* payload,int plSize, bool inList) {
    buffer = makeNBTHeader(buffer,LONG_ARRAY,name,plSize,0,inList,false);
    int i = 0;
    while(i<plSize) {
        *((long int*)buffer) = flipIntEndian(*payload);
        buffer+=8;
    }
    return buffer;
}

void readNBTNumEntry(char* buffer,bool noHeader,void* outBuffer,enum nbtType type) {
    if(!noHeader) {
        buffer = passNBTHeader(buffer);
    }
    flipEndian(buffer,outBuffer,type);
    outBuffer+=typeLength[type];
}

char readNBTByteEntry(char* buffer,bool noHeader) {
    char payload;
    readNBTNumEntry(buffer,noHeader,payload,CHAR);
    return payload;
}


short int readNBTShortEntry(char* buffer,bool noHeader) {
    short int payload;
    readNBTNumEntry(buffer,noHeader,payload,SHORT);
    return payload;
}

int readNBTIntEntry(char* buffer,bool noHeader) {
    int payload;
    readNBTNumEntry(buffer,noHeader,payload,INT);
    return payload;
}

long int readNBTLongEntry(char* buffer,bool noHeader) {
    long int payload;
    readNBTNumEntry(buffer,noHeader,payload,LONG);
    return payload;
}

float readNBTFloatEntry(char* buffer,bool noHeader) {
    float payload;
    readNBTNumEntry(buffer,noHeader,payload,FLOAT);
    return payload;
}

double readNBTDoubleEntry(char* buffer,bool noHeader) {
    double payload;
    readNBTNumEntry(buffer,noHeader,payload,DOUBLE);
    return payload;
}


struct charArray readNBTArrayEntry(char* buffer,bool noHeader,bool stringEntry) {
    if(!noHeader) {
        buffer = passNBTHeader(buffer);
    }
    struct charArray payload;
    if(!stringEntry) {
        payload.size = flipIntEndian(*((int*)buffer));
        buffer+=4;
    }
    else {
        payload.size = flipShortEndian(*((short int*)buffer));
        buffer+=2;
    }
    payload.array = buffer;
    return payload;
}

struct charArray readNBTByteArrayEntry(char* buffer,bool noHeader) {
    return readNBTArrayEntry(buffer,noHeader,false);
}

struct charArray readNBTStringEntry(char* buffer,bool noHeader) {
    return readNBTArrayEntry(buffer,noHeader,true);
}

struct charArray readNBTIntArrayEntry(char* buffer,bool noHeader) {
    struct charArray ca = readNBTArrayEntry(buffer,noHeader,false);
    struct intArray payload;
    payload.array = (int*)ca.array;
    payload.size = ca.size;
}

struct charArray readNBTLongArrayEntry(char* buffer,bool noHeader) {
    struct charArray ca = readNBTArrayEntry(buffer,noHeader,false);
    struct longArray payload;
    payload.array = (long int*)ca.array;
    payload.size = ca.size;
}


char* skipNBTListEntry(char* nbtData) {
    enum nbtType listType = (enum nbtType)(*nbtData);
    nbtData++;
    int size = flipIntEndian(*((int*)nbtData));
    nbtData+=4;
    while(size>0) {
        nbtData=getNextNBTEntry(nbtData,true,listType);
        size-=1;
    }
    return nbtData;
}

char* skipNBTCompoundEntry(char* nbtData) {
    while((enum nbtType)(*nbtData)!=END) {
        nbtData=getNextNBTEntry(nbtData,false,0);
    }
    nbtData++;
    return nbtData;
}

char* passNBTHeader(char* nbtData) {
    enum nbtType type = (enum nbtType)(*nbtData);
    nbtData++;
    if(type!=END) {
        int nameLen = flipShortEndian(*((short int*)nbtData));
        nbtData+=2+nameLen;
    }
    return nbtData;
}

char* getNextNBTEntry(char* nbtData,bool inList,enum nbtType listType) {
    enum nbtType type;
    if(inList) {
        type = listType;
    }
    else {
        type = (enum nbtType)(*nbtData);
        nbtData = passNBTHeader(nbtData);
    }
    int arrayLen;
    switch (type) {
        case END:
            break;
        case BYTE:
            nbtData++;
            break;
        case SHORT:
            nbtData+=2;
            break;
        case INT:
            nbtData+=4;
            break;
        case LONG:
            nbtData+=8;
            break;
        case FLOAT:
            nbtData+=4;
            break;
        case DOUBLE:
            nbtData+=8;
            break;
        case BYTE_ARRAY:
            arrayLen = flipIntEndian(*((int*)nbtData));
            nbtData+=4+arrayLen;
            break;
        case STRING:
            arrayLen = flipShortEndian(*((short int*)nbtData));
            nbtData+=2+arrayLen;
            break;
        case LIST:
            nbtData=skipNBTListEntry(nbtData);
            break;
        case COMPOUND:
            nbtData=skipNBTCompoundEntry(nbtData);
            break;
        case INT_ARRAY:
            arrayLen = flipIntEndian(*((int*)nbtData));
            nbtData+=4+arrayLen*4;
            break;
        case LONG_ARRAY:
            arrayLen = flipIntEndian(*((int*)nbtData));
            nbtData+=4+arrayLen*8;
            break;
    }
    return nbtData;
}

char* findNBTEntry(char* nbtData,enum nbtType type,char* name) {
    char buffer[1024];
    int headerLen = makeNBTHeader(buffer,type,name,0,0,false,true)-buffer;
    bool done = false;
    while(!done) {
        if(memcmp(buffer,nbtData,headerLen)==0) {
            return nbtData;
        }
        else {
            nbtData = getNextNBTEntry(nbtData,false,0);
        }
    }
}
