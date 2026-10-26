#include "global.h"
#include "ram.h"
/* SpritePool_Remove: Recovered byte-tail function; looks up a slot by id via SpritePool_Find and removes its sprite, clearing active flag and id */
#define SpritePool_Remove sub_08019E2C
#define SpritePool_Remove sub_08019E2C
typedef struct {
    u32 id;
    u8 active, pad[7];
    u8 sprite[44];
} Slot;
Slot *SpritePool_Find(void *, u32);
void Sprite_Remove(void *);
void SpritePool_Remove(u32 id)
{
    if (gSpritePoolManager) {
        Slot *p = SpritePool_Find(gSpritePoolManager, id);
        if (p) {
            Sprite_Remove(p->sprite);
            p->active = 0;
            p->id = 0;
        }
    }
}
