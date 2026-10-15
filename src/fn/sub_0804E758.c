#include "area_effect.h"
// MATCHING_CONCESSION: register-pinning
#define AreaEffect_UpdateGraphicsId sub_0804E758
u32 sub_08228DB8(void);
void AnimData_SelectTiles(void *, u32);
void AreaEffect_UpdateGraphicsId(struct AreaEffectManager *manager)
{
    u32 id;
    switch (sub_08228DB8()) {
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
            register u8 *cursor asm("r0") = &manager->count;
            s32 n;
            count = cursor;
            n = *cursor;
            if (i < n) {
                register u32 strideSeed asm("r1") = sizeof(struct AreaEffectEntry);
                register u32 stride asm("r9");
                asm("" : "+r"(strideSeed));
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
                            AnimData_SelectTiles(graphics, manager->graphicsId);
                            graphics += 0x1C;
                        } while (--j >= 0);
                    }
                    i = next;
                    {
                        register u8 *readCount asm("r0") = count;
                        n = *readCount;
                    }
                } while (i < n);
            }
        }
    }
}
