#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Reviewed original extent: 384 bytes; candidate is 380 bytes. Initialization ordering/allocation differs.
#include "contact_actor.h"
void Shadow_Initialize(void *, void *, s32);
void ContactActor_ApplyHealthContact(struct ContactCollider *, struct ContactCollider *,
                                     struct ContactActor *);
void ContactActor_ApplyResourceContact(void *, struct ContactCollider *, struct ContactActor *);
void Collider_SetBodyProperties(void *, u32, u32, u32);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
inline static u32 Arg1(u32 k_, struct ContactActor *slot)
{
    return ((u8 *)slot) + k_;
}

u32 ContactActor_InitializeSlot(struct ContactActorPool *pool, struct ContactActor *slot)
{
    u32 zero;
    struct ContactPackedVector bounds;
    struct ContactPackedVector offset;
    void *graphics;
    struct DisplaySequenceSprite *sprite;
    void *shadow;
    struct ContactCollider *collider;
    const struct ContactPosition *position;
    slot->state = 0;
    slot->stateFrame = 0;
    slot->enter = 1;
    slot->direction = 0;
    graphics = slot->graphics;
    AnimData_Load(graphics, 0xDA6D);
    AnimData_SelectPaletteBank(graphics, 0x2D3);
    *((void **)(((u8 *)graphics) + 0xC)) = pool->tileData;
    sprite = &slot->sprite;
    Sprite_Init(sprite, graphics, 1);
    sprite->priority = 2;
    CpuSet((zero = 0, &zero), Arg1(0x100, slot), 0x05000002);
    shadow = slot->shadow;
    Shadow_Initialize(shadow, ((u8 *)slot) + 0x100, 0);
    ActorShadow_Disable(shadow);
    collider = &slot->healthCollider;
    bounds.x = 32;
    bounds.y = 240;
    bounds.z = 32;
    offset.x = 0;
    offset.y = 120;
    offset.z = 0;
    Collider_Init(collider, 0, 0x4005, 0, 16, &bounds, &offset);
    position = &slot->position;
    Collider_SetPosition(collider, (void *)position, 0);
    Collider_SetCallback(collider, ContactActor_ApplyHealthContact, slot);
    Collider_SetBodyProperties(collider, pool->healthColliderId, 0x82, 1);
    Collider_Refresh(collider);
    collider = &slot->resourceCollider;
    ((struct ContactPosition *)(&bounds))->x = 32;
    ((struct ContactPosition *)(&bounds))->y = 240;
    ((struct ContactPosition *)(&bounds))->z = 32;
    ((struct ContactPosition *)(&offset))->x = 0;
    ((struct ContactPosition *)(&offset))->y = 120;
    ((struct ContactPosition *)(&offset))->z = 0;
    Collider_Init(collider, 0, 0x2001, 0, 16, &bounds, &offset);
    Collider_SetPosition(collider, (void *)position, 0);
    Collider_SetCallback(collider, ContactActor_ApplyResourceContact, slot);
    Collider_SetAttackProperties(collider, pool->resourceColliderId, 30, 0x02080000, 0x20000, 30);
}

#else
#include "global.h"
INCLUDE_ASM("asm/nonmatching", ContactActor_InitializeSlot);
#endif
