#include <string.h>

#include <util.h>
#include <nbt.h>
#include <chunk.h>

struct coordinates3D getTileEntityCoords(int pos) {
    struct coordinates3D coords;
    coords.x = pos%1024;
    coords.y = (pos>>10)%1024;
    coords.z = (pos>>20)%1024;
    return coords;
}

char* addTileEntityToChunk(char* outputBuffer,char* nbtData,struct coordinates2D chunkPos) {
    char* pos = findNBTEntry(nbtData,INT,"Pos");
    int posValue = flipIntEndian(*((int *)passNBTHeader(pos)));
    struct coordinates3D coords = getTileEntityCoords(posValue);    
    if (coordsInChunk(coords,chunkPos)) {
        char* end = findNBTEntry(nbtData,END,"");
        memcpy(outputBuffer,nbtData,pos-nbtData);
        outputBuffer+=pos-nbtData;
        
        outputBuffer = makeNBTIntEntry(outputBuffer,"x", coords.x, false);
        outputBuffer = makeNBTIntEntry(outputBuffer,"y", coords.y, false);
        outputBuffer = makeNBTIntEntry(outputBuffer,"z", coords.z, false);
 
        nbtData = getNextNBTEntry(pos,false,0);
        memcpy(outputBuffer,nbtData,end-nbtData);
        outputBuffer+=end-nbtData;
        outputBuffer = makeNBTEndEntry(outputBuffer);
    }
    return outputBuffer;
}

char* copyTileEntities(char* inputTileEntities, char* outputBuffer, struct coordinates2D chunkPos) {
    outputBuffer=makeNBTListEntry(outputBuffer,"TileEntities",0,COMPOUND,false);
    int* listLenPtr = (int*)(outputBuffer-4);
    inputTileEntities++;
    int numTileEntities = flipIntEndian(*((int*)inputTileEntities));
    inputTileEntities+=4;
    int i = 0;
    int numTileEntitiesInChunk = 0;
    while(i<numTileEntities) {
        char* newBufferPoint = addTileEntityToChunk(outputBuffer,inputTileEntities,chunkPos);
        if(newBufferPoint!=outputBuffer) {
            outputBuffer = newBufferPoint;
            numTileEntitiesInChunk++;
        }
        inputTileEntities = getNextNBTEntry(inputTileEntities,true,COMPOUND);
        i++;
    }
    *listLenPtr = flipIntEndian(numTileEntitiesInChunk);
    return outputBuffer;
}
