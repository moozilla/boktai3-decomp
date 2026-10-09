#include "global.h"
void sub_081DAFDC(u8 *,u8 *,u8 *,s32);
void sub_081DB0C4(u8 *,u8 *,u32,s32,s32,s32,s32,s32);
void sub_0821FF24(u8 *,u8 *,u32);
static inline void init(u16 *dest,u32 zero,u32 value) { *dest++=value; *dest=zero; }
s32 sub_081DB1D8(u8 *p,u8 *pos,u32 flag,s32 duration,s32 arg4,s32 arg5,s32 arg6,s32 arg7,s32 frame,u32 arg9)
{
    u8 *emitter;
    sub_081DAFDC(p+0x18,p+0x44,pos,frame);
    emitter=p+0x60;
    sub_081DB0C4(p,emitter,flag,duration,arg4,arg5,arg6|0x40000,arg7);
    sub_0821FF24(emitter,pos,0);
    init((u16 *)(p+0xb4),0,arg9);
    return 0;
}
