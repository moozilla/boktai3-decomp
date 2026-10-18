#include "global.h"
extern const u32 gUnk_08613D34[];
void sub_0824923C(u8 *, u32);
void Actor_DispatchAnimationPhase(u8 *p)
{
    sub_0824923C(p, gUnk_08613D34[*(u16 *)(p + 0xF5A) >> 4]);
}
