#include "global.h"
static inline u8 test(volatile u32 *p,s32 i){s32 mask=1;mask<<=i;if(*p&mask)return 1;return 0;}
u8 *sub_08125B68(u8 *s)
{
    s32 i=0;
    volatile u32 *flags=(volatile u32 *)(s+0x69C);
    s32 one=1;
    u8 *kind=s+0x4F;
    u8 *entry=s+0x1C;
    do {
        if(test(flags,i)) {
            kind+=0x40;
            entry+=0x40;
            ++i;
        } else {
            *flags|=one<<i;
            *kind=i;
            return entry;
        }
    } while(i<=25);
    return 0;
}
