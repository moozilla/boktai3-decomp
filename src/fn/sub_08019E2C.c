#include "global.h"
#define SpritePool_Remove sub_08019E2C
typedef struct {
    u32 id;
    u8 active, pad[7];
    u8 sprite[44];
} Slot;
extern void *gUnk_020000B0;
Slot *SpritePool_Find(void *, u32);
void Sprite_Remove(void *);
void SpritePool_Remove(u32 id)
{
    if (gUnk_020000B0) {
        Slot *p = SpritePool_Find(gUnk_020000B0, id);
        if (p) {
            Sprite_Remove(p->sprite);
            p->active = 0;
            p->id = 0;
        }
    }
}
