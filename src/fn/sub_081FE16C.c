#include "global.h"
struct OrbitEffectSlot {
    u32 state;
    u8 unknown04[8];
    u32 position[2];
    u8 motionParams[0x20];
    u8 sprite[0xc];
    void *spriteAnimation;
    u8 unknown44[0xc];
    s32 spritePosition[3];
    u8 unknown5c[4];
    u8 secondarySprite[0xc];
    void *secondaryAnimation;
    u8 unknown70[0x30];
    u8 collider[0x60];
};
struct OrbitEffectManager {
    u8 unknown00[0x1c];
    u8 animation[0x1c];
};
struct OrbitColliderVector {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
void RisingActorConfig_Initialize(void *);
void Sprite_InitUnlinked(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, u32);
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetBodyProperties(void *, s32, s32, s32);
s32 Collider_Refresh(void *);
void OrbitEffect_FireProjectileOnContact(void);

static inline void ClearOrbitPosition(void *destination)
{
    u32 zero = 0;
    CpuSet(&zero, destination, 0x05000002);
}

void OrbitEffect_InitSlot(struct OrbitEffectManager *manager, struct OrbitEffectSlot *slot,
                          s32 index)
{
    struct OrbitColliderVector size, offset;
    void *collider;
    RisingActorConfig_Initialize(slot->motionParams);
    slot->state = 0;
    ClearOrbitPosition(slot->position);
    Sprite_InitUnlinked(slot->sprite, manager->animation, 3);
    AnimData_SelectPaletteBank(slot->spriteAnimation, 0x51);
    Sprite_InitUnlinked(slot->secondarySprite, manager->animation, 0x403);
    AnimData_SelectPaletteBank(slot->secondaryAnimation, 0x51);
    collider = slot->collider;
    size.x = 0x40;
    size.y = 0x80;
    size.z = 0x40;
    offset.x = 0;
    offset.y = 0x80;
    offset.z = 0;
    Collider_Init(collider, 0, 0x4004, 0, 0x20, &size, &offset);
    Collider_SetPosition(collider, slot->spritePosition, 0);
    Collider_SetCallback(collider, OrbitEffect_FireProjectileOnContact, slot);
    Collider_SetBodyProperties(collider, 0, 0, 0);
    Collider_Refresh(collider);
}
