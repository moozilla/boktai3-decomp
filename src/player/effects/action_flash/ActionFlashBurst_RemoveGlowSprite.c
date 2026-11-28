#include "global.h"
#include "engine/video/sprite.h"

struct ActionFlashGlowOwner {
    u8 unknown0[0xC0];
    struct DisplaySequenceSprite glow;
};

u32 Sprite_Remove(struct DisplaySequenceSprite *);

void ActionFlashBurst_RemoveGlowSprite(struct ActionFlashGlowOwner *burst)
{
    Sprite_Remove(&burst->glow);
}
