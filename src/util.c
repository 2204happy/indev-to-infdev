#include <sys/stat.h>
#include <stdio.h>
#include <util.h>

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


int flipIntEndian(int n) {
    return (n&0x000000ff)<<24|
           (n&0x0000ff00)<<8|
           (n&0x00ff0000)>>8|
           (n&0xff000000)>>24;
}

short int flipShortEndian(short int n) {
    return (n&0x00ff)<<8|
           (n&0xff00)>>8;
}

long int flipLongEndian(long int n) {
    return (n&0x00000000000000ff)<<56|
           (n&0x000000000000ff00)<<40|
           (n&0x0000000000ff0000)<<24|
           (n&0x00000000ff000000)<<8|
           (n&0x000000ff00000000)>>8|
           (n&0x0000ff0000000000)>>24|
           (n&0x00ff000000000000)>>40|
           (n&0xff00000000000000)>>56;
}
