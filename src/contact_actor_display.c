#include "scene_graphics_actor.h"
#include "contact_actor.h"
#include "player.h"
#include "contact_swarm.h"
#include "display_sprite_playback.h"

void Collision_CalculateDamageResponse(void *, void *);

void PlayerStats_AddContactHundredths(void *, void *, u32);

struct SceneGraphicsActor *SceneActors_GetBySlot(s32);

void DamageNumberDisplay_Show(void *, u32, s32, void *, const struct ContactPackedVector *, u32);

void SceneActors_AddHealthCost(u32);

void SparkParticle_Spawn(void *, u32, const void *, u32, u32, u32, u32, u32);

extern const u8 gUnk_0824DE9C[];

void ContactActor_ApplyHealthContact(struct ContactCollider *sourceInput,
                                     struct ContactCollider *otherInput,
                                     struct ContactActor *ownerInput)
{
    struct ContactCollider *source = sourceInput;
    struct ContactCollider *other = otherInput;
    struct ContactActor *owner = ownerInput;
    struct ContactPackedVector size;
    u32 directionMask;
    struct ContactPosition *position;
    u32 mode;
    u32 direction;
    struct SceneGraphicsActor *actor;
    directionMask = 0x100;
    if (source->flags & directionMask) {
        direction = source->direction;
    } else {
        direction = Math_Atan2(other->x - source->x, other->z - source->z);
    }
    do {
        owner->direction = direction;
    } while (0);
    mode = 0;
    if (owner->targetIndex >= 0) {
        owner->retargetTimer = 250;
        owner->targetIndex = mode;
    }
    Collision_CalculateDamageResponse(source, other);
    PlayerStats_AddContactHundredths(source, other, 2);
    actor = SceneActors_GetBySlot(0);
    if (actor) {
        u32 playerMode = gPlayers[0]->actionMode;
        if ((actor->graphics.stepTableKind == 1) && (playerMode != 2)) {
            other->cost = mode;
        }
    }
    size.x = 16;
    size.y = 0;
    size.z = 16;
    {
        u32 cost = other->cost;
        position = &owner->position;
        DamageNumberDisplay_Show(other, cost, 0, position, &size, mode = 1);
    }
    SceneActors_AddHealthCost(other->cost);
    {
        u32 state = 5;
        u32 zero;
        zero = 0;
        owner->state = state;
        owner->enter = mode;
        owner->stateFrame = zero;
    }
    if (!other->cost) {
        Sound_PlaySongChecked(0x211);
    } else {
        Sound_PlaySong(0x108);
        if (source->reactionFlags & mode) {
            u32 index = (gRandomIndex + 1) & 1023;
            u32 angle;
            gRandomIndex = index;
            angle = ((const u8 *)RANDOM_TABLE_ADDR)[index * 2];
            SparkParticle_Spawn(position, 1, gUnk_0824DE9C, angle, 3, 8, 2, 2);
        }
    }
}

void SceneActors_AddResourceCost(u32);

void ContactActor_ApplyResourceContact(void *source, struct ContactCollider *other,
                                       struct ContactActor *owner)
{
    struct ContactPackedVector size;
    size.x = 16;
    size.y = 0;
    size.z = 16;
    DamageNumberDisplay_Show(source, other->cost, 1, &owner->position, &size, 1);
    SceneActors_AddResourceCost(other->cost);
}

void Shadow_Initialize(void *, void *, s32);

void ContactActor_ApplyHealthContact(struct ContactCollider *, struct ContactCollider *,
                                     struct ContactActor *);

void ContactActor_ApplyResourceContact(void *, struct ContactCollider *, struct ContactActor *);

void Collider_SetBodyProperties(void *, u32, u32, u32);

void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

static inline void *SlotSpritePosition(struct ContactActor *slot)
{
    return &slot->sprite.position;
}

static inline void SelectSlotPalette(void *graphics)
{
    AnimData_SelectPaletteBank(graphics, 0x2D3);
}

u32 ContactActor_InitializeSlot(struct ContactActorPool *pool, struct ContactActor *slot)
{
    struct ContactPackedVector bounds;
    struct ContactPackedVector offset;
    struct ContactSwarmGraphics *graphics;
    struct DisplaySequenceSprite *sprite;
    void *shadow;
    struct ContactCollider *collider;
    const struct ContactPosition *position;
    u16 initialFrame[] = {0};
    slot->state = 0;
    slot->enter = 1;
    slot->stateFrame = initialFrame[0];
    slot->direction = 0;
    graphics = (struct ContactSwarmGraphics *)slot->graphics;
    AnimData_Load(graphics, 0xDA6D);
    SelectSlotPalette(graphics);
    graphics->paletteData = pool->tileData;
    sprite = &slot->sprite;
    Sprite_Init(sprite, graphics, 1);
    sprite->priority = 2;
    ZeroFill8(SlotSpritePosition(slot));
    shadow = slot->shadow;
    Shadow_Initialize(shadow, &slot->sprite.position, 0);
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
    Collider_SetAttackProperties(collider, pool->resourceColliderId, 30, 0x208000, 0x20000, 30);
}

