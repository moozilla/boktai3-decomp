#include "global.h"

struct Task { u8 pad[0x10]; u16 f10; u8 pad12[2]; u8 f14; };
extern u16 gUnk_03005240;
struct Task *sub_08219CB4(u32);
void sub_08219DD8(struct Task *, u32);
void sub_08219F74(struct Task *);

struct Task *sub_0821A004(u32 a, u32 b)
{
    struct Task *t = sub_08219CB4(b);
    if (t == 0)
        return 0;
    sub_08219DD8(t, b);
    t->f14 = a;
    if (gUnk_03005240 == 0)
        gUnk_03005240++;
    t->f10 = gUnk_03005240;
    gUnk_03005240++;
    sub_08219F74(t);
    return t;
}
