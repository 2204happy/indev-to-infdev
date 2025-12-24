#include <util.h>
#include <stdbool.h>

struct coordinates3D getCoordsInChunk(struct coordinates3D globalCoords,struct coordinates2D chunkPos);

bool coordsInChunk(struct coordinates3D coords,struct coordinates2D chunkPos);

void getChunkFilePath(struct coordinates2D chunkPos, char* worldName,char* filePathBuffer);

void buildChunkBlockArray(char* inputBlockArray,char* inputDataArray, struct coordinates2D chunkPos, struct coordinates3D worldSize,char* blockArray,char* dataArray,char* skyLightArray, char* heightMap);

int makeChunk(char* buffer,char* inputBlockArray,char* inputDataArray, struct coordinates2D chunkPos, struct coordinates3D worldSize, char* tileEntities);

void saveChunk(char* buffer, int size, char* worldName, struct coordinates2D chunkPos);
