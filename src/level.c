#include <stdbool.h>
#include <zlib.h>
#include <stdio.h>
#include <string.h>

#include <nbt.h>


struct levelData {
    struct coordinates3D dimensions;
    struct coordinates3D spawn;
    long int time;
    char* playerEntry;
    char* blockArray;
    char* dataArray;
    char* tileEntities;
};

char* getPlayerEntry(char* inputPtr) {
    char* playerEntryPointer;
    inputPtr++;
    int numEntities = flipIntEndian(*((int*)inputPtr));
    inputPtr+=4;
    int i = 0;
    bool found = false;
    while (i<numEntities && !found) {
        char* id = passNBTHeader(findNBTEntry(inputPtr,STRING,"id"));
        short int strLen = flipShortEndian(*((short int*)id));
        strLen = strLen>11 ? 11 : strLen;
        id+=2;
        if(memcmp(id,"LocalPlayer",strLen) == 0) {
            playerEntryPointer = inputPtr;
            found = true;
        }
        else {
            inputPtr = getNextNBTEntry(inputPtr,true,COMPOUND);
        }
    }
    return playerEntryPointer;
}

struct levelData getLevelData(char* inputBuffer) {
    struct levelData data;
    char* MinecraftLevel = passNBTHeader(findNBTEntry(inputBuffer,COMPOUND,"MinecraftLevel"));
    char* Environment = passNBTHeader(findNBTEntry(MinecraftLevel,COMPOUND,"Environment"));
    
    short int TimeOfDay = flipShortEndian(*((short int*)passNBTHeader(findNBTEntry(Environment,SHORT,"TimeOfDay"))));
    
    data.time = TimeOfDay;
    
    char* Entities = passNBTHeader(findNBTEntry(MinecraftLevel,LIST,"Entities"));
    
    data.playerEntry = getPlayerEntry(Entities);
    
    char* Map = passNBTHeader(findNBTEntry(MinecraftLevel,COMPOUND,"Map"));
    data.blockArray = passNBTHeader(findNBTEntry(Map,BYTE_ARRAY,"Blocks"))+4;
    data.dataArray = passNBTHeader(findNBTEntry(Map,BYTE_ARRAY,"Data"))+4;
    data.tileEntities = passNBTHeader(findNBTEntry(MinecraftLevel,LIST,"TileEntities"));
    
}

int makeLevelDat(char* buffer,struct levelData data) {
    char* bufferStart = buffer;
    
    buffer = makeNBTCompoundEntry(buffer,"",false);
    buffer = makeNBTCompoundEntry(buffer,"Data",false);
    
    buffer = makeNBTLongEntry(buffer,"LastPlayed", 0, false);
    
    buffer = makeNBTLongEntry(buffer,"SizeOnDisk", 0, false);
    
    buffer = makeNBTLongEntry(buffer,"RandomSeed", 0x123456789abcdef0, false);

    buffer = makeNBTIntEntry(buffer,"SpawnX", data.spawn.x, false);
    
    buffer = makeNBTIntEntry(buffer,"SpawnY", data.spawn.y, false);
    
    buffer = makeNBTIntEntry(buffer,"SpawnZ", data.spawn.z, false);
    
    buffer = makeNBTLongEntry(buffer,"Time", data.time, false);
    
    char* playerEnd = getNextNBTEntry(data.playerEntry,true,COMPOUND);
    char* playerID = findNBTEntry(data.playerEntry,STRING,"id");
    

    buffer = makeNBTCompoundEntry(buffer,"Player",false);
    
    int copyLen = playerID-data.playerEntry;
    memcpy(buffer,data.playerEntry,copyLen);
    buffer+=copyLen;
    char* inPtr = getNextNBTEntry(playerID,false,0);
    
    copyLen = playerEnd-inPtr;
    memcpy(buffer,inPtr,copyLen);
    buffer+=copyLen;
    
    buffer = makeNBTEndEntry(buffer);
    buffer = makeNBTEndEntry(buffer);
    
    return buffer-bufferStart;
}

void saveLevelDat(char* buffer, int size, char* worldName) {
    char filePath[64] = "";
    
    sprintf(filePath,"%s/level.dat",worldName);
    
    gzFile f = gzopen(filePath,"wb");
    gzwrite(f,buffer,size);
    gzclose(f);
}
