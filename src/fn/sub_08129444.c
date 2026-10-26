#include "global.h"
#include "ram.h"
struct FadingSprite {
    u8 sprite[0x90];
    u16 age;
    u8 active;
    s8 slot;
    u16 blinkAge;
    u8 resetBlink;
};
struct FadingSpritePool {
    u8 unknown00[0x38];
    u32 used;
};
u8 Sprite_BlinkUntilExpired(void *, u32, s32, s32);
void Sprite_Remove(void *);
void FadingSprite_UpdateExpiry(void *unused, struct FadingSprite *sprite)
{
    if (sprite->resetBlink) {
        sprite->resetBlink = 0;
        sprite->blinkAge = 0;
    }
    if (Sprite_BlinkUntilExpired(sprite, sprite->blinkAge, 60, 40)) {
        struct FadingSpritePool *pool = gFadingSpritePool;
        pool->used &= ~(1 << sprite->slot);
        sprite->active = 0;
        Sprite_Remove(sprite);
        sprite->slot = -1;
        sprite->age = 0;
    } else {
        sprite->blinkAge++;
    }
}
