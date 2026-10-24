// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
void Shadow_Initialize(void *, void *, s32);
void ContactSwarm_ApplyHealthContact(struct ContactCollider *, struct ContactCollider *,
                                     struct ContactSwarmSlot *);
void ContactSwarm_IgnoreResourceContact(struct ContactCollider *, struct ContactCollider *,
                                        struct ContactSwarmSlot *);
void Collider_SetBodyProperties(void *, u32, u32, u32);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
u32 ContactSwarm_InitializeSlot(struct ContactSwarm *pool, struct ContactSwarmSlot *slot)
{
    struct ContactPackedVector bounds;
    struct ContactPackedVector offset;
    void *graphics;
    struct ContactSwarmSprite *sprite;
    void *shadow;
    struct ContactCollider *collider;
    const struct ContactPosition *position;
    slot->direction = 0;
    slot->speed = 0;
    slot->state = 0;
    slot->enter = 1;
    slot->stateFrame = 0;
    graphics = &slot->display.animation.draw.graphics;
    AnimData_Load(graphics, 0xDA6D);
    {
        void *argument = graphics;
        asm("" : "+r"(argument));
        AnimData_SelectPaletteBank(argument, 0x2D3);
    }
    sprite = &slot->display.animation.draw.sprite;
    Sprite_Init(sprite, graphics, 1);
    sprite->priority = 2;
    position = &sprite->position;
    ZeroFill8((void *)position);
    shadow = slot->shadow;
    Shadow_Initialize(shadow, (void *)position, 0);
    ActorShadow_Disable(shadow);
    collider = &slot->display.healthCollider;
    bounds.x = 32;
    bounds.y = 240;
    bounds.z = 32;
    offset.x = 0;
    offset.y = 120;
    offset.z = 0;
    Collider_Init(collider, 0, 0x5005, 0, 16, &bounds, &offset);
    Collider_SetPosition(collider, (void *)position, 0);
    Collider_SetCallback(collider, ContactSwarm_ApplyHealthContact, slot);
    Collider_SetBodyProperties(collider, 0, 0x80, 0);
    Collider_Refresh(collider);
    collider = &slot->resourceCollider;
    ((struct ContactPosition *)&bounds)->x = 32;
    ((struct ContactPosition *)&bounds)->y = 240;
    ((struct ContactPosition *)&bounds)->z = 32;
    ((struct ContactPosition *)&offset)->x = 0;
    ((struct ContactPosition *)&offset)->y = 120;
    ((struct ContactPosition *)&offset)->z = 0;
    Collider_Init(collider, 0, 0x2001, 0, 16, &bounds, &offset);
    Collider_SetPosition(collider, (void *)position, 0);
    Collider_SetCallback(collider, ContactSwarm_IgnoreResourceContact, slot);
    Collider_SetAttackProperties(collider, 10, 30, 0x8000, 0, 8);
}
