#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>

#include <nbt.h>
#include <util.h>
#include <chunk.h>
#include <tile-entities.h>

struct coordinates3D getCoordsInChunk(struct coordinates3D coords,struct coordinates2D chunkPos) {
    coords.x-=chunkPos.x*16;
    coords.z-=chunkPos.z*16;
    return coords;
}

bool coordsInChunk(struct coordinates3D globalCoords,struct coordinates2D chunkPos) {
    struct coordinates3D coords = getCoordsInChunk(globalCoords,chunkPos);
    return coords.x>=0 && coords.x<16 && coords.z>=0 && coords.z<16;
}

void getChunkFilePath(struct coordinates2D chunkPos, char* worldName,char* filePathBuffer) {
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

void buildChunkArrays(char* inputBlockArray,char* inputDataArray, struct coordinates2D chunkPos, struct coordinates3D worldSize,char* blockArray,char* dataArray,char* skyLightArray, char* heightMap) {
    int x = 0;
    while (x<16) {
        int z = 0;
        while(z<16) {
            int y = 127;
            int heightMapIndex = z+x*16;
            *(heightMap+heightMapIndex)=-1;
            while(y>=0) {
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
                if(block!=0 && *(heightMap+heightMapIndex)==-1) {
                    *(heightMap+heightMapIndex) = y;
                }
                int infdevIndex = y+(z*128+(x*128*16));
                *(blockArray+infdevIndex) = block;
                char dataValue = (inputDataValue&0xf0)>>4;
                char skyLightValue;
                if(*(heightMap+heightMapIndex)==-1 || *(heightMap+heightMapIndex)==y) {
                    skyLightValue = 0x0f;
                }
                else {
                     skyLightValue = inputDataValue&0x0f;
                }
                int dataLightArrayIndex = infdevIndex>>1;
                if(infdevIndex%2==0) {
                    *(dataArray+dataLightArrayIndex) += dataValue<<4;
                    *(skyLightArray+dataLightArrayIndex) += skyLightValue<<4;
                }
                else {
                    *(dataArray+dataLightArrayIndex) = dataValue;
                    *(skyLightArray+dataLightArrayIndex) = skyLightValue;                    
                }
                y-=1;
            }
            z+=1;
        }
        x+=1;
    }
}

int makeChunk(char* buffer,char* inputBlockArray,char* inputDataArray, struct coordinates2D chunkPos, struct coordinates3D worldSize, char* tileEntities) {
    const int chunkBlockCount = 16*16*128;
    
    char* bufferStart = buffer;
    
    buffer=makeNBTCompoundEntry(buffer,"",false);
    buffer=makeNBTCompoundEntry(buffer,"Level",false);
    buffer=makeNBTIntEntry(buffer,"xPos",chunkPos.x,false);
    buffer=makeNBTIntEntry(buffer,"zPos",chunkPos.z,false);
    //chunkSize+=makeNBTLongEntry(buffer,"LastUpdate",0,false);
    
    
    char blockArray[chunkBlockCount];
    char dataArray[chunkBlockCount/2];
    char skyLightArray[chunkBlockCount/2];
    char heightMap[256];
    
    buildChunkArrays(inputBlockArray,inputDataArray,chunkPos,worldSize,blockArray,dataArray,skyLightArray,heightMap);
    
    buffer=makeNBTByteArrayEntry(buffer,"Blocks",blockArray,chunkBlockCount,false);

    
    buffer=makeNBTByteArrayEntry(buffer,"Data",dataArray,chunkBlockCount/2,false);
    buffer=makeNBTByteArrayEntry(buffer,"SkyLight",skyLightArray,chunkBlockCount/2,false);
    
    memset(skyLightArray,0,chunkBlockCount/2);
    buffer=makeNBTByteArrayEntry(buffer,"BlockLight",skyLightArray,chunkBlockCount/2,false);
    
    buffer=makeNBTByteArrayEntry(buffer,"HeightMap",heightMap,256,false);
    
    
    buffer=makeNBTListEntry(buffer,"Entities",0,COMPOUND,false);
    buffer = copyTileEntities(tileEntities, buffer, chunkPos);
    
    buffer=makeNBTEndEntry(buffer);
    buffer=makeNBTEndEntry(buffer);
    
    return buffer-bufferStart;
} 

void saveChunk(char* buffer, int size, char* worldName, struct coordinates2D chunkPos) {
    char filePath[64] = "";
    getChunkFilePath(chunkPos,worldName,filePath);
    
    gzFile f = gzopen(filePath,"wb");
    gzwrite(f,buffer,size);
    gzclose(f);
}
