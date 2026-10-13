#include "global.h"
struct Motion {
    u8 pad0[6];
    u16 velocity;
};
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void sub_08039968(struct Motion *p, int base, int mask)
{
    u32 index = (gUnk_03005308 + 1) & 1023;
    gUnk_03005308 = index;
    {
        u32 offset = index * 2;
        int random;
        random = *(u16 *)((u8 *)gUnk_0203B400 + offset);
        random &= mask;
        {
            int result = base + random;
            p->velocity = -result;
        }
    }
}
