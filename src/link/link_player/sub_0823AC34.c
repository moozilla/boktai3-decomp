#include "global.h"
struct FlagAC {
    s8 a;
    u8 b, c, d;
};
s32 LinkPlayer_GetAbilityCostById(s32);
s32 LinkPlayer_IsAbilityIdValid(u8 *, s32);
void LinkPlayer_InitAbilityState(u8 *p)
{
    struct FlagAC *f = (struct FlagAC *)(p + 0x33c);
    s8 value = (*(u8 **)(p + 0x344))[0x3e];
    u32 zero = 0;
    f->a = value;
    if (f->a < 0) {
        f->d = zero;
        f->b = zero;
    } else {
        f->d = LinkPlayer_GetAbilityCostById(f->a);
        f->b = LinkPlayer_IsAbilityIdValid(p, f->a);
    }
    f->c = zero;
}
