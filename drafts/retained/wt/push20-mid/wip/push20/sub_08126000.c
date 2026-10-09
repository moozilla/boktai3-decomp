#include "global.h"
extern void (*const gUnk_08610740[])(void);
void sub_08249240(u8 *,u8 *,void (*)(void));
static inline u8 test(u32 *p,u32 mask) {if(*p&mask)return 1;return 0;}
u32 sub_08126000(u8 *s)
{
    u8 *first=s+0x1C;
    if(*(u32 *)((u32)s+0x69C)) {
        s32 i=0;
        u32 *flags=(u32 *)(0x69C+s);
        void (*const *table)(void)=gUnk_08610740;
        u8 *kind=s+0x4E;
        u8 *entry=first;
        do {
            if(test(flags,1<<i)) sub_08249240(s,entry,table[*kind]);
            kind+=0x40;
            entry+=0x40;
            ++i;
        } while(i<=25);
    }
    return 0;
}
