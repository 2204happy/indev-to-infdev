#include <stdio.h>
#include <zlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

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

int flipIntEndian(int n) {
    return (n&0x000000ff)<<24|(n&0x0000ff00)<<8|(n&0x00ff0000)>>8|(n&0xff000000)>>24;
}

void makeChunk(char* worldName, char* blocks, struct ChunkPos chunkPos, struct IndevWorldSize worldSize) {
    char filePath[64] = "";
    getChunkFilePath(chunkPos,worldName,filePath);
    
    
    char header[] = "\x0a\x00\x00\x0a\x00\x05Level\x03\x00\x04xPos";
    char middle5[] = "\x03\x00\x04zPos";
    char middle6[] = "\x07\x00\x06""Blocks\x00\x00\x80\x00";
    char middle1[] = "\x04\x00\x0aLastUpdate\x00\x00\x00\x00\x00\x00\x00\x00\x07\x00\x04""Data\x00\x00\x40\x00";
    char middle2[] = "\x07\x00\x0a""BlockLight\x00\x00\x40\x00";
    char middle3[] = "\x07\x00\x08SkyLight\x00\x00\x40\x00";
    char middle4[] = "\x07\x00\x09HeightMap\x00\x00\x01\x00";
    char footer[] = "\x00\x00";
    int bufferSize = 32768+3*16384+256+2*4+sizeof(header)+sizeof(middle1)+sizeof(middle2)+sizeof(middle3)+sizeof(middle4)+sizeof(middle5)+sizeof(middle6)+sizeof(footer)-8;
    char* outBuffer = malloc(bufferSize);
    char* bufferPtr = outBuffer;
    
    memcpy(bufferPtr,header,sizeof(header)-1);
    bufferPtr+=sizeof(header)-1;
    
    int xPosBE = flipIntEndian(chunkPos.x);
    memcpy(bufferPtr,&xPosBE,4);
    bufferPtr+=4;
    
    memcpy(bufferPtr,middle5,sizeof(middle5)-1);
    bufferPtr+=sizeof(middle5)-1;
    
    int zPosBE = flipIntEndian(chunkPos.z);
    memcpy(bufferPtr,&zPosBE,4);
    bufferPtr+=4;
    
    memcpy(bufferPtr,middle6,sizeof(middle6)-1);
    bufferPtr+=sizeof(middle6)-1;
    
    buildChunkBlockArray(bufferPtr,blocks,chunkPos,worldSize);
    bufferPtr+=32768;
    
    memcpy(bufferPtr,middle1,sizeof(middle1)-1);
    bufferPtr+=sizeof(middle1)-1;
    memset(bufferPtr,0,16384);
    bufferPtr+=16384;
    
    memcpy(bufferPtr,middle2,sizeof(middle2)-1);
    bufferPtr+=sizeof(middle2)-1;
    memset(bufferPtr,0,16384);
    bufferPtr+=16384;
    
    memcpy(bufferPtr,middle3,sizeof(middle3)-1);
    bufferPtr+=sizeof(middle3)-1;
    memset(bufferPtr,0,16384);
    bufferPtr+=16384;
    
    memcpy(bufferPtr,middle4,sizeof(middle4)-1);
    bufferPtr+=sizeof(middle4)-1;
    memset(bufferPtr,32,256);
    bufferPtr+=256;
    
    
    memcpy(bufferPtr,footer,sizeof(footer)-1);
    
    
    gzFile f = gzopen(filePath,"wb");
    gzwrite(f,outBuffer,bufferSize);
    gzclose(f);
    free(outBuffer);
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
