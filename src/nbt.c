#include <string.h>
#include <stdlib.h>

#include <util.h>
#include <nbt.h>

int makeNBTHeader(char* buffer, enum nbtType type,char* name, int size, enum nbtType listType, bool inList, bool nameOnly) {
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
    if(!nameOnly) {
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
    }
    return headerLen;
    
}

int makeNBTEndEntry(char* buffer) {
    return makeNBTHeader(buffer, END,"", 0, 0, false,false);
}

int makeNBTByteEntry(char* buffer,char* name, char payload, bool inList) {
    int size = makeNBTHeader(buffer,BYTE,name,0,0,inList,false);
    buffer+=size;
    *(buffer) = payload;
    size++;
    return size;
}

int makeNBTShortEntry(char* buffer,char* name, short int payload, bool inList) {
    int size = makeNBTHeader(buffer,SHORT,name,0,0,inList,false);
    buffer+=size;
    *((short int*)buffer) = flipShortEndian(payload);
    size+=2;
    return size;
}

int makeNBTIntEntry(char* buffer,char* name, int payload, bool inList) {
    int size = makeNBTHeader(buffer,INT,name,0,0,inList,false);
    buffer+=size;
    *((int*)buffer) = flipIntEndian(payload);
    size+=4;
    return size;
}

int makeNBTLongEntry(char* buffer,char* name, long int payload, bool inList) {
    int size = makeNBTHeader(buffer,LONG,name,0,0,inList,false);
    buffer+=size;
    *((long int*)buffer) = flipLongEndian(payload);
    size+=8;
    return size;
}


int makeNBTFloatEntry(char* buffer,char* name, float payload, bool inList) {
    int size = makeNBTHeader(buffer,FLOAT,name,0,0,inList,false);
    buffer+=size;
    *((int*)buffer) = flipIntEndian(*((int*)(&payload)));
    size+=4;
    return size;
}

int makeNBTDoubleEntry(char* buffer,char* name, double payload, bool inList) {
    int size = makeNBTHeader(buffer,DOUBLE,name,0,0,inList,false);
    buffer+=size;
    *((long int*)buffer) = flipLongEndian(*((long int*)(&payload)));
    size+=8;
    return size;
}

int makeNBTByteArrayEntry(char* buffer,char* name,char* payload,int plSize, bool inList) {
    int size = makeNBTHeader(buffer,BYTE_ARRAY,name,plSize,0,inList,false);
    buffer+=size;
    memcpy(buffer,payload,plSize);
    size+=plSize;
    return size;
}

int makeNBTStringEntry(char* buffer,char* name,char* payload, bool inList) {
    int plSize = strlen(payload);
    int size = makeNBTHeader(buffer,STRING,name,plSize,0,inList,false);
    buffer+=size;
    memcpy(buffer,payload,plSize);
    size+=plSize;
    return size;
}

int makeNBTListEntry(char* buffer,char* name, int plSize, enum nbtType listType, bool inList) {
    return makeNBTHeader(buffer,LIST,name,plSize,listType,inList,false);
}

int makeNBTCompoundEntry(char* buffer,char* name,bool inList) {
    return makeNBTHeader(buffer,COMPOUND,name,0,0,inList,false);
}

int makeNBTIntArrayEntry(char* buffer,char* name,int* payload,int plSize, bool inList) {
    int size = makeNBTHeader(buffer,INT_ARRAY,name,plSize,0,inList,false);
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
    int size = makeNBTHeader(buffer,LONG_ARRAY,name,plSize,0,inList,false);
    buffer+=size;
    int i = 0;
    while(i<plSize) {
        *((long int*)buffer) = flipIntEndian(*payload);
        buffer+=8;
    }
    size+=plSize*8;
    return size;
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

char* getNextNBTEntry(char* nbtData,bool inList,enum nbtType listType) {
    enum nbtType type;
    if(inList) {
        type = listType;
    }
    else {
        type = (enum nbtType)(*nbtData);
        nbtData++;
        if(type!=END) {
            int nameLen = flipShortEndian(*((short int*)nbtData));
            nbtData+=2+nameLen;
        }
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
    char* buffer = malloc(1024);
    int headerLen = makeNBTHeader(buffer,type,name,0,0,false,true);
    bool done = false;
    int treeLevel = 0;
    while(!done) {
        if(memcmp(buffer,nbtData,headerLen)==0) {
            free(buffer);
            return nbtData+headerLen;
        }
        else {
            nbtData = getNextNBTEntry(nbtData,false,0);
        }
    }
    free(buffer);
}
