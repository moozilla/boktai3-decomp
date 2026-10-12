// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define ResultGame_UpdatePlayerMarker sub_0801C6CC
extern u16 gUnknown_03005260[];extern int gUnk_0824DB38[];extern u8 gUnk_0824DB64[],gUnk_0824DB6F[];void sub_0822B2F8(int);
void ResultGame_UpdatePlayerMarker(u8*p){register int *velocity asm("r4");u32 key;{register u32 input asm("r0")=gUnknown_03005260[1];register u32 mask asm("r2");asm("" : "+r"(input));mask=1;asm("" : "+r"(mask));mask&=input;asm("" : "+r"(mask));key=mask;}
 if(key){velocity=(int*)(p+0x90);*velocity=gUnk_0824DB38[*(u32*)(p+0x50)];sub_0822B2F8(0x3ec);}
 else {int v;register int *initial asm("r0")=(int*)(p+0x90);asm("" : "+r"(initial));v=*initial;velocity=initial;if(v>0){*velocity=key;*((u8*)initial+0x22)=key;}
 else {u32 index;u8 *counter=p+0xb2;register u8 *table asm("r0")=gUnk_0824DB64;asm("" : : "r"(counter),"r"(table));{register u32 n asm("r6");n=*(u32*)(p+0x50);asm("" : "+r"(n));index=n;}if(p[0xb2]<gUnk_0824DB64[index]){*velocity=key;++p[0xb2];}else{*velocity=v-gUnk_0824DB6F[index];if(*velocity< -22)*velocity=-22;}}}
 *(int*)(p+0x8c)+=*velocity;
}
