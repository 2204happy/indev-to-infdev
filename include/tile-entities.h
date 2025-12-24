struct coordinates3D getTileEntityCoords(int pos);

char* addTileEntityToChunk(char* outputBuffer,char* nbtData,struct coordinates2D chunkPos);

char* copyTileEntities(char* inputTileEntities, char* outputBuffer, struct coordinates2D chunkPos); 
