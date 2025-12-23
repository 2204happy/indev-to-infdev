#include <stdio.h>
#include <zlib.h>
#include <stdlib.h>
#include <string.h>

#include <nbt.h>
#include <chunk.h>
#include <util.h>

int main() {
    char* buffer = malloc(8407005);
    gzFile f = gzopen("test.mclevel","rb");
    gzread(f,buffer,8407005);
    gzclose(f);
    char* MinecraftLevel = findNBTEntry(buffer,COMPOUND,"MinecraftLevel");
    char* Map = findNBTEntry(MinecraftLevel,COMPOUND,"Map");
    char* Blocks = findNBTEntry(Map,BYTE_ARRAY,"Blocks");
    char* Data = findNBTEntry(Map,BYTE_ARRAY,"Data");

    
    mkdirIfNotExists("World1");
    struct IndevWorldSize worldSize;
    worldSize.x = 256;
    worldSize.y = 64;
    worldSize.z = 256;
    struct ChunkPos chunkPos;
    chunkPos.x = 0;
    while(chunkPos.x*16<worldSize.x) {
        chunkPos.z = 0;
        while(chunkPos.z*16<worldSize.z) {
            makeChunk("World1",Blocks+4,Data+4,chunkPos,worldSize);
            chunkPos.z+=1;
        }
        chunkPos.x+=1;
    }
    
    free(buffer);
    return 0;
}
