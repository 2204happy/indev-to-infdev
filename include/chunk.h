#include <util.h>

void getChunkFilePath(struct ChunkPos chunkPos, char* worldName,char* filePathBuffer);

void buildChunkBlockArray(char* inputBlockArray,char* inputDataArray, struct ChunkPos chunkPos, struct IndevWorldSize worldSize,char* blockArray,char* dataArray,char* skyLightArray, char* heightMap);

void makeChunk(char* worldName, char* inputBlockArray,char* inputDataArray, struct ChunkPos chunkPos, struct IndevWorldSize worldSize);
