// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#define ContactSwarm_InitializeSlot     sub_08043FB0
#define ContactSwarm_ApplyHealthContact sub_08043F64
void Shadow_Initialize(void *, void *, s32);
void ContactSwarm_ApplyHealthContact(struct ContactCollider *, struct ContactCollider *,
                                     struct ContactSwarmSlot *);
void sub_08043FAC(void *, struct ContactCollider *, struct ContactSwarmSlot *);
void sub_0821FF7C(void *, u32, u32, u32);
void sub_0821FF58(void *, u32, u32, u32, u32, u32);
u32 ContactSwarm_InitializeSlot(struct ContactSwarm *pool, struct ContactSwarmSlot *slot)
{
    u32 zero;
    struct ContactPackedVector bounds;
    struct ContactPackedVector offset;
    void *graphics;
    struct ContactSwarmSprite *sprite;
    void *shadow;
    struct ContactCollider *collider;
    register const struct ContactPosition *position asm("r9");
    slot->direction = 0;
    slot->speed = 0;
    slot->state = 0;
    slot->enter = 1;
    slot->stateFrame = 0;
    graphics = slot->graphics;
    AnimData_Load(graphics, 0xDA6D);
    {
        void *argument = graphics;
        asm("" : "+r"(argument));
        AnimData_SelectTiles(argument, 0x2D3);
    }
    sprite = &slot->sprite;
    Sprite_Init(sprite, graphics, 1);
    sprite->priority = 2;
    {
        u32 offset = 0xE8;
        struct ContactPosition *destination;
        const u32 *source;
        asm("" : "+r"(offset));
        destination = (void *)(offset + (u32)slot);
        position = destination;
        zero = 0;
        source = &zero;
        asm("" : "+r"(source));
        CpuSet(source, destination, 0x05000002);
    }
    shadow = slot->shadow;
    Shadow_Initialize(shadow, (void *)position, 0);
    ActorShadow_Disable(shadow);
    collider = &slot->healthCollider;
    bounds.x = 32;
    bounds.y = 240;
    bounds.z = 32;
    offset.x = 0;
    offset.y = 120;
    offset.z = 0;
    Collider_Init(collider, 0, 0x5005, 0, 16, &bounds, &offset);
    Collider_SetPosition(collider, (void *)position, 0);
    Collider_SetCallback(collider, ContactSwarm_ApplyHealthContact, slot);
    sub_0821FF7C(collider, 0, 0x80, 0);
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
    Collider_SetCallback(collider, sub_08043FAC, slot);
    sub_0821FF58(collider, 10, 30, 0x8000, 0, 8);
}
