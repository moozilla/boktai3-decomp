#include "global.h"
u32 sub_08113398(s16 *,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32);
u32 sub_081134B0(u8 *s,s16 *off,u32 a,u32 b,u32 c,u32 d,u32 e,u32 f,u32 g,u32 h,u32 i,u32 j,u32 k,u32 l,u32 m)
{
    s16 v[3];
    s32 y,z;
    v[0]=*(s32 *)(s+0x10)>>12;
    y=*(s32 *)(s+0x14)+*(s32 *)(s+0x38);
    z=*(s32 *)(s+0x18);
    v[1]=((z+y)>>12)+0x60;
    v[2]=z>>12;
    if (off) {
        v[0]=off[0]+v[0];
        v[1]=v[1]+off[1];
        v[2]=off[2]+v[2];
    }
    return sub_08113398(v,a,b,c,d,e,f,g,h,i,j,k,l,m);
}
