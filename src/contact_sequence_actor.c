#include "contact_sequence_pool.h"
#include "contact_sequence_actor.h"
#include "display_sprite_playback.h"
#include "player_cue_sequence.h"
#include "render.h"

void ContactSequenceActor_SetFlags(struct ContactSequenceActor *actor, u32 flags)
{
    actor->flags |= flags;
}

/* ContactSequenceActor_ClearFlags: Clears the specified mask bits from the actor flags. */

void ContactSequenceActor_ClearFlags(struct ContactSequenceActor *actor, u32 mask)
{
    actor->flags &= ~mask;
}

u32 ContactSequenceActor_GetFlags(struct ContactSequenceActor *actor, u32 mask)
{
    return actor->flags & mask;
}

void ContactSequenceActor_SetState(struct ContactSequenceActor *actor, u32 state)
{
    u32 zero = 0;
    actor->state = state;
    actor->stateFrame = zero;
    actor->enter = 1;
}

u32 ContactSequenceActor_TakeEntryFlag(struct ContactSequenceActor *actor)
{
    u32 result;
    if (!actor->enter) {
        result = 0;
    } else {
        actor->enter = 0;
        result = 1;
    }
    return result;
}

void Collision_CalculateDamageResponse(void *, void *);

void PlayerStats_AddContactHundredths(void *, void *, u32);

void sub_080148CC(u32, u32, void *, u32, u32, u32, u32, u32, u32, u32, u32);

void DamageNumberDisplay_Show(void *, u32, s32, void *, const struct ContactPackedVector *, u32);

void ContactSequenceActor_ApplyHealthContact(struct ContactCollider *source,
                                             struct ContactCollider *other,
                                             struct ContactSequenceActor *actor)
{
    struct ContactPackedVector size;
    struct ContactSequenceActor *saved;
    struct ContactPosition *position;
    saved = actor;
    Collision_CalculateDamageResponse(source, other);
    if (actor->cooldown == 0) {
        PlayerStats_AddContactHundredths(source, other, actor->reactionValue);
        actor->cooldown = 10;
        actor->health -= other->cost;
        actor->flash = 8;
        if (actor->health <= 0) {
            actor->health = 0;
            ContactSequenceActor_SetState(actor, 3);
        }
        position = &saved->position;
        sub_080148CC(2, 14, position, 64, 256, 12, 8, 8, 8, 30, 30);
        size.x = 16;
        size.y = 0;
        size.z = 16;
        DamageNumberDisplay_Show(other, other->cost, 0, position, &size, 1);
        Sound_PlaySongChecked(0x3B9);
    }
}

void Collider_SetBodyProperties(void *, u32, u32, u32);

void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

void ContactSequenceActor_Initialize(struct ContactSequenceOwner *owner,
                                     struct ContactSequenceActor *actor)
{
    struct ContactSwarmGraphics *graphics = &actor->graphics;
    struct ContactSwarmSprite *sprite;
    struct ContactPackedVector bounds, offset;
    struct ContactCollider *collider;
    if (AnimData_Load(graphics, 0x4147)) {
        sprite = &actor->sprite;
        Sprite_Init(sprite, graphics, 1);
        sprite->priority = 2;
        Anim_SetSequence(&actor->sequence, owner->sequenceResource, 0, 0, 0);
        bounds.x = 80;
        bounds.y = 128;
        bounds.z = 80;
        offset.x = 0;
        offset.y = 128;
        offset.z = 0;
        collider = &actor->healthCollider;
        Collider_Init(collider, 0, 0x4005, 0, 16, &bounds, &offset);
        Collider_SetCallback(collider, ContactSequenceActor_ApplyHealthContact, actor);
        Collider_SetBodyProperties(collider, owner->contactValue, 0, 0);
        Collider_Refresh(collider);
        collider = &actor->resourceCollider;
        Collider_Init(collider, 0, 0x2001, 0, 16, &bounds, &offset);
        Collider_SetCallback(collider, 0, actor);
        Collider_SetAttackProperties(collider, 90, 50, 0, 0x60000, 30);
        actor->active = 0;
        actor->reactionValue = owner->reactionValue;
    }
}

void ContactSequenceActor_Deactivate(struct ContactSequenceOwner *owner,
                                     struct ContactSequenceActor *actor)
{
    actor->sprite.flags |= 1;
    ContactSequenceActor_SetState(actor, 0);
    {
        struct ContactCollider *collider = &actor->healthCollider;
        collider->flags |= 4;
    }
    actor->active = 0;
}

