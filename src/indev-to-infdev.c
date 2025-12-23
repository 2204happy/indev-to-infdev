#include <stdio.h>
#include <zlib.h>
#include <stdlib.h>
#include <string.h>

#include <nbt.h>
#include <chunk.h>
#include <util.h>
#include <level.h>

int main() {
    char* inputBuffer = malloc(8407005);
    gzFile f = gzopen("test.mclevel","rb");
    gzread(f,inputBuffer,8407005);
    gzclose(f);
    char* MinecraftLevel = findNBTEntry(inputBuffer,COMPOUND,"MinecraftLevel");
    char* Map = findNBTEntry(MinecraftLevel,COMPOUND,"Map");
    char* Blocks = findNBTEntry(Map,BYTE_ARRAY,"Blocks");
    char* Data = findNBTEntry(Map,BYTE_ARRAY,"Data");
    
    char worldName[] = "World1";
    
    char* outputBuffer = malloc(1024*1024);
    mkdirIfNotExists(worldName);
    struct IndevWorldSize worldSize;
    worldSize.x = 256;
    worldSize.y = 64;
    worldSize.z = 256;
    struct ChunkPos chunkPos;
    chunkPos.x = 0;
    while(chunkPos.x*16<worldSize.x) {
        chunkPos.z = 0;
        while(chunkPos.z*16<worldSize.z) {
            int size = makeChunk(outputBuffer,Blocks+4,Data+4,chunkPos,worldSize);
            saveChunk(outputBuffer,size,worldName,chunkPos);
            chunkPos.z+=1;
        }
        chunkPos.x+=1;
    }
    
    int size = makeLevelDat(outputBuffer);
    saveLevelDat(outputBuffer,size,worldName);
    
    
    free(inputBuffer);
    free(outputBuffer);
    return 0;
}
