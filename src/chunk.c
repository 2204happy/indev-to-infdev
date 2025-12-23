#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>

#include <nbt.h>
#include <util.h>
#include <chunk.h>

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

void buildChunkBlockArray(char* inputBlockArray,char* inputDataArray, struct ChunkPos chunkPos, struct IndevWorldSize worldSize,char* blockArray,char* dataArray,char* skyLightArray, char* heightMap) {
    int x = 0;
    while (x<16) {
        int z = 0;
        while(z<16) {
            int y = 0;
            while(y<128) {
                char block;
                char inputDataValue;
                if (y<worldSize.y) {
                    int indevIndex = (y*worldSize.z+(z+chunkPos.z*16))*worldSize.x+(x+chunkPos.x*16);
                    block = *(inputBlockArray+indevIndex);
                    inputDataValue = *(inputDataArray+indevIndex);
                }
                else {
                    block = 0;
                    inputDataValue = 0x0f;
                }
                if(block!=0) {
                    int heightMapIndex = z+x*16;
                    *(heightMap+heightMapIndex) = y;
                }
                int infdevIndex = y+(z*128+(x*128*16));
                *(blockArray+infdevIndex) = block;
                char dataValue = (inputDataValue&0xf0)>>4;
                char skyLightValue = inputDataValue&0x0f;
                int dataLightArrayIndex = infdevIndex>>1;
                if(infdevIndex%2==0) {
                    *(dataArray+dataLightArrayIndex) = dataValue<<4;
                    *(skyLightArray+dataLightArrayIndex) = skyLightValue<<4;
                }
                else {
                    *(dataArray+dataLightArrayIndex) += dataValue;
                    *(skyLightArray+dataLightArrayIndex) += skyLightValue;                    
                }
                y+=1;
            }
            z+=1;
        }
        x+=1;
    }
}

void makeChunk(char* worldName, char* inputBlockArray,char* inputDataArray, struct ChunkPos chunkPos, struct IndevWorldSize worldSize) {
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
    //chunkSize+=makeNBTLongEntry(buffer+chunkSize,"LastUpdate",0,false);
    
    
    char blockArray[chunkBlockCount];
    char dataArray[chunkBlockCount/2];
    char skyLightArray[chunkBlockCount/2];
    char heightMap[256];
    
    buildChunkBlockArray(inputBlockArray,inputDataArray,chunkPos,worldSize,blockArray,dataArray,skyLightArray,heightMap);
    
    chunkSize+=makeNBTByteArrayEntry(buffer+chunkSize,"Blocks",blockArray,chunkBlockCount,false);

    
    chunkSize+=makeNBTByteArrayEntry(buffer+chunkSize,"Data",dataArray,chunkBlockCount/2,false);
    chunkSize+=makeNBTByteArrayEntry(buffer+chunkSize,"SkyLight",skyLightArray,chunkBlockCount/2,false);
    
    memset(skyLightArray,0,chunkBlockCount/2);
    chunkSize+=makeNBTByteArrayEntry(buffer+chunkSize,"BlockLight",skyLightArray,chunkBlockCount/2,false);
    
    chunkSize+=makeNBTByteArrayEntry(buffer+chunkSize,"HeightMap",heightMap,256,false);
    
    
    chunkSize+=makeNBTListEntry(buffer+chunkSize,"Entities",0,COMPOUND,false);
    chunkSize+=makeNBTListEntry(buffer+chunkSize,"TileEntities",0,COMPOUND,false);
    
    chunkSize+=makeNBTEndEntry(buffer+chunkSize);
    chunkSize+=makeNBTEndEntry(buffer+chunkSize);
    
    
    gzFile f = gzopen(filePath,"wb");
    gzwrite(f,buffer,chunkSize);
    gzclose(f);
    free(buffer);
} 