/* ContactSequenceActor_UpdateInactive: Empty update callback for the contact sequence actor inactive state. */

void ContactSequenceActor_UpdateInactive(struct ContactSequenceOwner *owner,
                                         struct ContactSequenceActor *actor)
{
}

void Anim_RestartSequence(void *, const void *, u32, u32, u32);

extern const struct ContactSequenceFrame gUnk_08605974[];

void ContactSequenceActor_UpdateAppearing(struct ContactSequenceOwner *parent,
                                          struct ContactSequenceActor *owner)
{
    if (ContactSequenceActor_TakeEntryFlag(owner)) {
        owner->sprite.flags &= ~1;
        Anim_RestartSequence(&owner->sequence, parent->sequenceResource,
                             gUnk_08605974[owner->variant & 1].frame, 0, 8);
    }
    if (owner->sequence.index == 3 && owner->sequence.elapsed == 0)
        Sound_PlaySongChecked(0x3B8);
    ContactSequenceActor_SetFlags(owner, 1);
    if (PlayerCue_StepSequenceEnded(&owner->sprite, &owner->sequence))
        ContactSequenceActor_SetState(owner, 2);
}

void ContactSequenceActor_UpdateActive(struct ContactSequenceOwner *parent,
                                       struct ContactSequenceActor *owner)
{
    ContactSequenceActor_TakeEntryFlag(owner);
    PlayerCue_StepSequence(&owner->sprite, &owner->sequence);
    Collider_Refresh(&owner->resourceCollider);
}

s32 TransientSpriteEffects_Spawn(u32, u32, const struct ContactPosition *,
                                 const struct ContactPosition *, u32, u32);

/* The 63 masks are 16-bit literals: cse shares one HImode constant register (sl) among
   them, and reload takes each SImode use from sl afresh because find_equiv_reg rejects a
   copy made in another mode. */

static inline u32 RandomValue(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}

static inline u32 RandomBits(const u16 *table, u16 mask)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index] & mask;
}

void ContactSequenceActor_UpdateDying(struct ContactSequenceOwner *parent,
                                      struct ContactSequenceActor *actor)
{
    union {
        struct ContactPackedVector bits;
        struct ContactPosition position;
    } scatter;
    struct ContactPosition *position;
    struct ContactPosition *scatterPosition;
    s32 coordinates[3];
    struct ContactSequenceScriptArgs args;
    const u16 *table;

    if (ContactSequenceActor_TakeEntryFlag(actor)) {
        table = (const u16 *)RANDOM_TABLE_ADDR;
        scatter.bits.x = RandomValue(table) & 63;
        scatter.bits.y = RandomBits(table, 127);
        scatter.bits.z = RandomValue(table) & 63;
        scatterPosition = &scatter.position;
        scatterPosition->x += actor->position.x;
        scatterPosition->y += (position = &actor->position)->y;
        scatterPosition->z += position->z;
        TransientSpriteEffects_Spawn(0, 0x1004, scatterPosition, 0, 256, 256);
        {
            u32 sx = (u16)(RandomValue(table) & 63), sy, sz;
            scatterPosition->x = sx;
            sy = RandomBits(table, 127);
            scatterPosition->y = sy;
            sz = (u16)(RandomValue(table) & 63);
            sx += (u16)actor->position.x;
            scatterPosition->x = sx;
            sy += (u16)position->y;
            scatterPosition->y = sy;
            scatterPosition->z = (u16)position->z + sz;
        }
        TransientSpriteEffects_Spawn(0, 0x1004, scatterPosition, 0, 256, 256);
        sub_080148CC(4, 14, position, 64, 256, 12, 8, 8, 8, 30, 30);
        {
            s32 health = actor->health;
            if ((health <= 0) && parent->script) {
                coordinates[0] = actor->position.x;
                coordinates[1] = actor->position.y;
                coordinates[2] = actor->position.z;
                args.count = 3;
                args.values = coordinates;
                Script_Call(parent->script, &args);
            }
        }
        Sound_PlaySongChecked(0x3BA);
    }
    ContactSequenceActor_SetFlags(actor, 1);
    ContactSequenceActor_Deactivate(parent, actor);
}

extern void (*const gUnk_086059A4[])(struct ContactSequenceOwner *, struct ContactSequenceActor *);

