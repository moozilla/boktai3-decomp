// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneObject_UpdateLowered sub_0801F5B8
void sub_082260A4(int);void sub_0821AD08(int,int);
void SceneObject_UpdateLowered(u8*s){register int timer asm("r6")=*(s16*)(s+0x82);if(timer){sub_082260A4(timer);(*(u16*)(s+0x82))--;}else if(*(int*)(s+0x64)){sub_0821AD08(*(int*)(s+0x64),0);*(int*)(s+0x64)=timer;}}
