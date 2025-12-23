#include <util.h>

void getChunkFilePath(struct ChunkPos chunkPos, char* worldName,char* filePathBuffer);

void buildChunkBlockArray(char* inputBlockArray,char* inputDataArray, struct ChunkPos chunkPos, struct IndevWorldSize worldSize,char* blockArray,char* dataArray,char* skyLightArray, char* heightMap);

int makeChunk(char* buffer,char* inputBlockArray,char* inputDataArray, struct ChunkPos chunkPos, struct IndevWorldSize worldSize);

void saveChunk(char* buffer, int size, char* worldName, struct ChunkPos chunkPos);