static inline s32 scaleOffset(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

extern const u32 gUnk_0860597C[];

void ContactSequenceActor_Update(struct ContactSequenceOwner *parent,
                                 struct ContactSequenceActor *actor)
{
    struct ContactPosition jitter;
    actor->flags = 0;
    gUnk_086059A4[actor->state](parent, actor);
    if (actor->cooldown) {
        AnimData_SelectPaletteBank(&actor->graphics, 0x192);
        ContactSequenceActor_SetFlags(actor, 1);
        --actor->cooldown;
    } else {
        u32 tile = gUnk_0860597C[actor->sequence.index];
        AnimData_SelectPaletteBank(&actor->graphics, tile);
        if (tile == 0x165)
            actor->graphics.paletteData = parent->palette;
    }
    if (actor->sequence.index <= 2)
        actor->sprite.priority = 3;
    else
        actor->sprite.priority = 2;
    if (ContactSequenceActor_GetFlags(actor, 1)) {
        struct ContactCollider *collider = &actor->healthCollider;
        collider->flags |= 4;
    } else {
        struct ContactCollider *collider = &actor->healthCollider;
        s32 mask = -5;
        collider->flags &= mask;
    }
    if (actor->flash) {
        --actor->flash;
        if (!actor->flash) {
            actor->sprite.position = actor->position;
        } else {
            struct ContactPosition *offset;
            u32 index = (gRandomIndex + 1) & 1023;
            const u8 *sample;
            u32 mask;
            u32 flash;
            u32 direction;
            u32 amplitude;
            gRandomIndex = index;
            sample = (const u8 *)RANDOM_TABLE_ADDR + index * 2;
            mask = 255;
            flash = actor->flash;
            offset = &jitter;
            direction = *sample;
            amplitude = flash * 4;
            offset->x = scaleOffset(gUnk_086149C4[(direction + 64) & mask] * amplitude);
            offset->y = 0;
            offset->z = scaleOffset(gUnk_086149C4[direction] * amplitude);
            {
                struct ContactPosition *destination = &actor->sprite.position;
                const struct ContactPosition *source = &jitter;
                const struct ContactPosition *first = &jitter;
                destination->x = first->x + actor->position.x;
                destination->y = actor->position.y + source->y;
                destination->z = source->z + actor->position.z;
            }
        }
    }
    actor->healthCollider.cost = 0;
    ++actor->stateFrame;
}

void ContactSequenceActor_Destroy(struct ContactSequenceOwner *owner,
                                  struct ContactSequenceActor *actor)
{
    Collider_Remove(&actor->healthCollider);
    Sprite_Remove(&actor->sprite);
}

void Palette_InterpolateBanks(void *, void *, void *, u32, u32);

s32 ContactSequencePool_Update(struct ContactSequenceOwner *owner)
{
    struct ContactSequenceActor *actor;
    s32 remaining;
    owner->activeCount = 0;
    actor = owner->actors;
    remaining = 7;
    do {
        if (actor->active) {
            ContactSequenceActor_Update(owner, actor);
            if (actor->state == 2)
                ++owner->activeCount;
        }
        --remaining;
        ++actor;
    } while (remaining >= 0);
    if (owner->activeCount) {
        if (!owner->fadeReverse) {
            ++owner->fadeLevel;
            if (owner->fadeLevel > 31) {
                owner->fadeLevel = 32;
                owner->fadeReverse = 1;
            }
        } else {
            --owner->fadeLevel;
            if (owner->fadeLevel <= 0) {
                owner->fadeLevel = 0;
                owner->fadeReverse = 0;
            }
        }
        {
            u8 *bank = gPaletteBankData;
            u8 *first, *second;
            first = bank + 0x2C00;
            second = bank + 0x2CA0;
            Palette_InterpolateBanks(owner->palette, first, second, owner->fadeLevel, 5);
        }
    }
    return 0;
}

s32 ContactSequencePool_Destroy(struct ContactSequenceOwner *owner)
{
    struct ContactSequenceActor *actor = owner->actors;
    s32 remaining = 7;
    do {
        ContactSequenceActor_Destroy(owner, actor);
        --remaining;
        ++actor;
    } while (remaining >= 0);
    return 0;
}

s32 ContactSequencePool_Initialize(struct ContactSequenceOwner *owner, s32 initialHealth,
                                   u32 contactValue, s32 resourceValue, u32 script,
                                   u32 reactionValue)
{
    struct ContactSequenceActor *actor;
    s32 remaining;
    owner->initialHealth = initialHealth;
    owner->contactValue = contactValue;
    owner->resourceValue = resourceValue;
    owner->script = script;
    owner->reactionValue = reactionValue;
    owner->sequenceResource = ScriptProc_Start(0x922E, 0xC5E2);
    actor = owner->actors;
    remaining = 7;
    do {
        ContactSequenceActor_Initialize(owner, actor);
        --remaining;
        ++actor;
    } while (remaining >= 0);
    CpuSet(gPaletteBankData + 0x2C00, owner->palette, 0x04000008);
    return 0;
}

struct ContactSequenceOwner *ContactSequencePool_Create(s32 initialHealth, u32 contactValue,
                                                        s32 resourceValue, u32 script,
                                                        u32 reactionValue)
{
    struct ContactSequenceOwner *owner = Task_Create(8, sizeof(*owner));
    if (owner) {
        Task_SetCallbacks(owner, (void (*)(void *))ContactSequencePool_Update,
                          (void (*)(void *))ContactSequencePool_Destroy);
        if (ContactSequencePool_Initialize(owner, initialHealth, contactValue, resourceValue,
                                           script, reactionValue) < 0) {
            Task_Kill(owner);
            return NULL;
        }
    }
    return owner;
}

struct ContactSequenceActor *
ContactSequencePool_FindInactiveActor(struct ContactSequenceOwner *owner)
{
    struct ContactSequenceActor *actor = owner->actors;
    s32 index = 0;
    struct ContactSequenceActor *result;
    do {
        if (!actor->active) {
            result = actor;
            goto done;
        }
        ++index;
        ++actor;
    } while (index <= 7);
    result = NULL;
done:
    return result;
}

struct ContactSequenceActor *ContactSequencePool_FindInactiveActor(struct ContactSequenceOwner *);

void *TerrainModifiers_FindCellOverride(u32, u32);

void Collider_SetAttackPowerAndForce(void *, u32, u32);

s32 ContactSequencePool_SpawnActor(struct ContactSequenceOwner *pool,
                                   const struct ContactPosition *position, u32 variant)
{
    u32 savedVariant;
    struct ContactSequenceOwner *owner;
    struct ContactSequenceActor *actor;
    struct ContactPosition *actorPosition;
    struct ContactPosition *savedPosition;
    s32 x, z;
    u32 cell;
    const u8 *entry;
    u32 height, slope;
    struct ContactCollider *healthCollider, *resourceCollider;
    savedVariant = variant;
    owner = pool;
    if (!owner)
        return -1;
    actor = ContactSequencePool_FindInactiveActor(owner);
    if (!actor)
        return -1;
    actor->position = *position;
    actorPosition = &actor->position;
    x = actor->position.x >> 8;
    z = actorPosition->z >> 8;
    savedPosition = actorPosition;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    entry = TerrainModifiers_FindCellOverride(cell, 1);
    if (entry)
        entry += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        entry = map->cells + offset;
    }
    slope = *entry >> 4;
    height = (*entry & 15) << 8;
    switch (slope) {
        case 1:
            height -= (u8)actorPosition->z;
            break;
        case 2:
            height -= (u8)actorPosition->x;
            break;
    }
    actor->position.y = height;
    actor->sprite.position = actor->position;
    healthCollider = &actor->healthCollider;
    Collider_SetPosition(healthCollider, savedPosition, 0);
    resourceCollider = &actor->resourceCollider;
    Collider_SetPosition(resourceCollider, savedPosition, 0);
    Collider_SetAttackPowerAndForce(resourceCollider, (u16)owner->resourceValue, 50);
    Collider_SetBodyProperties(healthCollider, owner->contactValue, 0, 0);
    ContactSequenceActor_SetState(actor, 1);
    actor->stateFrame = 0;
    actor->flash = 0;
    actor->health = owner->initialHealth;
    actor->variant = savedVariant;
    {
        struct DisplaySequenceState *state = &actor->sequence;
        void *resource = (void *)owner->sequenceResource;
        const struct ContactSequenceFrame *frames = gUnk_08605974;
        u32 one = 1;
        Anim_SetSequence(state, resource, frames[savedVariant & one].frame, 0, 8);
        actor->cooldown = 0;
        actor->active = one;
    }
    return 0;
}

s32 ContactSequencePool_BeginAllDeaths(struct ContactSequenceOwner *owner)
{
    struct ContactSequenceActor *actor;
    s32 remaining;
    if (!owner)
        return -1;
    actor = owner->actors;
    remaining = 7;
    do {
        if (actor->active && actor->state <= 2)
            ContactSequenceActor_SetState(actor, 3);
        --remaining;
        ++actor;
    } while (remaining >= 0);
    return 0;
}

u32 ContactSequencePool_GetActiveCount(struct ContactSequenceOwner *owner)
{
    if (!owner)
        return 0;
    return owner->activeCount;
}
