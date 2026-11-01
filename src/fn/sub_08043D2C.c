#ifdef NONMATCHING
// fuzzy: 97.1%
#include "contact_actor.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
struct ContactStartSprite {
    u8 prefix[0x1C];
    struct ContactPosition position;
    u8 unknown24[0x14];
    const void *tileData;
};
static inline s32 TruncateShift12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline u32 NextRandom(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}
// confidence=low: initializes and scatters sixteen contact actors around a supplied position.
s32 ContactActorPool_Start(struct ContactActorPool *pool, const struct ContactPosition *position,
                           const struct ContactPosition *center, u32 speed, u32 parameter)
{
    struct ContactActor *slot;
    s32 index;
    void *work;
    pool->hideRequested = 0;
    pool->clearRequested = 0;
    pool->lifetime = 0;
    work = (void *)&pool->fadeMode;
    *(s8 *)work = 1;
    pool->fadeLevel = 63;
    {
        u8 *bank = gPaletteBankData;
        void *first = bank + 0x5A60;
        void *second = bank + 0x5AA0;
        Palette_InterpolateBanks(pool->tileData, first, second, 63, 6);
    }
    pool->retreatCenter = *center;
    slot = &pool->slots[0];
    index = 0;
    work = gRandomTable;
    do {
        struct ContactStartSprite *sprite;
        struct ContactPosition *actorPosition;
        struct ContactCollider *health;
        u32 radius;

        slot->state = 0;
        slot->enter = 1;
        slot->stateFrame = 0;
        slot->direction = (index * 32) + (NextRandom((const u16 *)work) & 31);
        slot->speed = speed;
        slot->unknown12 = parameter;
        sprite = (void *)(&slot->sprite);
        if (pool->mode == 2) {
            if (index <= 7)
                slot->targetIndex = 1;
            else
                slot->targetIndex = 0;
        } else if (pool->mode == 1)
            slot->targetIndex = 0;
        else {
            slot->targetIndex = -1;
        }
        slot->retargetTimer = 0;
        radius = (NextRandom((const u16 *)work) & 31) + 16;
        slot->desiredHeight = position->y;
        slot->targetHeight = position->y + (NextRandom((const u16 *)work) & 511);
        slot->position.x =
            position->x + TruncateShift12(gUnk_086149C4[(slot->direction + 64) & 255] * radius);
        slot->position.y = slot->targetHeight;
        slot->position.z = TruncateShift12(gUnk_086149C4[slot->direction] * radius) + position->z;
        slot->steeringTimer = (NextRandom((const u16 *)work) & 7) + 7;
        actorPosition = &slot->position;
        sprite->position = slot->position;
        slot->sprite.flags &= ~1;
        ContactActor_SetDirectionalSequence(pool, slot, 0);
        AnimData_SelectPaletteBank(slot->graphics, 0x2D3);
        sprite->tileData = pool->tileData;
        ActorShadow_Enable(slot->shadow);
        Collider_SetPosition(&slot->resourceCollider, actorPosition, 0);
        health = &slot->healthCollider;
        Collider_SetPosition(health, actorPosition, 0);
        health->flags |= 4;
        index++;
        slot++;
    } while (index <= 15);
    pool->state = 1;
    return 0;
}
#else
#include "global.h"
INCLUDE_ASM("asm/nonmatching", ContactActorPool_Start);
#endif
