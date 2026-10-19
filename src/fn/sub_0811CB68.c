#include "global.h"
#include "modifier_list.h"
#ifdef NONMATCHING
// fuzzy: 92.3%
// foundry draft (worker best 90.0%); what differs is mostly register allocation and instruction order
#define CharacterPreview_PositionParts sub_0811CB68
void sub_0811CB68(void *resource, struct ModifierDisplayObject *parts,
                  struct ModifierPreviewOrigin *origin, u16 flags)
{
    u16 flip = flags;
    s32 i = 0;
    flip &= 4;
    for (; i <= 4; i++) {
        s32 x = 0;
        s32 y = 0;
        switch (i) {
            case 3:
                x = 15;
                y = -2;
                break;
            case 4:
                x = -19;
                y = -2;
                break;
        }
        if (flip)
            x = -x;
        parts[i].x = origin->x + x;
        parts[i].y = origin->y + y;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0811CB68);
#endif
