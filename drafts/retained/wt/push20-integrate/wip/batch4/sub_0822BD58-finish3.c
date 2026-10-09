#include "global.h"
void *sub_08219C40(s32);
void sub_08219D38(void *);
s32 sub_0822F0A4(u32,void *,u32);
u32 sub_0822BD58(u16 index,s32 size)
{
    u16 *p;
    s32 n;
    if(size>0x1ff8) size=0x1ff8;
    size &= ~3;
    p=sub_08219C40(size);
    if(!p) return 0;
    n=size>>1;
    if(n>0) {
        u16 fill = 0xabcd;
        u16 *q=p;
        do { *q++=fill; } while(--n);
    }
    sub_0822F0A4(index,p,size);
    sub_08219D38(p);
    return 1;
}
