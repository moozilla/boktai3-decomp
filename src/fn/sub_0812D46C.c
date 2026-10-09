#include "global.h"
extern void (*const gUnk_086107F8[])(void);
void sub_08249240(u8 *,u8 *,void (*)(void));
static inline u8 test(u32 *p,u32 mask) {if(*p&mask)return 1;return 0;}
u32 sub_0812D46C(u8 *s)
{
    u8 *first=s+0x3C;
    if(*(u32 *)(s+0x38)) {
        s32 i=0;
        void (*const *table)(void)=gUnk_086107F8;
        u8 *kind=s+0xD2;
        u8 *entry=first;
        do {
            if(test((u32 *)(s+0x38),1<<i)) sub_08249240(s,entry,table[*kind]);
            kind+=0xA0;
            entry+=0xA0;
            ++i;
        } while(i<=7);
    }
    return 0;
}
