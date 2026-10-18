#define TimedSprite_BlinkAndRemove sub_08127A38
#include "global.h"
struct TimedSprite {
    u32 flags;
    u8 unknown04[0x96];
    u8 active;
    s8 slot;
    u16 age;
};
struct TimedSpritePool {
    u8 unknown00[0x38];
    u32 used;
};
extern const u8 gUnk_08610780[];
s32 Mod(s32, s32);
void Sprite_Remove(void *);
void sub_08127A38(struct TimedSpritePool *pool, struct TimedSprite *sprite)
{
    struct TimedSprite *copy = sprite;
    u32 rate;
    if (copy->age > 19) {
        rate = 2;
        if (copy->age <= 50) {
            rate = 0;
            if (copy->age > 30) rate = 1;
        }
        if (Mod(copy->age, gUnk_08610780[rate]) == 0) {
            u32 flags = sprite->flags;
            u32 bit = 1;
            if (flags & bit) flags &= ~1;
            else flags |= bit;
            sprite->flags = flags;
        }
        if (copy->age > 70) {
            sprite->flags |= 1;
            pool->used &= ~(1 << copy->slot);
            copy->active = 0;
            Sprite_Remove(sprite);
            copy->slot = -1;
        }
    }
}
