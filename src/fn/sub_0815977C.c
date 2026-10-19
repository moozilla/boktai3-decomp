
#ifdef NONMATCHING
// fuzzy: 74.7%
// foundry draft (worker best 86.7%); what differs is mostly register allocation and instruction order
#define PlayerStats_AddHundredths sub_0815977C
#include "global.h"
#include "player.h"
s32 Div(s32,s32);
static inline u32 ReadFlags(u32 *flags,u32 mask) { return mask & *flags; }
static inline s32 HasFlag(struct Player *p,u32 mask)
{
    u32 offset;
    u32 *flags;
    if (!p) return 0;
    offset=(u32)&((struct Player *)0)->actionFlags;
    flags=(void *)((u8 *)p+offset);
    if(ReadFlags(flags,mask)) return 1;
    return 0;
}

void sub_0815977C(struct Player *p,s32 amount)
{
    if(p->kind==1 && p->actionMode==0) {
        if(!(HasFlag(p,0x2000) && HasFlag(p,0x4000))) {
            if(HasFlag(p,0x2000)) amount*=2;
            else if(HasFlag(p,0x4000)) amount=Div(amount,2);
        }
        p->statHundredths+=amount;
        if(p->statHundredths>999) p->statHundredths=1000;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0815977C);
#endif
