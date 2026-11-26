#include "effects/projectiles/timed_sprite.h"
extern const u8 gUnk_08610780[];
s32 Mod(s32, s32);
void Sprite_Remove(void *);
void TimedSprite_BlinkAndRemove(struct TimedSpritePool *pool, struct TimedSprite *sprite)
{
    struct TimedSprite *copy = sprite;
    u32 rate;
    if (copy->blinkAge > 19) {
        rate = 2;
        if (copy->blinkAge <= 50) {
            rate = 0;
            if (copy->blinkAge > 30)
                rate = 1;
        }
        if (Mod(copy->blinkAge, gUnk_08610780[rate]) == 0) {
            u32 flags = sprite->sprite.flags;
            if (flags & 1)
                flags &= ~1;
            else
                flags |= 1;
            sprite->sprite.flags = flags;
        }
        if (copy->blinkAge > 70) {
            sprite->sprite.flags |= 1;
            pool->allocated &= ~(1 << copy->slot);
            copy->state = 0;
            Sprite_Remove(sprite);
            copy->slot = -1;
        }
    }
}
