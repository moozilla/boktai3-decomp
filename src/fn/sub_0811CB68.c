#include "global.h"
#include "modifier_list.h"
void CharacterPreview_PositionParts(void *resource, struct ModifierDisplayObject *parts,
                                    struct ModifierPreviewOrigin *origin, u32 flags)
{
    s32 i = 0;
    u32 flip = flags & 4;
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
