/* Tentative family and field names, based on static accesses. */
#define SpriteGroup_RemoveSlot sub_081F23A0
#include "global.h"
void Sprite_Remove(void *);
struct SpriteGroupSprite {
    u32 flags;
    u8 unknown4[0x28];
};
struct SpriteGroupSlot {
    u8 unknown0[0xC8];
    u32 flags;
    u8 unknownCC[0x3C];
    struct SpriteGroupSprite sprites[7];
};
struct SpriteGroup {
    u8 unknown0[0x18];
    u32 activeSlots;
};
u32 sub_081F23A0(struct SpriteGroup *p, struct SpriteGroupSlot *slot, s32 index)
{
    struct SpriteGroupSprite *sprite = slot->sprites;
    u32 *flags;
    s32 i = 3;
    do {
        Sprite_Remove(sprite);
        i--;
        sprite++;
    } while (i >= 0);
    sprite = &slot->sprites[4];
    flags = &slot->flags;
    {
        u32 one = 1;
        i = 2;
        do {
            sprite->flags |= one;
            Sprite_Remove(sprite);
            i--;
            sprite++;
        } while (i >= 0);
    }
    {
        u32 one = 1;
        *flags |= one;
        p->activeSlots &= ~(one << index);
    }
}
