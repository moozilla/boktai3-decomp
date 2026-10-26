#include "contact_burst.h"
void ContactBurst_OnContact(void *, void *, struct ContactBurst *);
void Collider_SetBodyProperties(void *, u32, u32, u32);
s32 ContactBurst_Initialize(struct ContactBurst *effect, const struct ContactPosition *world)
{
    struct ContactSwarmGraphics *graphics;
    struct TransientEffectSprite *sprite;
    struct ContactCollider *collider;
    struct ContactPackedVector bounds, offset;
    s32 sample;
    s32 status;
    effect->enabled = 0;
    effect->previousEnabled = 0;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    sample = ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
    effect->contactFrame = Mod(sample >> 8, 3) * 2 + 32;
    effect->fade = 1;
    effect->world = world;
    graphics = &effect->graphics;
    if (!AnimData_Load(graphics, 0x4147)) {
        status = -1;
        goto done;
    }
    sprite = &effect->sprite;
    Sprite_Init(sprite, graphics, 0x211);
    AnimData_SelectPaletteBank(graphics, 0x164);
    sprite->frame = 27;
    sprite->priority = 1;
    bounds.x = 96;
    bounds.y = 128;
    bounds.z = 96;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    collider = &effect->collider;
    Collider_Init(collider, 0, 0x4005, 0, 16, &bounds, &offset);
    Collider_SetCallback(collider, (void *)ContactBurst_OnContact, effect);
    Collider_SetBodyProperties(collider, 0, 0, 0);
    /* The ROM forwards Collider_Refresh's incidental nonnegative return word. */
    status = Collider_Refresh(collider);
done:
    return status;
}
