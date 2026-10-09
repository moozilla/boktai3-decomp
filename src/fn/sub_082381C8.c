#include "global.h"
u8 *sub_08219FBC(u32,u32);
void sub_0821A04C(u8 *,void (*)(u8 *),void (*)(u8 *));
void sub_08237ED8(u8 *);
void sub_08237FFC(u8 *);
void sub_0821A0C0(u8 *);
s32 sub_08238110(u8 *,u8 *,u32,s32,s32,s32,s32,s32,s32,u32);
u8 *sub_082381C8(u8 *pos,u32 flag,s32 duration,s32 arg3,s32 arg4,s32 arg5,s32 arg6,s32 arg7,u32 arg8)
{
    u8 *obj=sub_08219FBC(8,196);
    if(obj) {
        sub_0821A04C(obj,sub_08237ED8,sub_08237FFC);
        if(sub_08238110(obj,pos,flag,duration,arg3,arg4,arg5,arg6,arg7,arg8)<0) { sub_0821A0C0(obj); return 0; }
    }
    return obj;
}
