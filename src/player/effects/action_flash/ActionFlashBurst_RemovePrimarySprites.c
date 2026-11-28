#include "global.h"
#include "engine/video/sprite.h"

struct ActionFlashSpriteOwner {
    u8 unknown0[0x20];
    struct DisplaySequenceSprite orbit;
    u8 orbitResource[0x1C];
    struct DisplaySequenceSprite flash;
};

void Sprite_Remove(struct DisplaySequenceSprite *);

void ActionFlashBurst_RemovePrimarySprites(struct ActionFlashSpriteOwner *burst)
{
    Sprite_Remove(&burst->orbit);
    Sprite_Remove(&burst->flash);
}
