#include "global.h"
#define OrbitEffect_RemoveSlot sub_081FE244

struct OrbitSlotSprite {
    u32 flags;
    u8 unknown04[0x28];
};
struct OrbitSlotCollider {
    u8 unknown00[6];
    u16 flags;
    u8 unknown08[0x58];
};
struct OrbitEffectSlot {
    u32 state;
    u8 unknown04[8];
    u32 position[2];
    u8 unknown14[4];
    u16 motionMode;
    u8 unknown1a[0x1a];
    struct OrbitSlotSprite sprite;
    struct OrbitSlotSprite secondarySprite;
    u8 unknown8c[0x14];
    struct OrbitSlotCollider collider;
};
struct OrbitEffectManager {
    u8 unknown00[0x18];
    u32 activeSlots;
    u8 unknown1c[0xeec - 0x1c];
    u32 activeCount;
};
void Sound_StopSong(u32);
void sub_081F33F0(s32);
void OrbitEffect_RemoveTrailSprites(struct OrbitEffectSlot *);
void Sprite_Remove(void *);

void sub_081FE244(struct OrbitEffectManager *manager, struct OrbitEffectSlot *slot, s32 index)
{
    manager->activeCount--;
    Sound_StopSong(0x564);
    sub_081F33F0(index);
    OrbitEffect_RemoveTrailSprites(slot);
    slot->motionMode = 0;
    {
        struct OrbitSlotCollider *collider = &slot->collider;
        collider->flags |= 4;
    }
    slot->secondarySprite.flags = (slot->secondarySprite.flags & ~2) | 1;
    Sprite_Remove(&slot->secondarySprite);
    slot->sprite.flags = (slot->sprite.flags & ~2) | 1;
    Sprite_Remove(&slot->sprite);
    manager->activeSlots &= ~(1 << index);
}
