#include "global.h"
#ifdef NONMATCHING
// fuzzy: 92.0%
// dead movs r5, #0 placement and the store-flag of the flash test
typedef u32 (*ActorUpdate)(u8 *);
void Actor_PlayAnimationEntry(u8 *, u32, u32, u32, u32);
s32 sub_081C8FB0(u8 *, u8 *, u32);
static inline void orr(u16 *a, u32 m)
{
    *a = m | *a;
}
static inline u8 Actor_ShouldFlash(u8 *a, u8 *b)
{
    u8 hit = 0;
    if ((u8)(*(ActorUpdate *)(b + 0x2e4))(b) != 1 && sub_081C8FB0(a, b, 0))
        hit = 1;
    return hit;
}
void sub_081CC4CC(u8 *a, u8 *b)
{
    if (b[9]) {
        b[9] = 0;
        b[7] = 0;
        Actor_PlayAnimationEntry(b, 0, 1, 0, 4);
    }
    orr((u16 *)(b + 0xe), 0x1000);
    if (Actor_ShouldFlash(a, b))
        orr((u16 *)(b + 0xe), 0x800);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081CC4CC);
#endif
