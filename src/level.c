#include <stdbool.h>
#include <zlib.h>
#include <stdio.h>

#include <nbt.h>

int makeLevelDat(char* buffer) {
    char* bufferStart = buffer;
    
    buffer = makeNBTCompoundEntry(buffer,"",false);
    buffer = makeNBTCompoundEntry(buffer,"Data",false);
    
    buffer = makeNBTLongEntry(buffer,"LastPlayed", 0, false);
    
    buffer = makeNBTLongEntry(buffer,"SizeOnDisk", 0, false);
    
    buffer = makeNBTLongEntry(buffer,"RandomSeed", 0x123456789abcdef0, false);

    buffer = makeNBTIntEntry(buffer,"SpawnX", 32, false);
    
    buffer = makeNBTIntEntry(buffer,"SpawnY", 32, false);
    
    buffer = makeNBTIntEntry(buffer,"SpawnZ", 32, false);
    
    buffer = makeNBTLongEntry(buffer,"Time", 0, false);

    buffer = makeNBTCompoundEntry(buffer,"Player",false);

    buffer = makeNBTListEntry(buffer,"Pos", 3, DOUBLE, false);
    buffer = makeNBTDoubleEntry(buffer,"",0,true);
    buffer = makeNBTDoubleEntry(buffer,"",64,true);
    buffer = makeNBTDoubleEntry(buffer,"",0,true);
    
    buffer = makeNBTListEntry(buffer,"Rotation", 2, FLOAT, false);
    buffer = makeNBTFloatEntry(buffer,"",0,true);
    buffer = makeNBTFloatEntry(buffer,"",0,true);
    
    buffer = makeNBTListEntry(buffer,"Motion", 3, DOUBLE, false);
    buffer = makeNBTDoubleEntry(buffer,"",0,true);
    buffer = makeNBTDoubleEntry(buffer,"",64,true);
    buffer = makeNBTDoubleEntry(buffer,"",0,true);
    
    buffer = makeNBTByteEntry(buffer,"OnGround", 0, false);

    buffer = makeNBTFloatEntry(buffer,"FallDistance",0,false);
    
    buffer = makeNBTShortEntry(buffer,"Health", 20, false);
  
    buffer = makeNBTShortEntry(buffer,"AttackTime", 0, false);
    buffer = makeNBTShortEntry(buffer,"HurtTime", 0, false);
    buffer = makeNBTShortEntry(buffer,"DeathTime", 0, false);
    buffer = makeNBTShortEntry(buffer,"Air", 300, false);
    buffer = makeNBTShortEntry(buffer,"Fire", -20, false);

    buffer = makeNBTListEntry(buffer,"Inventory", 0, COMPOUND, false);

    buffer = makeNBTEndEntry(buffer);
    
    buffer = makeNBTEndEntry(buffer);
    buffer = makeNBTEndEntry(buffer);
    
    return buffer-bufferStart;
}

void saveLevelDat(char* buffer, int size, char* worldName) {
    char filePath[64] = "";
    
    sprintf(filePath,"%s/level.dat",worldName);
    
    gzFile f = gzopen(filePath,"wb");
    gzwrite(f,buffer,size);
    gzclose(f);
}
