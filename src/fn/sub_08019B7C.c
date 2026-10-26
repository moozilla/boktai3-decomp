#include "global.h"
#include "ram.h"
u8 *SpritePool_Create(void);
u8 *SpritePool_FindFreeSlot(u8 *);
void Sprite_ResetRenderDefaults(u8 *);
u8 *SpritePool_Allocate(void)
{
    u8 *s = gSpritePoolManager;
    u8 *q;
    if (s != 0 || (s = SpritePool_Create()) != 0) {
        q = SpritePool_FindFreeSlot(s);
        if (q != 0) {
            Sprite_ResetRenderDefaults(q + 0xc);
            return q;
        }
    }
    return 0;
}
