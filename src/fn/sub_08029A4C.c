// MATCHING_CONCESSION: register-pinning
#include "global.h"
void sub_08029A4C(s16*p,int x,int z){if(x!=1||z){p[0]=(x<<8)+0xa80;{register int height asm("r0")=256;asm("":"+r"(height));p[1]=height;}p[2]=(z<<8)+0xa80;}}