void Collider_Remove(void *);

void Shadow_Destroy(void *);

u32 ContactActor_DestroySlot(struct ContactActorPool *unused, struct ContactActor *slot)
{
    Collider_Remove(&slot->healthCollider);
    Shadow_Destroy(slot->shadow);
    Sprite_Remove(&slot->sprite);
    /* Callers ignore the unspecified success result. */
}

s32 ContactActor_HideSlot(struct ContactActorPool *unused, struct ContactActor *slot,
                          s32 unusedIndex)
{
    u32 *flags = &slot->sprite.flags;
    struct ContactCollider *collider;
    *flags |= 1;
    collider = &slot->healthCollider;
    collider->flags |= 4;
    ActorShadow_Disable(slot->shadow);
    /* The observed callers ignore the unspecified success result. */
}

s32 ContactActor_HideSlot(struct ContactActorPool *, struct ContactActor *, s32);

void SceneActors_StartDisappearEffect(void *);

void ContactActorPool_HideSlots(struct ContactActorPool *pool)
{
    struct ContactActor *slot = pool->slots;
    s32 i = 0;
    do {
        ContactActor_HideSlot(pool, slot, i);
        i++;
        slot++;
    } while (i <= 15);
    SceneActors_StartDisappearEffect(&pool->retreatCenter);
    pool->state = 0;
}

void ContactActor_SetDirectionalSequence(struct ContactActorPool *pool, struct ContactActor *slot,
                                         u32 index)
{
    u8 flip[2];
    u32 direction = ((slot->direction + 96) >> 6) & 3;
    u8 *first = &flip[0];
    u8 *second = &flip[1];
    u8 *saved;
    u32 check;
    check = (u8)(direction - 1);
    saved = second;
    if (check <= 1)
        *first = 1;
    else
        *first = 0;
    if (direction > 1)
        *second = 1;
    else
        *second = 0;
    Anim_SetSequence(&slot->sequence, pool->sequenceResource, (u16)index, flip[0], *saved);
}

void ContactActor_UpdateDisplay(struct ContactActor *owner)
{
    struct ContactSwarmSprite *sprite = (struct ContactSwarmSprite *)&owner->sprite;
    s32 diff;
    s32 wave, offset;
    owner->sprite.position = owner->position;
    owner->finished = PlayerCue_StepSequenceEnded(sprite, &owner->sequence);
    diff = owner->position.y - owner->targetHeight;
    if (diff <= -4)
        owner->position.y += 4;
    else if (diff > 3)
        owner->position.y -= 4;
    else
        owner->position.y = owner->targetHeight;
    wave = gUnk_086149C4[owner->bobPhase] * 32;
    if (wave >= 0)
        offset = wave >> 12;
    else
        offset = -((-wave) >> 12);
    sprite->position.y += offset;
    owner->bobPhase += 4;
}

void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);

static inline s32 truncateShift12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

void ContactActor_UpdateAdvance(struct ContactActorPool *pool, struct ContactActor *slotInput,
                                s32 indexInput)
{
    struct ContactActor *slot = slotInput;
    s32 index = indexInput;
    struct ContactPosition delta;
    struct ContactPosition *write;
    u32 rawSpeed;
    u32 direction;
    s32 speed;
    if (slot->enter)
        slot->enter = 0;
    rawSpeed = slot->speed;
    write = &delta;
    direction = slot->direction;
    speed = rawSpeed * 2;
    write->x = truncateShift12(gUnk_086149C4[(direction + 64) & 255] * speed);
    write->y = 0;
    {
        const s16 *entry = gUnk_086149C4 + direction;
        write->z = truncateShift12(*entry * speed);
    }
    {
        struct ContactPosition *position = &slot->position;
        const struct ContactPosition *read = &delta;
        {
            const struct ContactPosition *first = &delta;
            slot->position.x += first->x;
        }
        position->y += read->y;
        position->z += read->z;
        Collider_SetPosition(&slot->healthCollider, position, 0);
    }
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    if (slot->stateFrame >= index + 2) {
        slot->state = 1;
        slot->enter = 1;
        slot->stateFrame = 0;
    }
    slot->stateFrame++;
}
