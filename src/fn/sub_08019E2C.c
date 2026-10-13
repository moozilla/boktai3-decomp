#include "global.h"
#define SpritePool_Remove sub_08019E2C
typedef struct {
    u32 id;
    u8 active, pad[7];
    u8 sprite[44];
} Slot;
extern void *gUnk_020000B0;
Slot *SpritePool_Find(void *, u32);
void sub_08214514(void *);
void SpritePool_Remove(u32 id)
{
    if (gUnk_020000B0) {
        Slot *p = SpritePool_Find(gUnk_020000B0, id);
        if (p) {
            sub_08214514(p->sprite);
            p->active = 0;
            p->id = 0;
        }
    }
}
