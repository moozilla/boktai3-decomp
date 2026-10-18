#define FadingSprite_UpdateExpiry sub_08129444
#define Sprite_BlinkUntilExpired sub_082212DC
#define gFadingSpritePool gUnk_02000174
#include "global.h"
struct FadingSprite {
    u8 sprite[0x90];
    u16 age;
    u8 active;
    s8 slot;
    u16 blinkAge;
    u8 resetBlink;
};
struct FadingSpritePool { u8 unknown00[0x38]; u32 used; };
extern struct FadingSpritePool *gUnk_02000174;
u8 sub_082212DC(void *, u32, s32, s32);
void Sprite_Remove(void *);
void sub_08129444(void *unused, struct FadingSprite *sprite)
{
    if (sprite->resetBlink) {
        sprite->resetBlink = 0;
        sprite->blinkAge = 0;
    }
    if (sub_082212DC(sprite, sprite->blinkAge, 60, 40)) {
        struct FadingSpritePool *pool = gUnk_02000174;
        pool->used &= ~(1 << sprite->slot);
        sprite->active = 0;
        Sprite_Remove(sprite);
        sprite->slot = -1;
        sprite->age = 0;
    } else {
        sprite->blinkAge++;
    }
}
