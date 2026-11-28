#include "scene/collision/contact_swarm.h"
void Shadow_Initialize(void *, void *, s32);
void CountBats_ApplyHealthContact(struct ContactCollider *, struct ContactCollider *,
                                  struct ContactSwarmSlot *);
void CountBats_IgnoreResourceContact(struct ContactCollider *, struct ContactCollider *,
                                     struct ContactSwarmSlot *);
void Collider_SetBodyProperties(void *, u32, u32, u32);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
static inline void selectSwarmSlotPalette(struct ContactSwarmGraphics *graphics)
{
    AnimData_SelectPaletteBank(graphics, 0x2D3);
}
u32 CountBats_InitializeSlot(struct ContactSwarm *pool, struct ContactSwarmSlot *slot)
{
    struct ContactPackedVector bounds;
    struct ContactPackedVector offset;
    struct ContactSwarmGraphics *graphics;
    struct ContactSwarmSprite *sprite;
    struct ContactCollider *collider;
    slot->direction = 0;
    slot->speed = 0;
    slot->state = 0;
    slot->enter = 1;
    slot->stateFrame = 0;
    graphics = &slot->display.animation.draw.graphics;
    AnimData_Load(graphics, 0xDA6D);
    selectSwarmSlotPalette(graphics);
    sprite = &slot->display.animation.draw.sprite;
    Sprite_Init(sprite, graphics, 1);
    sprite->priority = 2;
    ZeroFill8(&sprite->position);
    Shadow_Initialize(slot->shadow, &sprite->position, 0);
    ActorShadow_Disable(slot->shadow);
    collider = &slot->display.healthCollider;
    bounds.x = 32;
    bounds.y = 240;
    bounds.z = 32;
    offset.x = 0;
    offset.y = 120;
    offset.z = 0;
    Collider_Init(collider, 0, 0x5005, 0, 16, &bounds, &offset);
    Collider_SetPosition(collider, &sprite->position, 0);
    Collider_SetCallback(collider, CountBats_ApplyHealthContact, slot);
    Collider_SetBodyProperties(collider, 0, 0x80, 0);
    Collider_Refresh(collider);
    collider = &slot->resourceCollider;
    bounds.x = 32;
    bounds.y = 240;
    bounds.z = 32;
    offset.x = 0;
    offset.y = 120;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, 16, &bounds, &offset);
    Collider_SetPosition(collider, &sprite->position, 0);
    Collider_SetCallback(collider, CountBats_IgnoreResourceContact, slot);
    Collider_SetAttackProperties(collider, 10, 30, 0x8000, 0, 8);
}
