#include "global.h"
extern u32 gUnk_030053F4,gUnk_030053F0;
void sub_08116C4C(u32,s32 *);
static inline s32 absolute(s32 x) { if(x<0)x=-x;return x; }
u32 sub_081155FC(u8 *s)
{
    s32 v[4];
    s32 i;
    u32 mask=0x800;
    if ((gUnk_030053F4|gUnk_030053F0)&mask) {
        i=0;
        s+=0xDC;
        do {
            sub_08116C4C(i,v);
            if (absolute((v[0]>>12)-*(s16 *)s)<=160) return 1;
            ++i;
        } while (i<=1);
fail:
        return 0;
    } else {
        sub_08116C4C(0,v);
        if(absolute((v[0]>>12)-*(s16 *)(s+0xDC))>160) goto fail;
    }
    return 1;
}
