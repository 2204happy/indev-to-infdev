#include <nbt.h>
#include <endian.h>
#include <string.h>

int makeNBTHeader(char* buffer, enum nbtType type,char* name, int size, enum nbtType listType, bool inList) {
    int headerLen = 0;
    if(type == END) {
        *buffer = 0;
        headerLen++;
        return headerLen;
    }
    
    if(!inList) {
        *buffer = (char)type;
        headerLen++;
        int nameLen = strlen(name);
        short int nameLenBE = flipShortEndian((short int) nameLen);
        *((short int*)(buffer+headerLen)) = nameLenBE;
        headerLen+=2;
        memcpy(buffer+headerLen,name,nameLen);
        headerLen+=nameLen;
    }
    if(type==LIST) {
        *(buffer+headerLen) = (char)listType;
        headerLen+=1;
    }
    if(type>=7 && type!=STRING && type!=COMPOUND) {
        *((int*)(buffer+headerLen)) = flipIntEndian(size);
        headerLen+=4;
    }
    else if(type==STRING) {
        *((short int*)(buffer+headerLen)) = flipShortEndian((short int)size);
        headerLen+=2;        
    }
    return headerLen;
    
}

int makeNBTEndEntry(char* buffer) {
    return makeNBTHeader(buffer, END,"", 0, 0, false);
}

int makeNBTByteEntry(char* buffer,char* name, char payload, bool inList) {
    int size = makeNBTHeader(buffer,BYTE,name,0,0,inList);
    buffer+=size;
    *(buffer) = payload;
    size++;
    return size;
}

int makeNBTShortEntry(char* buffer,char* name, short int payload, bool inList) {
    int size = makeNBTHeader(buffer,SHORT,name,0,0,inList);
    buffer+=size;
    *((short int*)buffer) = flipShortEndian(payload);
    size+=2;
    return size;
}

int makeNBTIntEntry(char* buffer,char* name, int payload, bool inList) {
    int size = makeNBTHeader(buffer,INT,name,0,0,inList);
    buffer+=size;
    *((int*)buffer) = flipIntEndian(payload);
    size+=4;
    return size;
}

int makeNBTLongEntry(char* buffer,char* name, long int payload, bool inList) {
    int size = makeNBTHeader(buffer,LONG,name,0,0,inList);
    buffer+=size;
    *((long int*)buffer) = flipLongEndian(payload);
    size+=8;
    return size;
}


int makeNBTFloatEntry(char* buffer,char* name, float payload, bool inList) {
    int size = makeNBTHeader(buffer,FLOAT,name,0,0,inList);
    buffer+=size;
    *((int*)buffer) = flipIntEndian(*((int*)(&payload)));
    size+=4;
    return size;
}

int makeNBTDoubleEntry(char* buffer,char* name, double payload, bool inList) {
    int size = makeNBTHeader(buffer,DOUBLE,name,0,0,inList);
    buffer+=size;
    *((long int*)buffer) = flipLongEndian(*((long int*)(&payload)));
    size+=8;
    return size;
}

int makeNBTByteArrayEntry(char* buffer,char* name,char* payload,int plSize, bool inList) {
    int size = makeNBTHeader(buffer,BYTE_ARRAY,name,plSize,0,inList);
    buffer+=size;
    memcpy(buffer,payload,plSize);
    size+=plSize;
    return size;
}

int makeNBTStringEntry(char* buffer,char* name,char* payload, bool inList) {
    int plSize = strlen(payload);
    int size = makeNBTHeader(buffer,STRING,name,plSize,0,inList);
    buffer+=size;
    memcpy(buffer,payload,plSize);
    size+=plSize;
    return size;
}

int makeNBTListEntry(char* buffer,char* name, int plSize, enum nbtType listType, bool inList) {
    return makeNBTHeader(buffer,LIST,name,plSize,listType,inList);
}

int makeNBTCompoundEntry(char* buffer,char* name,bool inList) {
    return makeNBTHeader(buffer,COMPOUND,name,0,0,inList);
}

int makeNBTIntArrayEntry(char* buffer,char* name,int* payload,int plSize, bool inList) {
    int size = makeNBTHeader(buffer,INT_ARRAY,name,plSize,0,inList);
    buffer+=size;
    int i = 0;
    while(i<plSize) {
        *((int*)buffer) = flipIntEndian(*payload);
        buffer+=4;
    }
    size+=plSize*4;
    return size;
}

int makeNBTLongArrayEntry(char* buffer,char* name,long int* payload,int plSize, bool inList) {
    int size = makeNBTHeader(buffer,LONG_ARRAY,name,plSize,0,inList);
    buffer+=size;
    int i = 0;
    while(i<plSize) {
        *((long int*)buffer) = flipIntEndian(*payload);
        buffer+=8;
    }
    size+=plSize*8;
    return size;
}
