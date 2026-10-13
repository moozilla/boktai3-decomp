#include "global.h"

struct S {
    u8 pad[0x20];
    u16 h20;
};
void Actor_PlayAnimationEntry(struct S *, s32, u32, u32, u32);

void sub_081D1A10(struct S *a, s32 b, u32 c, u32 d, u32 e)
{
    if (b <= 9) {
        Actor_PlayAnimationEntry(a, b + a->h20 * 10, c, d, e);
    } else {
        Actor_PlayAnimationEntry(a, b, c, d, e);
    }
}
