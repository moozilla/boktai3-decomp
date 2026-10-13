// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneActors_StartDirectionalAnimation sub_0802175C
void sub_08220D78(void *, int, u16, u16, u16);
void SceneActors_StartDirectionalAnimation(u8 *p, u8 *animation, int resource, int sequence,
                                           int speed, int offset, int flags)
{
    int savedFlags = flags;
    flags = savedFlags;
    if ((u8)(p[0xa8] - 1) <= 1)
        p[0xb6] = offset;
    else
        p[0xb6] = 0;
    if (p[0xa8] > 1)
        p[0xb7] = 1;
    else
        p[0xb7] = 0;
    {
        int selected = p[0xb6] + sequence;
        *(u16 *)(p + 0xb8) = selected;
    }
    sub_08220D78(animation, resource, *(u16 *)(p + 0xb8), speed, flags);
    flags &= 1;
    if (!flags) {
        asm("" : "+r"(flags));
        p[0xa3] = 0;
    }
    if (p[0xb7])
        *(u32 *)(animation + 8) |= 4;
    else
        *(u32 *)(animation + 8) &= ~4;
}
