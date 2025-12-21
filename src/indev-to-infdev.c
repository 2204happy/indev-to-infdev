#include <stdio.h>
#include <zlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <nbt.h>

struct IndevWorldSize {
    int x;
    int y;
    int z;
};

struct ChunkPos {
    int x;
    int z;
};

char* getb36(char* buffer, int n) {
    char* working = buffer;
    char digits[36] = "0123456789abcdefghijklmnopqrstuvwxyz";
    if(n>=36) {
        *working = digits[n/36];
        working++;
    }
    *working = digits[n%36];
    return buffer;
    
}

bool dirExists(char* dir) {
    struct stat s;
    return stat(dir,&s) == 0;
}

void mkdirIfNotExists(char* dir) {
    if (!dirExists(dir)) {
        mkdir(dir,0755);
    }
}

void getChunkFilePath(struct ChunkPos chunkPos, char* worldName,char* filePathBuffer) {
    char dirName1[3] = "";
    char dirName2[3] = "";
    char dirPath1[64] = "";
    char dirPath2[64] = "";
    char xpos[3] = "";
    char zpos[3] = "";
    getb36(dirName1,chunkPos.x%64);
    getb36(dirName2,chunkPos.z%64);
    sprintf(dirPath1,"%s/%s",worldName,dirName1);
    sprintf(dirPath2,"%s/%s/%s",worldName,dirName1,dirName2);
    mkdirIfNotExists(dirPath1);
    mkdirIfNotExists(dirPath2);
    sprintf(filePathBuffer,"%s/%s/%s/c.%s.%s.dat",worldName,dirName1,dirName2,getb36(xpos,chunkPos.x),getb36(zpos,chunkPos.z));
}

void buildChunkBlockArray(char* out, char* blocks, struct ChunkPos chunkPos, struct IndevWorldSize worldSize) {
    int y = 0;
    while (y<128) {
        int z = 0;
        while(z<16) {
            int x = 0;
            while(x<16) {
                char block;
                if (y<worldSize.y) {
                    int indevIndex = (y*worldSize.z+(z+chunkPos.z*16))*worldSize.x+(x+chunkPos.x*16);
                    block = *(blocks+indevIndex);
                }
                else {
                    block = 0;
                }
                int infdevIndex = y+(z*128+(x*128*16));
                *(out+infdevIndex) = block;
                x+=1;
            }
            z+=1;
        }
        y+=1;
    }
}

void makeChunk(char* worldName, char* blocks, struct ChunkPos chunkPos, struct IndevWorldSize worldSize) {
    const int chunkBlockCount = 16*16*128;
    
    char filePath[64] = "";
    getChunkFilePath(chunkPos,worldName,filePath);
    
    int bufferSize = 1024*1024;
    char* buffer = malloc(bufferSize);
    
    int chunkSize = 0;
    
    chunkSize+=makeNBTCompoundEntry(buffer,"",false);
    chunkSize+=makeNBTCompoundEntry(buffer+chunkSize,"Level",false);
    chunkSize+=makeNBTIntEntry(buffer+chunkSize,"xPos",chunkPos.x,false);
    chunkSize+=makeNBTIntEntry(buffer+chunkSize,"zPos",chunkPos.z,false);
    chunkSize+=makeNBTLongEntry(buffer+chunkSize,"LastUpdate",0,false);
    
    
    char* workingArray = malloc(chunkBlockCount);
    buildChunkBlockArray(workingArray,blocks,chunkPos,worldSize);
    
    chunkSize+=makeNBTByteArrayEntry(buffer+chunkSize,"Blocks",workingArray,chunkBlockCount,false);
    
    memset(workingArray,0,chunkBlockCount);
    
    chunkSize+=makeNBTByteArrayEntry(buffer+chunkSize,"BlockLight",workingArray,chunkBlockCount/2,false);
    chunkSize+=makeNBTByteArrayEntry(buffer+chunkSize,"SkyLight",workingArray,chunkBlockCount/2,false);
    chunkSize+=makeNBTByteArrayEntry(buffer+chunkSize,"HeightMap",workingArray,256,false);
        
    
    free(workingArray);
    
    chunkSize+=makeNBTListEntry(buffer+chunkSize,"Entities",0,COMPOUND,false);
    chunkSize+=makeNBTListEntry(buffer+chunkSize,"TileEntities",0,COMPOUND,false);
    
    chunkSize+=makeNBTEndEntry(buffer+chunkSize);
    chunkSize+=makeNBTEndEntry(buffer+chunkSize);
    
    
    gzFile f = gzopen(filePath,"wb");
    gzwrite(f,buffer,chunkSize);
    gzclose(f);
    free(buffer);
}

int main() {
    char* buffer = malloc(8407005);
    gzFile f = gzopen("test.mclevel","rb");
    gzread(f,buffer,8407005);
    gzclose(f);
    
    char* blocks = buffer+0xf9;
    
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
            makeChunk("World1",blocks,chunkPos,worldSize);
            chunkPos.z+=1;
        }
        chunkPos.x+=1;
    }
    
    free(buffer);
    return 0;
}
