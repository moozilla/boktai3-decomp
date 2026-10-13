// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Motion {
    u8 pad0[6];
    u16 velocity;
};
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void sub_08039920(struct Motion *arg, int baseArg, int mask)
{
    struct Motion *p = arg;
    int base = baseArg;
    u16 *table = gUnk_0203B400;
    u32 *state = &gUnk_03005308;
    u32 limit;
    register u32 index asm("r0");
    int random;
    index = *state + 1;
    limit = 1023;
    index &= limit;
    random = index * 2;
    random = *(u16 *)(random + (u32)table);
    random &= mask;
    base += random;
    index = (index + 1) & limit;
    *state = index;
    {
        int sign = *(u8 *)&table[index];
        if (sign <= 127)
            p->velocity = base;
        else {
            int negative = -base;
            p->velocity = negative;
        }
    }
}
