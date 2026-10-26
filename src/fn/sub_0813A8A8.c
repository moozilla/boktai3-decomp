#include "global.h"
#include "ram.h"
#define DisplaySlots_Create PositionLinkedSprite_EnsureManager
#define gDisplaySlots       gCompactSpritePool
/* Tentative family: two display slots owned by a task. */
struct DisplaySlot {
    u32 flags;
    u8 unknown[0x38];
    u32 x, y, active;
    u8 tail[0x1c];
};
struct DisplaySlots {
    u8 unknown[0x34];
    struct DisplaySlot slots[2];
};
struct DisplaySlots *PositionLinkedSprite_EnsureManager(void);
s32 DisplaySlots_Reserve(u32 x, u32 y)
{
    struct DisplaySlots *p;
    s32 i;
    if (!gCompactSpritePool) {
        p = PositionLinkedSprite_EnsureManager();
        if (!p)
            return -1;
    } else
        p = gCompactSpritePool;
    for (i = 0; i < 2; i++) {
        if (p->slots[i].active == 0) {
            p->slots[i].x = x;
            p->slots[i].flags |= 1;
            p->slots[i].y = y;
            p->slots[i].active = 1;
            return i;
        }
    }
    return -1;
}
