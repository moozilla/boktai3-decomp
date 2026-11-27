#include "global.h"
struct HudAnimation {
    u8 bytes[0x60];
};
struct HudManager {
    u8 pad0[0x184];
    u8 smallSprite[0xC28 - 0x184];
    u8 largeSprite[0x1420 - 0xC28];
    struct HudAnimation animations[124];
    u8 pad42A0[4];
    struct HudAnimation extraAnimations[2];
};
void Anim_Remove(void *);
void Sprite_Remove(void *);
void Hud_RemoveMenuGraphics(struct HudManager *p)
{
    struct HudAnimation *anim = p->animations;
    s32 i = 123;
    do {
        Anim_Remove(anim);
        anim++;
    } while (--i >= 0);
    Sprite_Remove(p->largeSprite);
    Anim_Remove(&p->extraAnimations[0]);
    Anim_Remove(&p->extraAnimations[1]);
    Sprite_Remove(p->smallSprite);
}
