#include "area_effect.h"
u32 SunClock_GetPhase(void);
void AnimData_SelectPaletteBank(void *, u32);
void AreaEffect_UpdateGraphicsId(struct AreaEffectManager *manager)
{
    u32 id;
    switch (SunClock_GetPhase()) {
        case 3:
        case 4:
            id = 0x1C8;
            break;
        case 5:
            id = 0x1C9;
            break;
        default:
        case 0:
        case 1:
        case 2:
            id = 0x1C7;
            break;
    }
    if (manager->graphicsId != id) {
        s32 i;
        u8 *count;
        manager->graphicsId = id;
        i = 0;
        {
            u8 *cursor = &manager->count;
            s32 n;
            n = *cursor;
            count = cursor;
            if (i < n) {
                u32 strideSeed = sizeof(struct AreaEffectEntry);
                u32 stride;
                stride = strideSeed;
                do {
                    u32 offset = stride * i;
                    u32 hasSprite = *((u8 *)manager + offset + 0xC9);
                    s32 next = i + 1;
                    if (hasSprite) {
                        u32 graphicsOffset = offset + 0x8C;
                        u8 *graphics = (u8 *)manager + graphicsOffset;
                        s32 j = 1;
                        do {
                            AnimData_SelectPaletteBank(graphics, manager->graphicsId);
                            graphics += 0x1C;
                        } while (--j >= 0);
                    }
                    i = next;
                    {
                        u8 *readCount = count;
                        n = *readCount;
                    }
                } while (i < n);
            }
        }
    }
}
