#include "global.h"
#define SceneCommands_BlendPalette sub_0802113C
extern int gUnk_030051C8;void sub_08003AA8(int,int,void*);
void SceneCommands_BlendPalette(void*p,void*unused,u8*message){s16*args=(s16*)(message+8);int out[7],mode;out[0]=2;out[1]=args[1];out[2]=args[2];out[3]=args[3];out[4]=args[4];out[5]=0;out[6]=*(s16*)(message+0x12);if(*(s16*)(message+0x14)==1)gUnk_030051C8=1;if(!out[2]&&!out[3]&&!out[4]){out[2]=4;out[3]=4;out[4]=4;}{int enabled=args[0];mode=2;if(!enabled)mode=1;}sub_08003AA8(mode,7,out);}
