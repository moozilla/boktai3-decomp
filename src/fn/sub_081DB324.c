#include "global.h"
u8 *sub_08219FBC(u32,u32);
void sub_0821A04C(u8 *,void (*)(u8 *),void (*)(u8 *));
void sub_081DAF50(u8 *);
void sub_081DAFCC(u8 *);
void sub_0821A0C0(u8 *);
s32 sub_081DB168(u8 *,u8 *,u32,s32,s32,s32,s32,s32,u32);
u8 *sub_081DB324(u8 *pos,u32 flag,s32 duration,s32 arg3,s32 arg4,s32 arg5,s32 arg6,u32 arg7)
{
    u8 *obj=sub_08219FBC(10,188);
    if(obj) {
        sub_0821A04C(obj,sub_081DAF50,sub_081DAFCC);
        if(sub_081DB168(obj,pos,flag,duration,arg3,arg4,arg5,arg6,arg7)<0) { sub_0821A0C0(obj); return 0; }
    }
    return obj;
}
