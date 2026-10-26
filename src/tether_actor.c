#include "tether_actor.h"

#define TetherActor_GetStatusTag sub_08049F54

#define TetherActor_GetSavedKind sub_0804A400

#define TetherActor_ClearInstance sub_080497E4

u8 *TerrainModifiers_FindCellOverride(s32, s32);

extern u32 gUnk_02000560;

static inline s32 scaleOffset(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline s32 scaleEmission(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline u32 cellByte(s32 cell)
{
    u8 *properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = cell * 4 + 12;
        properties = map->cells + offset;
    }
    return *properties;
}

static inline s32 cellIndex_08049840(s32 x, s32 z)
{
    s32 index;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        index = 0;
    else
        index = gUnk_030052F4->rows[z] + x;
    return index;
}

static inline s32 terrainHeight(struct ContactPackedVector *position, s32 index)
{
    u8 *properties = TerrainModifiers_FindCellOverride(index, 1);
    u32 surface, height;
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = index * 4 + 12;
        properties = map->cells + offset;
    }
    surface = *properties >> 4;
    height = (*properties & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}

static inline s32 absolute_08049840(s32 value)
{
    if (value < 0)
        value = -value;
    return value;
}

static inline u8 flaggedCellHandlingEnabled(void)
{
    u32 mask = 0x2000;
    if (gUnk_02000710->flags868 & mask)
        return TRUE;
    return FALSE;
}

static inline s32 cellIndex_0804AF80(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}

static inline u8 *cellProperties(s32 cell)
{
    u8 *properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = cell * 4 + 12;
        properties = map->cells + offset;
    }
    return properties;
}

static inline u8 *propertiesAt_0804B1CC(struct ContactPosition *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    s32 cell;
    u8 *properties;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        cell = 0;
    else
        cell = gUnk_030052F4->rows[z] + x;
    properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = cell * 4 + 12;
        properties = map->cells + offset;
    }
    return properties;
}

static inline s32 cellIndex_0804B6A4(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}

static inline s32 surfaceHeight(const struct ContactPosition *position)
{
    u8 *properties = cellProperties(cellIndex_0804B6A4(position->x >> 8, position->z >> 8));
    u32 surface = *properties >> 4;
    s32 height = (*properties & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}

static inline s32 truncateShift(s32 value, u32 shift)
{
    if (value >= 0)
        return value >> shift;
    return -(-value >> shift);
}

static inline s32 cosine(s32 angle, s32 radius)
{
    s32 sample = gUnk_086149C4[(angle + 64) & 255];
    s32 product;
    product = radius;
    product *= sample;
    {
        s32 result = truncateShift(product, 12);
        return result;
    }
}

static inline s32 sine(s32 angle, s32 radius)
{
    s32 sample = gUnk_086149C4[angle & 255];
    s32 product;
    product = radius;
    product *= sample;
    return truncateShift(product, 12);
}

static inline s32 absolute_0804BB30(s32 value)
{
    if (value < 0)
        value = -value;
    return value;
}

static inline u8 *propertiesAt_0804BCD4(struct ContactPosition *input)
{
    register struct ContactPosition *position asm("r6") = input;
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    s32 cell;
    u8 *properties;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        cell = 0;
    else
        cell = gUnk_030052F4->rows[z] + x;
    properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = cell * 4 + 12;
        properties = map->cells + offset;
    }
    return properties;
}

static inline u32 terrainFlagEnabled(void)
{
    u32 mask = 16;
    u32 flags;
    u32 result;
    flags = gUnk_02000710->flags868;
    if (flags & mask) {
        result = 1;
        asm("" : "+r"(result));
    } else {
        result = 0;
    }
    return result;
}

static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}

static inline u8 *propertiesAt_0804C748(s32 x, s32 z)
{
    s32 cell;
    u8 *properties;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        cell = 0;
    else
        cell = gUnk_030052F4->rows[z] + x;
    properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = cell * 4 + 12;
        properties = map->cells + offset;
    }
    return properties;
}

static inline s32 absolute_0804D270(s32 value)
{
    if (value < 0)
        return -value;
    return value;
}

static inline u32 segmentsOnly(void)
{
    u32 mask = 4;
    return gUnk_02000560 & mask;
}

void *ActorAttachment_UpdateFromDescriptor(void *, void *, u32, u32, u32, u32, u32, u32);

void SceneParticleBurst_CreateWhilePaused(void *, void *, void *, u32, u32, u32);

s32 AnimatedParticles_Create(s32, void *, const void *);

s32 DisplayAnimation_SelectPaletteBank(struct DisplayAnimation *, u32);

s32 DisplayAnimation_SetTileSource(struct DisplayAnimation *, u32);

s32 DisplayAnimation_SetEntry(struct DisplayAnimation *, u32, u32, u32, u32, u32, u32);

s32 DisplayAnimation_UpdatePlayback(struct DisplayAnimation *);

u32 Navigation_FindContainingRegion(void *);

void ProximityVolume_TranslateBounds(void *, void *);

u32 sub_0821F2EC(void *, void *, void *);

void SceneActorContacts_RemoveNode(void *);

void CellObject_Init(void *, void *);

s32 Inventory_AddItem(u32, u32);

u32 ScenePanel_IsPresent(void *);

void Script_ReadReference(void *);

void Script_WriteReferenceValue(void *, u32, s32);

s32 DisplayAnimation_InitializeKeyframes(struct DisplayAnimation *, u32, s32,
                                         const struct DisplayPosition *);

s32 DisplayAnimation_InitializeSequence(struct DisplayAnimation *, s32, u32, s32,
                                        const struct DisplayPosition *);

void DisplayAnimation_SetPriority(struct DisplayAnimation *, u32);

u32 SceneBody_Init(void *, u32, const struct ContactPosition *, u32, u32, void *);

u32 SceneBody_SetSprite(void *, void *);

u32 SceneBody_SetKeyframeDisplay(void *, void *);

u32 SceneActorContacts_RegisterNode(void *, void *, u32);

void TerrainOverlay_QueueAddActor(void *);

void Collider_SetBodyProperties(void *, u32, u32, u32);

void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

s32 Mod(s32, s32);

s32 Div(s32, s32);

void Obj_StepPosition(void *);

u32 NamedObject_FindNearPosition(struct ContactPosition *);

void NavMesh_UpdateProbe(void *, void *);

void SceneActorControl_RequestDeparture(s32);

void DisplayAnimation_SetFlags(struct DisplayAnimation *, u32);

void DisplayAnimation_ClearFlags(struct DisplayAnimation *, u32);

s32 DisplayAnimation_SetPalette(struct DisplayAnimation *, u32, u32, u32);

void Player_EnterKindFour(struct Player *);

void sub_0821DE30(void *);

void SceneBody_Unregister(void *);

void DisplayAnimation_Destroy(void *);

void TetherActor_InitializeSegments(struct TetherActor *actor);

void TetherActor_UpdateSegments(struct TetherActor *input);

void TetherActor_RemoveSegments(struct TetherActor *actor);

void TetherActor_CreateAuxiliarySprite(struct TetherActor *actor);

void TetherActor_RemoveAuxiliarySprite(struct TetherActor *actor);

void TetherActor_UpdateShake(struct TetherActor *actor);

void TetherActor_EmitDirectionalEffect(struct TetherActor *input);

void TetherActor_EmitRandomEffect(struct TetherActor *actor);

void TetherActor_EmitRingEffects(struct TetherActor *actor);

void TetherActor_EmitPointEffect(const struct ContactPackedVector *source, const void *config);

void TetherActor_UpdateGlow(struct TetherActor *actor);

void TetherActor_BlendPalette(struct TetherActor *s);

void TetherActor_SelectPalette(struct TetherActor *input, u32 index);

void TetherActor_UpdatePalette(struct TetherActor *actor);

void TetherActor_UpdateMovingEffects(struct TetherActor *actor);

void TetherActor_UpdateEffects(struct TetherActor *actor);

void TetherActor_SetState(struct TetherActor *actor, u32 state);

s32 TetherActor_SetHeadingEntry(struct TetherActor *actor, u32 entry);

s32 TetherActor_SetCardinalEntry(struct TetherActor *actor, u32 unusedEntry);

s32 TetherActor_SetEntry(struct TetherActor *input, u32 inputBase);

void TetherActor_ApplyPendingEffect(struct TetherActor *actor);

void TetherActor_UpdatePendingEffect(struct TetherActor *actor);

u32 TetherActor_TestInteractionPath(struct TetherActor *p);

void TetherActor_SelectInteractionState(struct TetherActor *actor);

void TetherActor_BeginAuxiliaryState(struct TetherActor *input);

void TetherActor_AdjustSlopeMovement(struct TetherActor *actor);

void TetherActor_ClearInstance(void);

void TetherActor_ClearExternalStatus(void);

s32 TetherActor_TestMatchingStatus(u32 a, u32 b, u32 c, u32 d);

u32 TetherActor_TestNearbyInteraction(struct ContactPosition *position, u32 tag, u32 flags);

void TetherActor_SetInteractionTarget(struct ContactPosition *a, u32 b);

struct ContactPosition *TetherActor_GetPosition(void);

u32 TetherActor_CopySampledPosition(struct ContactPosition *a);

u32 TetherActor_IsState(u32 a);

void TetherActor_SetInteractionPathFromScript(void);

void TetherActor_ResetInteraction(void);

void TetherActor_RequestPaletteBlend(u32 a);

u32 TetherActor_CanStartInteraction(void);

void TetherActor_StartInteractionEffect(void);

u32 TetherActor_IsStateOne(void);

s32 TetherActor_GetParameter(void);

void TetherActor_SetParameter(u32 a);

void TetherActor_BeginDetachedState(u32 a);

void TetherActor_BeginDisplayPositionState(void);

u32 TetherActor_GetStatusFlags(void);

u32 TetherActor_TestStatusFlags(u32 m);

u16 TetherActor_GetStatusTag(void);

void TetherActor_AddStatusFlags(u32 a, u32 b);

void TetherActor_RemoveStatusFlags(u32 m);

u32 TetherActor_CopyPosition(struct ContactPosition *a);

void TetherActor_SetPosition(struct ContactPosition *a);

u32 TetherActor_BeginKindFourStateNine(void);

u32 TetherActor_BeginKindFourStateTen(void);

u32 TetherActor_RequestKindNineAction(u32 a, u32 b);

u8 TetherActor_GetRequestStatus(void);

u8 TetherActor_GetRequestMode(void);

void TetherActor_BeginDirectedState(struct ContactPosition *position);

void TetherActor_WriteTransformToScript(void);

void TetherActor_SetTransformFromScript(void);

void TetherActor_RequestPositionSample(void);

void TetherActor_StartAuxiliaryState(void);

s16 TetherActor_GetSavedKind(void);

s32 TetherActor_GetConfiguredKind(void);

void TetherActor_InitKindFromScript(struct TetherActor *actor, u32 entry);

void TetherActor_InitInteractionFromScript(struct TetherActor *actor);

void TetherActor_InitDisplayFromScript(struct TetherActor *actor);

void TetherActor_InitBodyFromScript(struct TetherActor *actor);

void TetherActor_InitChargeCollider(struct TetherActor *p);

void TetherActor_InitSecondaryCollider(struct TetherActor *p);

void TetherActor_InitStateParameters(struct TetherActor *p);

void TetherActor_AccumulateCharge(struct TetherActor *p, u32 a);

void TetherActor_UpdateChargeDecay(struct TetherActor *p);

void TetherActor_UpdateInteractionCooldown(struct TetherActor *actor);

void TetherActor_UpdateChargeAndCooldown(struct TetherActor *actor);

void TetherActor_UpdateSecondaryColliderValues(struct TetherActor *p);

void TetherActor_OnChargeContact(struct ContactCollider *collider, void *other,
                                 struct TetherActor *inputActor);

void TetherActor_UpdateSampledPosition(struct TetherActor *p);

void TetherActor_HandleFlaggedCell(struct TetherActor *actor);

void TetherActor_UpdateWorldPosition(struct TetherActor *actor);

u32 TetherActor_TestTerrainTransition(struct TetherActor *actor);

u32 TetherActor_SelectNextState(struct TetherActor *p);

void TetherActor_UpdateLockedShake(struct TetherActor *actor);

void TetherActor_UpdateInteractionValue(struct TetherActor *p);

void TetherActor_UpdateIdleState(struct TetherActor *p);

void TetherActor_BlockPlayerAcrossTerrain(struct TetherActor *actor);

void TetherActor_LimitPlayerDistance(struct TetherActor *input);

void TetherActor_UpdateSecondaryLock(struct TetherActor *input);

void TetherActor_UpdateFollowingState(struct TetherActor *actor);

u32 TetherActor_TestStepPosition(struct ContactPosition *origin, struct ContactPosition *input,
                                 u32 inputMask);

u32 TetherActor_TestStepDelta(struct ContactPackedVector *origin, struct ContactPackedVector *delta,
                              u32 mask);

u32 TetherActor_TestNearDelta(s32 a, s32 b);

void TetherActor_UpdateRoamingState(struct TetherActor *actor);

void TetherActor_UpdateWanderingState(struct TetherActor *actor);

void TetherActor_UpdateRecoveryState(struct TetherActor *actor);

void TetherActor_UpdateDisplayDeparture(struct TetherActor *actor);

void TetherActor_UpdateDisplayReturn(struct TetherActor *actor);

void TetherActor_UpdateAuxiliaryTransition(struct TetherActor *actor);

void TetherActor_UpdateDescendingState(struct TetherActor *actor);

void TetherActor_UpdateRapidDescendingState(struct TetherActor *actor);

void TetherActor_UpdateBounceReturn(struct TetherActor *actor);

void TetherActor_UpdateKindNineRecoil(struct TetherActor *actor);

void TetherActor_UpdatePassiveState(struct TetherActor *actor);

void TetherActor_UpdateSpecialEntryTransition(struct TetherActor *p);

void TetherActor_UpdateSpecialEntryState(struct TetherActor *actor);

void TetherActor_UpdateDirectedEntryTransition(struct TetherActor *p);

void TetherActor_UpdateDirectedMovement(struct TetherActor *actor);

void TetherActor_UpdateDirectedState(struct TetherActor *p);

void TetherActor_UpdateReverseDirectedMovement(struct TetherActor *actor);

void TetherActor_UpdateReverseDirectedState(struct TetherActor *actor);

s32 TetherActor_Tick(struct TetherActor *actor);

struct TetherActor *TetherActor_Destroy(struct TetherActor *p);

s32 TetherActor_InitializeFromScript(struct TetherActor *actor, u32 entry, u32 unused);

struct TetherActor *TetherActor_CreateFromScript(u32 a, u32 b);

void TetherActor_InitializeSegments(struct TetherActor *actor)
{
    struct TetherSegment *segment = actor->segments;
    s32 offset = -4;
    s32 remaining = 6;
    do {
        void *resource = AnimData_FindEntry(0x1C1C);
        ParticleSprite_Initialize(segment, resource, 0);
        Effect_SetOrigin(segment, offset, offset);
        segment->priority = 255;
        segment->flags |= 1;
        --remaining;
        ++segment;
    } while (remaining >= 0);
}

#ifdef NONMATCHING
// fuzzy: 94.6%
// fix.py best (sweep after the inlined-insn fix)
// Remaining: the ROM hoists neither z endpoint out of the segment loop and keeps the
// priority in sb; this hoists start.z and spills the priority.
static inline s32 scaleOffsetR(s32 value)
{
    s32 result;
    if (value >= 0)
        result = value >> 12;
    else
        result = -((-value) >> 12);
    return result;
}
void TetherActor_UpdateSegments(struct TetherActor *actor)
{
    struct TetherSegment *segment;
    struct ContactPackedVector start;
    struct ContactPackedVector end;
    s32 x;
    s32 z;
    s32 offsetX;
    s32 offsetZ;
    u32 angle;
    s32 priority;
    s32 i;
    if ((*gUnk_02000580)->tetherMode != 6) {
        s32 n;
        for (n = 0, segment = actor->segments; n < 7; n++, segment++)
            segment->flags |= 1;

        return;
    }
    angle = (((*gUnk_02000580)->heading + 1) & 7) * 32;
    x = gUnk_086149C4[(angle + 64) & 255];
    z = gUnk_086149C4[angle & 255];
    if ((*gUnk_02000580)->side) {
        offsetX = scaleOffsetR(20 * (z - x));
        offsetZ = scaleOffsetR(20 * (x + z));
    } else {
        offsetX = scaleOffsetR(20 * (x + z));
        offsetZ = scaleOffsetR(20 * (z - x));
    }
    start.x = (*gUnk_02000580)->position.x + offsetX;
    start.y = (*gUnk_02000580)->position.y + 220;
    start.z = (*gUnk_02000580)->position.z + offsetZ;
    angle = ((actor->heading + 5) & 7) * 32;
    end.x = actor->position.x + scaleOffsetR(100 * gUnk_086149C4[(angle + 64) & 255]);
    end.y = actor->position.y - 50;
    end.z = actor->position.z + scaleOffsetR(100 * gUnk_086149C4[angle & 255]);
    priority = ((*gUnk_02000580)->heading == 4) ? (-20) : (-2);
    segment = actor->segments;
    i = 0;
    do {
        s32 count = i + 1;
        s32 remaining = 7 - i;
        segment->position.x = ((end.x * count) + (start.x * remaining)) >> 3;
        segment->position.z = ((start.z * remaining) + (end.z * count)) >> 3;
        segment->position.y = ((end.y * count) + (start.y * remaining)) >> 3;
        segment->priority = priority;
        segment->flags &= ~1;
        i = count;
        ++segment;
    } while (i <= 6);
}

#else

INCLUDE_ASM("asm/nonmatching", TetherActor_UpdateSegments);

#endif

void TetherActor_RemoveSegments(struct TetherActor *actor)
{
    struct TetherSegment *segment = actor->segments;
    s32 remaining = 6;
    do {
        Effect_Remove(segment);
        --remaining;
        ++segment;
    } while (remaining >= 0);
}

void TetherActor_CreateAuxiliarySprite(struct TetherActor *actor)
{
    u8 *active = &actor->auxiliaryActive;
    if (!*active) {
        struct ContactSwarmGraphics *graphics = &actor->auxiliaryGraphics;
        struct ContactSwarmSprite *sprite;
        AnimData_Load(graphics, 0x1C1B);
        sprite = &actor->auxiliarySprite;
        Sprite_Init(sprite, graphics, 512);
        Sprite_SetOriginStub(sprite, -8, -8);
        sprite->frame = 6;
        *active = 1;
    }
}

void TetherActor_RemoveAuxiliarySprite(struct TetherActor *actor)
{
    u8 *active = &actor->auxiliaryActive;
    if (*active == 1) {
        Sprite_Remove(&actor->auxiliarySprite);
        *active = 0;
    }
}

void TetherActor_UpdateShake(struct TetherActor *actor)
{
    u32 phase = actor->shakeFrame & 127;
    if (phase > 95) {
        s32 shake;
        if (phase == 96)
            Sound_PlaySong(0x2AA);
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        {
            u32 sample = ((const u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308];
            u32 mask = 1;
            shake = (sample & mask) * 11 - 5;
        }
        actor->shakeX = shake;
        actor->shakeZ = -shake;
    }
    ++actor->shakeFrame;
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_EmitDirectionalEffect(struct TetherActor *input)
{
    struct TetherActor *actor = input;
    u32 angle = ((actor->heading + 1) & 7) * 32;
    const s16 *sourceTable = gUnk_086149C4;
    const s16 *table;
    s32 rawX;
    s32 scaledX;
    s32 scaledZ;
    u32 highX;
    u16 y;
    struct ContactPackedVector position, velocity, size;
    rawX = sourceTable[(angle + 64) & 255] * 100;
    table = sourceTable;
    scaledX = scaleEmission(rawX);
    highX = (u32)((u16)actor->position.x + scaledX) << 16;
    asm("" : "+r"(highX), "+r"(actor));
    y = actor->position.y;
    position.x = highX >> 16;
    position.y = y;
    scaledZ = scaleEmission(table[angle & 255] * 100);
    position.z = actor->position.z + scaledZ;
    velocity.x = scaleEmission(table[(angle + 64) & 255] * 10);
    velocity.y = 0;
    velocity.z = scaleEmission(table[angle & 255] * 10);
    size.x = 32;
    size.y = 0;
    size.z = 32;
    SceneParticleBurst_CreateWhilePaused(&position, &velocity, &size, 1, 3, 4);
}

void TetherActor_EmitRandomEffect(struct TetherActor *actor)
{
    u32 angle;
    const u8 *sample;
    u32 mask;
    struct ContactPackedVector velocity, position, size;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    sample = (const u8 *)RANDOM_TABLE_ADDR + gUnk_03005308 * 2;
    mask = 255;
    angle = *sample;
    velocity.x = scaleEmission(gUnk_086149C4[(angle + 64) & mask] * 32);
    velocity.y = 0;
    velocity.z = scaleEmission(gUnk_086149C4[angle] * 32);
    position.x = actor->position.x + (u16)velocity.x * 4;
    position.y = actor->position.y;
    position.z = actor->position.z + (u16)velocity.z * 4;
    size.x = 0;
    size.y = 0;
    size.z = 0;
    SceneParticleBurst_CreateWhilePaused(&position, &velocity, &size, 1, 3, 4);
    if (angle & 1)
        Sound_PlaySong(0xD3);
}

void TetherActor_EmitRingEffects(struct TetherActor *actor)
{
    struct ContactPackedVector velocity, position, size;
    s32 index = 0;
    do {
        u32 angle = index * 32;
        velocity.x = scaleEmission(gUnk_086149C4[(angle + 64) & 255] * 16);
        velocity.y = 0;
        angle &= 255;
        velocity.z = scaleEmission(gUnk_086149C4[angle] * 16);
        position.x = actor->position.x + (u16)velocity.x * 8;
        position.y = actor->position.y;
        position.z = actor->position.z + (u16)velocity.z * 8;
        size.x = 0;
        size.y = 0;
        size.z = 0;
        SceneParticleBurst_CreateWhilePaused(&position, &velocity, &size, 1, 3, 4);
        ++index;
    } while (index <= 7);
}

union TetherPointPacket {
    struct ContactPackedVector bits;
    u32 words[2];
};

// MATCHING_CONCESSION: register-pinning

void TetherActor_EmitPointEffect(const struct ContactPackedVector *source, const void *config)
{
    union TetherPointPacket position;
    const u16 *table;
    u32 *state;
    u32 *savedState;
    register u32 index asm("r3");
    u32 indexMask;
    s32 sampleX;
    s32 sampleZ;
    s32 sampleY;
    register u32 jitterMask asm("r9");
    register u32 upperMask asm("r10");
    register u32 xy asm("r5");
    s32 x;
    register s32 z asm("r1");
    s32 y;
    u32 highY;
    position.bits = *source;
    table = (const u16 *)RANDOM_TABLE_ADDR;
    state = &gUnk_03005308;
    savedState = state;
    index = *state + 1;
    indexMask = 1023;
    index &= indexMask;
    sampleX = table[index];
    x = (u16)position.bits.x - 15;
    jitterMask = 31;
    asm("" : "+r"(jitterMask));
    x += sampleX & jitterMask;
    x = (u16)x;
    upperMask = 0xFFFF0000;
    xy = (position.words[0] & upperMask) | x;
    position.words[0] = xy;
    index = (index + 1) & indexMask;
    sampleZ = table[index];
    z = (u16)position.bits.z - 15;
    z += sampleZ & jitterMask;
    z = (u16)z;
    asm volatile("" : "+r"(upperMask) : "r"(z) : "memory");
    position.words[1] = (position.words[1] & upperMask) | (u16)z;
    index = (index + 1) & indexMask;
    *savedState = index;
    sampleY = table[index];
    y = ((s32)xy >> 16) - 16;
    sampleY &= 63;
    y -= sampleY;
    highY = (u32)y << 16;
    position.words[0] = (xy & 0xFFFF) | highY;
    AnimatedParticles_Create(1, &position, config);
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_UpdateGlow(struct TetherActor *actor)
{
    switch (actor->glowMode) {
        case 0:
            if (actor->glowFrame) {
                s32 frame = actor->glowFrame;
                actor->palette[1] = (frame * 31) >> 6;
                --actor->glowFrame;
            } else
                actor->palette[1] = 0;
            break;
        case 1: {
            s32 frame;
            if (actor->glowFrame <= 63)
                frame = actor->glowFrame;
            else {
                register s32 current asm("r0") = actor->glowFrame;
                frame = 128 - current;
            }
            actor->palette[1] = (frame * 31) >> 6;
            ++actor->glowFrame;
            if (actor->glowFrame > 127)
                actor->glowFrame = 0;
        } break;
        case 2:
            if (actor->glowFrame <= 63) {
                s32 frame = actor->glowFrame;
                actor->palette[1] = (frame * 31) >> 6;
                ++actor->glowFrame;
            } else
                actor->palette[1] = 31;
            break;
    }
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_BlendPalette(struct TetherActor *s)
{
    u16 *a = (u16 *)(gPaletteBankData + (s->paletteIndex + 0x283) * 32);
    u16 *b = (u16 *)(gPaletteBankData + 0x5160);
    s32 inv = 64 - s->paletteBlend;
    s32 weight = s->paletteBlend;
    s32 i = 0;
    register u32 componentMask asm("r8") = 31;
    u32 redMask;
    redMask = 31;
    do {
        u16 c = *a;
        s32 r = redMask & c;
        u32 packedC = (u32)c << 16;
        s32 g = (packedC >> 21) & componentMask;
        s32 bl = (packedC >> 26) & componentMask;
        u16 d = *b;
        s32 rr = redMask & d;
        u32 packedD = (u32)d << 16;
        s32 gg = (packedD >> 21) & componentMask;
        s32 bb = (packedD >> 26) & componentMask;
        s32 red = ((inv * r + weight * rr) >> 6) & redMask;
        s32 green = ((inv * g + weight * gg) >> 6) & redMask;
        s32 blue = ((inv * bl + weight * bb) >> 6) & redMask;
        s->palette[i] = red | (green << 5) | (blue << 10);
        a++;
        b++;
    } while (++i <= 15);
}

void TetherActor_SelectPalette(struct TetherActor *input, u32 index)
{
    struct TetherActor *actor = input;
    struct DisplayAnimation *display = &actor->display.animation;
    if (display->kind == 2)
        DisplayAnimation_SelectPaletteBank(display, 408);
    else {
        const u16 **sourceField;
        const u16 *source;
        struct DisplayAnimation *savedDisplay;
        u16 *buffer;
        u16 *destination;
        s32 remaining;
        DisplayAnimation_SelectPaletteBank(display, index + 0x283);
        sourceField = &actor->display.colors.palette;
        savedDisplay = display;
        buffer = actor->palette;
        source = *sourceField;
        destination = buffer;
        remaining = 15;
        do {
            *destination = *source;
            ++source;
            ++destination;
            --remaining;
        } while (remaining >= 0);
        DisplayAnimation_SetTileSource(savedDisplay, (u32)buffer);
    }
}

void TetherActor_UpdatePalette(struct TetherActor *actor)
{
    if (actor->kind == 9)
        return;
    if (actor->blendRequested || actor->blendHold) {
        if (actor->paletteBlend <= 63)
            ++actor->paletteBlend;
    } else if (actor->paletteBlend)
        --actor->paletteBlend;
    if (actor->paletteOverrideTimer) {
        if (actor->appliedPalette != 9) {
            if (actor->display.animation.kind == 2)
                actor->appliedPalette = 0;
            else
                actor->appliedPalette = 9;
            TetherActor_SelectPalette(actor, actor->appliedPalette);
        }
        --actor->paletteOverrideTimer;
    } else if (actor->paletteBlend) {
        actor->appliedPalette = 8;
        TetherActor_BlendPalette(actor);
    } else {
        if (actor->appliedPalette != actor->paletteIndex) {
            actor->appliedPalette = actor->paletteIndex;
            TetherActor_SelectPalette(actor, actor->appliedPalette);
        }
        TetherActor_UpdateGlow(actor);
    }
}

void TetherActor_UpdateMovingEffects(struct TetherActor *actor)
{
    if (actor->soundCooldown)
        --actor->soundCooldown;
    if (!actor->moving)
        goto stopped;
    if (actor->statusFlags)
        goto stopped;
    {
        if (actor->kind == 5) {
            if (!actor->effectFrame && !actor->soundCooldown) {
                Sound_PlaySong(0x26A);
                actor->soundCooldown = 100;
            }
        } else {
            if (!actor->effectFrame) {
                struct ContactPosition *position = &actor->position;
                s32 x = position->x >> 8;
                s32 z = position->z >> 8;
                s32 cell;
                u8 *properties;
                u32 surface;
                if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
                    cell = 0;
                else
                    cell = gUnk_030052F4->rows[z] + x;
                properties = TerrainModifiers_FindCellOverride(cell, 1);
                if (properties)
                    properties += 4;
                else {
                    struct EngineMap *map = gUnk_030052F4;
                    u32 offset = cell * 4 + 12;
                    properties = map->cells + offset;
                }
                surface = *properties >> 4;
                if (surface - 1 <= 1)
                    Sound_PlaySong(0xD3);
                else if (actor->kind == 9)
                    Sound_PlaySong(0x4D8);
                else
                    Sound_PlaySong(0xD2);
            }
            if (!(actor->effectFrame & 7))
                TetherActor_EmitDirectionalEffect(actor);
        }
        ++actor->effectFrame;
    }
    return;
stopped:
    actor->effectFrame = 0;
}

void TetherActor_UpdateEffects(struct TetherActor *actor)
{
    TetherActor_UpdatePalette(actor);
    TetherActor_UpdateMovingEffects(actor);
    TetherActor_UpdateSegments(actor);
}

void TetherActor_SetState(struct TetherActor *actor, u32 state)
{
    actor->state = state;
    actor->stateFrame = 0;
}

s32 TetherActor_SetHeadingEntry(struct TetherActor *actor, u32 entry)
{
    u32 variant, flags;
    struct DisplayAnimation *display;
    if (actor->heading > 4) {
        variant = 8 - actor->heading;
        flags = 1;
    } else {
        variant = actor->heading;
        flags = 0;
    }
    display = &actor->display.animation;
    DisplayAnimation_SetEntry(display, entry, variant, flags, 0, 0, 0);
    return DisplayAnimation_UpdatePlayback(display);
}

s32 TetherActor_SetCardinalEntry(struct TetherActor *actor, u32 unusedEntry)
{
    s32 angle = (u8)((actor->heading + 5) << 5);
    u8 flags[2];
    struct DisplayAnimation *display;
    u32 orientation;
    orientation = ((angle + 96) >> 6) & 3;
    flags[0] = orientation;
    {
        u8 *packet = flags;
        u8 *saved = packet;
        u8 *first;
        u8 *second;
        u8 *savedSecond;
        u32 sector;
        u8 test;
        sector = *saved;
        first = saved;
        second = packet + 1;
        test = sector - 1;
        savedSecond = second;
        if (test <= 1)
            *first = 1;
        else
            *first = 0;
        if (sector > 1)
            *second = 1;
        else
            *second = 0;
        display = &actor->display.animation;
        DisplayAnimation_SetEntry(display, 44, flags[0], *savedSecond, 0, 0, 0);
    }
    return DisplayAnimation_UpdatePlayback(display);
}

// MATCHING_CONCESSION: register-pinning

s32 TetherActor_SetEntry(struct TetherActor *input, u32 inputBase)
{
    struct TetherActor *actor = input;
    u32 base = inputBase;
    register u32 kind asm("r0") = actor->kind;
    s32 result;
    if (kind != 9) {
        register u32 entry asm("r1") = kind;
        asm("" : "+r"(entry));
        entry = base + entry;
        result = TetherActor_SetHeadingEntry(actor, entry);
    } else
        result = TetherActor_SetCardinalEntry(actor, base);
    return result;
}

void TetherActor_ApplyPendingEffect(struct TetherActor *actor)
{
    if (actor->chargeReady) {
        actor->chargeReady = 0;
        actor->charge = 0;
        actor->pendingEffectHandle = ActorAttachment_UpdateFromDescriptor(
            actor->pendingEffectHandle, &actor->bodyConfig, 127, 0, 0, 0, 250, 290);
    }
}

void TetherActor_UpdatePendingEffect(struct TetherActor *actor)
{
    TetherActor_ApplyPendingEffect(actor);
}

u32 TetherActor_TestInteractionPath(struct TetherActor *p)
{
    u8 *a = (u8 *)&p->position;
    u8 *b;
    u32 r;
    if (Navigation_FindContainingRegion(a) == 0xFF ||
        (b = (u8 *)&p->interactionProbe, ProximityVolume_TranslateBounds(b, a),
         sub_0821F2EC(b, a, (u8 *)&p->interactionTarget) == 0))
        r = 0;
    else
        r = 1;
    return r;
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_SelectInteractionState(struct TetherActor *actor)
{
    actor->currentValue = actor->maximumValue;
    asm("" ::: "r3");
    if (!actor->interactionEnabled || actor->statusFlags)
        TetherActor_SetState(gUnk_02000488, 0);
    else if (TetherActor_TestInteractionPath(actor))
        TetherActor_SetState(gUnk_02000488, 3);
    else
        TetherActor_SetState(gUnk_02000488, 4);
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_BeginAuxiliaryState(struct TetherActor *input)
{
    struct TetherActor *actor = input;
    struct ContactPosition *position;
    s32 x, z;
    s32 cell;
    u8 *properties;
    u32 surface, height;
    actor->auxiliaryOrigin = actor->position;
    actor->auxiliaryOrigin.y += 384;
    TetherActor_CreateAuxiliarySprite(actor);
    actor->auxiliarySprite.position = actor->position;
    position = &actor->auxiliarySprite.position;
    x = position->x >> 8;
    z = position->z >> 8;
    do {
        if ((((x < 0) || (z < 0)) || (((u32)x) >= gUnk_030052F8)) || (((u32)z) >= gUnk_030052FC)) {
            cell = 0;
        } else {
            u32 row = gUnk_030052F4->rows[z];
            cell = row + x;
        }
    } while (0);
    properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties) {
        properties += 4;
    } else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = cell * 4 + 12;
        properties = map->cells + offset;
    }
    surface = *properties >> 4;
    height = (*properties & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    actor->auxiliarySprite.position.y = height;
    if (actor->auxiliaryRegistered) {
        SceneActorContacts_RemoveNode(actor->auxiliaryRegistration);
        actor->auxiliaryRegistered = 0;
    }
    TetherActor_UpdatePendingEffect(actor);
    TetherActor_SetState(actor, 5);
}

// fix.py best (sweep after the inlined-insn fix)

extern u16 gUnk_02000550, gUnk_0200052C, gUnk_0200053C;

static inline void slopePush(struct TetherActor *actor, s32 octant)
{
    s32 angle = (octant + 1) << 5;
    s32 cosine = gUnk_086149C4[(angle + 64) & 255];
    s32 sine = gUnk_086149C4[angle & 255];
    actor->movementDelta.x -= scaleOffset(gUnk_02000550 * cosine);
    actor->movementDelta.z -= scaleOffset(gUnk_02000550 * sine);
}

static inline u32 NeighborHeight(s32 cell)
{
    u32 value = cellByte(cell);
    u32 mask = 15;
    mask &= value;
    return mask;
}
inline static void slopePush_AdjustSlopeMovement(struct TetherActor *actor, s32 octant)
{
    s32 angle = octant;
    register const s16 *table asm("r2") = gUnk_086149C4;
    s32 cosine;
    s32 sine;
    cosine = table[(angle + 64) & 255];
    sine = table[angle & 255];
    do {
        actor->movementDelta.x -= scaleOffset(gUnk_02000550 * cosine);
    } while (0);
    actor->movementDelta.z -= scaleOffset(gUnk_02000550 * sine);
}

void TetherActor_AdjustSlopeMovement(struct TetherActor *actor)
{
    s32 neighbor;
    s32 octant;
    if (((actor->kind != 9) || (actor->state == 11)) || (gUnk_02000550 < gUnk_0200052C))
        return;
    if (!actor->movementProbe) {
        void *probe = &actor->movementProbeStorage;
        CellObject_Init(probe, &actor->position);
        actor->movementProbe = probe;
    }
    octant = ((gUnk_0200053C + 112) & 255) >> 5;
    neighbor = 0;
    if (octant & 1) {
        if (octant == 1)
            neighbor = ((s16 *)gUnk_030052F4->cells)[2];
        else if (octant == 3)
            neighbor = -1;
        else if (octant == 5)
            neighbor = -((s16 *)gUnk_030052F4->cells)[2];
        else if (octant == 7)
            neighbor = 1;
        {
            u32 h;
            u32 value = cellByte(actor->movementProbeStorage.cell);
            h = 15;
            h &= value;
            if (h < NeighborHeight(actor->movementProbeStorage.cell + neighbor))
                return;
        }
        octant = (octant + 1) << 5;
        slopePush_AdjustSlopeMovement(actor, octant);
    } else {
        s32 x;
        s32 z;
        if (octant == 0)
            neighbor = ((s16 *)gUnk_030052F4->cells)[2] + 1;
        else if (octant == 2)
            neighbor = ((s16 *)gUnk_030052F4->cells)[2] - 1;
        else if (octant == 4)
            neighbor = ~((s16 *)gUnk_030052F4->cells)[2];
        else if (octant == 6)
            neighbor = (-((s16 *)gUnk_030052F4->cells)[2]) + 1;
        {
            u32 h;
            u32 value = cellByte(actor->movementProbeStorage.cell);
            h = 15;
            h &= value;
            if (h < NeighborHeight(actor->movementProbeStorage.cell + neighbor))
                return;
        }
        x = (u8)actor->position.x;
        z = (u8)actor->position.z;
        if (octant == 0) {
            if (x > z)
                neighbor = 1;
            else
                neighbor = ((s16 *)gUnk_030052F4->cells)[2];
        } else if (octant == 2) {
            if ((x + z) > 256)
                neighbor = ((s16 *)gUnk_030052F4->cells)[2];
            else
                neighbor = -1;
        } else if (octant == 4) {
            if (x > z)
                neighbor = -((s16 *)gUnk_030052F4->cells)[2];
            else
                neighbor = -1;
        } else if (octant == 6) {
            if ((x + z) > 256)
                neighbor = 1;
            else
                neighbor = -((s16 *)gUnk_030052F4->cells)[2];
        }
        {
            u32 h;
            u32 value = cellByte(actor->movementProbeStorage.cell);
            h = 15;
            h &= value;
            if (h < NeighborHeight(actor->movementProbeStorage.cell + neighbor))
                return;
        }
        octant = (octant + 1) << 5;
        slopePush_AdjustSlopeMovement(actor, octant);
    }
}

/* TetherActor_ClearInstance: Clears the global tether actor pointer without destroying the actor */

void TetherActor_ClearInstance(void)
{
    gUnk_02000488 = 0;
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_ClearExternalStatus(void)
{
    Inventory_AddItem(43, 0);
    {
        s16 *status = &gUnk_02000710->unk5AA;
        register s32 value asm("r2");
        register s32 half asm("r1");
        value = -1;
        asm("" : "+r"(value));
        half = value;
        *status = half;
    }
}

s32 TetherActor_TestMatchingStatus(u32 a, u32 b, u32 c, u32 d)
{
    s32 r;
    if (b & 4) {
        if (d & 4) {
            if (a == c)
                goto yes;
        }
    no:
        r = 0;
        goto done;
    } else {
        if (d & 4)
            goto no;
    }
yes:
    r = 1;
done:
    return r;
}

#define NEAR_CELL(out, px, pz)                                                                     \
    do {                                                                                           \
        s32 cx_ = (px) >> 8;                                                                       \
        s32 cz_ = (pz) >> 8;                                                                       \
        if (cx_ < 0 || cz_ < 0 || (u32)cx_ >= gUnk_030052F8 || (u32)cz_ >= gUnk_030052FC)          \
            out = 0;                                                                               \
        else                                                                                       \
            out = gUnk_030052F4->rows[cz_] + cx_;                                                  \
    } while (0)

#define NEAR_HEIGHT(out, pos, index)                                                               \
    do {                                                                                           \
        u8 *properties_ = TerrainModifiers_FindCellOverride(index, 1);                             \
        u32 surface_;                                                                              \
        if (properties_)                                                                           \
            properties_ += 4;                                                                      \
        else {                                                                                     \
            struct EngineMap *map_ = gUnk_030052F4;                                                \
            u32 offset_ = index * 4 + 12;                                                          \
            properties_ = map_->cells + offset_;                                                   \
        }                                                                                          \
        surface_ = *properties_ >> 4;                                                              \
        out = (*properties_ & 15) << 8;                                                            \
        switch (surface_) {                                                                        \
            case 1:                                                                                \
                out -= (u8)(pos)->z;                                                               \
                break;                                                                             \
            case 2:                                                                                \
                out -= (u8)(pos)->x;                                                               \
                break;                                                                             \
        }                                                                                          \
    } while (0)

u32 TetherActor_TestNearbyInteraction(struct ContactPosition *position, u32 tag, u32 flags)
{
    s32 dx;
    s32 dz;
    s32 own;
    s32 other = 0;
    struct EngineMap *map;
    s32 height;
    s32 diff;
    u32 state;
    union {
        struct ContactPackedVector v;
        struct ContactPosition p;
    } corner;
    if (!gUnk_02000488)
        goto no;
    dx = position->x - gUnk_02000488->position.x;
    dz = position->z - gUnk_02000488->position.z;
    if (dx * dx + dz * dz > 48400)
        goto no;
    state = gUnk_02000488->state;
    if ((u8)(state - 16) <= 1)
        goto yes;
    if ((u8)state != 0 && (u8)state != 3 && (u8)state != 4)
        goto no;
    if (!TetherActor_TestMatchingStatus(tag, flags, gUnk_02000488->statusTag,
                                        gUnk_02000488->statusFlags))
        goto no;
    NEAR_CELL(own, position->x, position->z);
    {
        struct ContactPosition *p = &gUnk_02000488->position;
        s32 cx = p->x >> 8;
        s32 cz = p->z >> 8;
        if (cx < 0 || cz < 0 || (u32)cx >= gUnk_030052F8 || (u32)cz >= gUnk_030052FC)
            other = 0;
        else
            other = gUnk_030052F4->rows[cz] + cx;
    }
    if (own == other)
        goto yes;
    if (own == other + 1 || own == other - 1 ||
        own == other + ((s16 *)(map = gUnk_030052F4)->cells)[2] ||
        own == other - ((s16 *)map->cells)[2]) {
        diff = position->y - gUnk_02000488->position.y;
        if (diff < 0)
            diff = -diff;
        if (diff > 255)
            goto no;
    yes:
        return 1;
    }
    diff = position->y - gUnk_02000488->position.y;
    if (diff < 0)
        diff = -diff;
    if (diff > 255)
        goto no;
    corner.v.x = position->x - dx;
    corner.v.z = position->z;
    {
        struct ContactPackedVector *cp = &corner.v;
        s32 cx = cp->x >> 8;
        s32 cz = cp->z >> 8;
        if (cx < 0 || cz < 0 || (u32)cx >= gUnk_030052F8 || (u32)cz >= gUnk_030052FC)
            own = 0;
        else
            own = map->rows[cz] + cx;
        NEAR_HEIGHT(height, cp, own);
    }
    diff = position->y - height;
    if (diff < 0)
        diff = -diff;
    if (diff <= 127)
        goto yes;
    corner.p.x = position->x;
    corner.p.z = position->z - dz;
    {
        struct ContactPosition *cp = &corner.p;
        NEAR_CELL(own, cp->x, cp->z);
        NEAR_HEIGHT(height, cp, own);
    }
    diff = position->y - height;
    if (diff < 0)
        diff = -diff;
    if (diff <= 127)
        goto yes;
no:
    return 0;
}

void TetherActor_SetInteractionTarget(struct ContactPosition *a, u32 b)
{
    struct TetherActor *p = gUnk_02000488;
    if (p) {
        p->targetPosition = *a;
        p->targetPosition.pad = b;
    }
}

struct ContactPosition *TetherActor_GetPosition(void)
{
    struct TetherActor *p = gUnk_02000488;
    if (!p)
        return 0;
    return &p->position;
}

u32 TetherActor_CopySampledPosition(struct ContactPosition *a)
{
    struct TetherActor *p = gUnk_02000488;
    u32 r;
    if (p) {
        *a = p->sampledPosition;
        r = 1;
    } else {
        ((u16 *)a)[0] = 0;
        ((u16 *)a)[1] = 0;
        ((u16 *)a)[2] = 0;
        r = 0;
    }
    return r;
}

u32 TetherActor_IsState(u32 a)
{
    struct TetherActor *p = gUnk_02000488;
    if (!p)
        return 0;
    if (p->state == a)
        return 1;
    return 0;
}

void TetherActor_SetInteractionPathFromScript(void)
{
    struct TetherActor *p = gUnk_02000488;
    if (p) {
        void *a;
        void *b;
        if (Script_SeekToKeyword(0x70)) {
            p->interactionTarget.x = Script_GetValue();
            p->interactionTarget.y = Script_GetValue();
            p->interactionTarget.z = Script_GetValue();
        }
        if (Script_SeekToKeyword(0x52))
            p->interactionScriptId = Script_GetValue();
        if (p->state == 3) {
            a = &p->position.x;
            if (Navigation_FindContainingRegion(a) == 0xFF ||
                (b = &p->interactionProbe, ProximityVolume_TranslateBounds(b, a),
                 sub_0821F2EC(b, a, &p->interactionTarget.x) == 0))
                TetherActor_SetState(p, 0);
            else
                TetherActor_SetState(p, 3);
        }
    }
}

void TetherActor_ResetInteraction(void)
{
    if (gUnk_02000488) {
        TetherActor_SetState(gUnk_02000488, 0);
        gUnk_02000488->interactionReset = 1;
    }
}

void TetherActor_RequestPaletteBlend(u32 a)
{
    struct TetherActor *p = gUnk_02000488;
    if (p)
        p->blendRequested = a;
}

// MATCHING_CONCESSION: register-pinning

u32 TetherActor_CanStartInteraction(void)
{
    struct TetherActor *actor = gUnk_02000488;
    u32 result;
    if (!actor)
        goto no;
    if (actor->interactionLock)
        goto no;
    asm("" ::: "r2");
    if (actor->blendRequested)
        goto no;
    asm("" ::: "r2");
    if (actor->statusFlags)
        goto no;
    asm("" ::: "r2");
    if (actor->interactionCooldown)
        goto no;
    if (actor->state <= 1 || ((u8)(actor->state - 3) <= 1 && actor->chargeReady))
        result = 1;
    else {
    no:
        result = 0;
    }
    return result;
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_StartInteractionEffect(void)
{
    struct TetherActor *actor = gUnk_02000488;
    u32 state;
    u32 savedState;
    register u32 zero asm("r3");
    u16 *cooldown;
    register u32 delay asm("r0");
    if (!actor)
        goto done;
    state = actor->state;
    savedState = state;
    asm("" : "+r"(savedState));
    if (savedState == 1) {
        TetherActor_UpdatePendingEffect(actor);
        {
            u16 *lock = &actor->interactionLock;
            zero = 0;
            *lock = savedState;
        }
        cooldown = &actor->interactionCooldown;
        delay = 300;
    } else {
        if (savedState != 0 && savedState != 2 && ((u8)(state - 3) > 1 || !actor->chargeReady))
            goto done;
        TetherActor_UpdatePendingEffect(actor);
        TetherActor_SelectInteractionState(actor);
        cooldown = &actor->interactionCooldown;
        zero = 0;
        delay = 600;
    }
    *cooldown = delay;
    actor->effectValue = zero;
    actor->pendingEffectHandle =
        ActorAttachment_UpdateFromDescriptor(actor->pendingEffectHandle, &actor->bodyConfig, 1,
                                             zero, &actor->effectValue, 200, 250, 330);
done:;
}

u32 TetherActor_IsStateOne(void)
{
    struct TetherActor *p = gUnk_02000488;
    if (!p)
        return 0;
    if (p->state == 1)
        return 1;
    return 0;
}

s32 TetherActor_GetParameter(void)
{
    struct TetherActor *p = gUnk_02000488;
    s32 r;
    if (p)
        r = p->parameter;
    else
        r = -1;
    return r;
}

void TetherActor_SetParameter(u32 a)
{
    struct TetherActor *p = gUnk_02000488;
    if (p)
        p->parameter = a;
}

void TetherActor_BeginDetachedState(u32 a)
{
    struct TetherActor *p = gUnk_02000488;
    if (p) {
        u8 *q;
        if (a)
            TetherActor_SetState(p, 7);
        else
            TetherActor_SetState(p, 6);
        q = &p->auxiliaryRegistered;
        if (*q) {
            SceneActorContacts_RemoveNode(&p->auxiliaryRegistration);
            *q = 0;
        }
        p->displayOffsetY = 0;
        TetherActor_UpdatePendingEffect(p);
    }
}

void TetherActor_BeginDisplayPositionState(void)
{
    struct TetherActor *g = gUnk_02000488;
    if (g != 0) {
        struct ContactPosition *d;
        TetherActor_SetState(g, 8);
        TetherActor_CreateAuxiliarySprite(g);
        d = (struct ContactPosition *)(&g->auxiliarySprite.position);
        *d = *(struct ContactPosition *)g->display.animation.position;
        g->displayOffsetY = 0x5dc;
        TetherActor_UpdatePendingEffect(g);
    }
}

u32 TetherActor_GetStatusFlags(void)
{
    struct TetherActor *p = gUnk_02000488;
    u32 r;
    if (p)
        r = p->statusFlags;
    else
        r = 0;
    return r;
}

u32 TetherActor_TestStatusFlags(u32 m)
{
    struct TetherActor *p = gUnk_02000488;
    if (!p)
        return 0;
    if (p->statusFlags & m)
        return 1;
    return 0;
}

/* TetherActor_GetStatusTag: Returns the actor status tag at offset 0x3E6 or zero when no actor exists */

u16 TetherActor_GetStatusTag(void)
{
    if (gUnk_02000488 == NULL)
        return 0;
    return gUnk_02000488->statusTag;
}

void TetherActor_AddStatusFlags(u32 a, u32 b)
{
    struct TetherActor *p = gUnk_02000488;
    if (p) {
        p->statusFlags |= a;
        gUnk_02000488->statusTag = b;
    }
}

void TetherActor_RemoveStatusFlags(u32 m)
{
    struct TetherActor *g = gUnk_02000488;
    if (g != 0) {
        u8 *q = &g->statusFlags;
        u32 z;
        u8 v = *q & ~m;
        z = 0;
        *q = v;
        gUnk_02000488->statusTag = z;
    }
}

u32 TetherActor_CopyPosition(struct ContactPosition *a)
{
    struct TetherActor *p = gUnk_02000488;
    u32 r;
    if (p) {
        *a = p->position;
        r = 1;
    } else
        r = 0;
    return r;
}

void TetherActor_SetPosition(struct ContactPosition *a)
{
    struct TetherActor *p = gUnk_02000488;
    if (p) {
        /* Engine display positions expose two words; coordinates use the aligned view. */
        struct ContactPosition **pp;
        p->savedPosition = *a;
        p->position = *a;
        pp = (struct ContactPosition **)&p->display.animation.position;
        **pp = *a;
        (*pp)->x += p->shakeX;
        (*pp)->z += p->shakeZ;
        p->targetPosition = *a;
    }
}

u32 TetherActor_BeginKindFourStateNine(void)
{
    struct TetherActor *p = gUnk_02000488;
    if (!p)
        return 0;
    if (p->kind != 4)
        return 0;
    if (p->state > 4)
        return 0;
    TetherActor_SetState(p, 9);
    return 1;
}

u32 TetherActor_BeginKindFourStateTen(void)
{
    struct TetherActor *p = gUnk_02000488;
    if (!p)
        return 0;
    if (p->kind != 4)
        return 0;
    TetherActor_SetState(p, 10);
    return 1;
}

u32 TetherActor_RequestKindNineAction(u32 a, u32 b)
{
    if (gUnk_02000488) {
        if (!ScenePanel_IsPresent(gUnk_02000488)) {
            if (gUnk_02000488->kind == 9) {
                gUnk_02000488->actionRequested = 1;
                gUnk_02000488->actionAngle = b;
                gUnk_02000488->actionMagnitude = a;
                return 1;
            }
        }
    }
    return 0;
}

u8 TetherActor_GetRequestStatus(void)
{
    struct TetherActor *p = gUnk_02000488;
    u8 r;
    if (p)
        r = p->requestStatus;
    else
        r = 0;
    return r;
}

u8 TetherActor_GetRequestMode(void)
{
    struct TetherActor *p = gUnk_02000488;
    u8 r;
    if (p)
        r = p->requestMode;
    else
        r = 0;
    return r;
}

// MATCHING_CONCESSION: register-pinning

/* Keep the existing pinned offset calculations: direct member addresses change allocation. */

void TetherActor_BeginDirectedState(struct ContactPosition *position)
{
    struct TetherActor *actor = gUnk_02000488;
    if (actor) {
        struct Player *player;
        struct TetherActor *savedActor;
        s32 dx;
        s32 dz;
        actor->directedTarget = *position;
        TetherActor_SetState(actor, 15);
        savedActor = gUnk_02000488;
        savedActor->directedStep = 16;
        player = gUnk_02000580[0];
        {
            s32 playerX = player->position.x;
            struct ContactPosition *targetPosition = &savedActor->directedTarget;
            s32 actorX = targetPosition->x;
            dx = playerX - actorX;
        }
        dz = player->position.z - savedActor->directedTarget.z;
        if (dx || dz) {
            register s32 angle asm("r0") = Math_Atan2(dx, dz);
            struct TetherActor *target;
            angle = (angle + 16) & 255;
            asm("" : "+r"(angle));
            angle >>= 5;
            target = gUnk_02000488;
            angle = (angle + 7) & 7;
            {
                u32 offset = 0x46C;
                target->directedHeading = angle;
            }
        } else {
            u32 index = (gUnk_03005308 + 1) & 1023;
            gUnk_03005308 = index;
            {
                u32 byteOffset = index << 1;
                const u16 *table;
                register u32 sample asm("r0");
                register u16 *heading asm("r1");
                table = (const u16 *)RANDOM_TABLE_ADDR;
                sample = *(const u16 *)(byteOffset + (u32)table);
                sample &= 7;
                {
                    register u32 offset asm("r2") = 0x46C;
                    asm("" : "+r"(offset));
                    heading = (u16 *)((u8 *)savedActor + offset);
                }
                *heading = sample;
            }
        }
        {
            register u8 *target asm("r0") = (u8 *)gUnk_02000488;
            register u32 offset asm("r3");
            offset = 0x46A;
            asm("" : "+r"(offset));
            *(u16 *)((u8 *)target + offset) = 29;
        }
    }
}

void TetherActor_WriteTransformToScript(void)
{
    struct TetherActor *p = gUnk_02000488;
    u32 buf[2];
    if (p) {
        Script_ReadReference(buf);
        Script_WriteReferenceValue(buf, 0, p->position.x);
        Script_ReadReference(buf);
        Script_WriteReferenceValue(buf, 0, p->position.y);
        Script_ReadReference(buf);
        Script_WriteReferenceValue(buf, 0, p->position.z);
        Script_ReadReference(buf);
        Script_WriteReferenceValue(buf, 0, p->heading);
    }
}

void TetherActor_SetTransformFromScript(void)
{
    struct TetherActor *actor = gUnk_02000488;
    if (actor) {
        if (Script_SeekToKeyword('p')) {
            struct ContactPosition *position;
            s32 x, z, cell;
            u8 *properties;
            u32 surface, height;
            actor->savedPosition.x = Script_GetValue();
            actor->savedPosition.y = Script_GetValue();
            actor->savedPosition.z = Script_GetValue();
            position = &actor->savedPosition;
            x = position->x >> 8;
            z = position->z >> 8;
            if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
                cell = 0;
            else {
                u32 row = gUnk_030052F4->rows[z];
                cell = row + x;
            }
            properties = TerrainModifiers_FindCellOverride(cell, 1);
            if (properties)
                properties += 4;
            else {
                struct EngineMap *map = gUnk_030052F4;
                u32 offset = cell * 4 + 12;
                properties = map->cells + offset;
            }
            surface = *properties >> 4;
            height = (*properties & 15) << 8;
            switch (surface) {
                case 1:
                    height -= (u8)position->z;
                    break;
                case 2:
                    height -= (u8)position->x;
                    break;
            }
            actor->savedPosition.y = height;

            actor->position = actor->savedPosition;
            *(struct ContactPosition *)actor->display.animation.position = actor->savedPosition;
            actor->origin = actor->savedPosition;
        }
        if (Script_SeekToKeyword('d'))
            actor->heading = Script_GetValue();
    }
}

void TetherActor_RequestPositionSample(void)
{
    struct TetherActor *p = gUnk_02000488;
    if (p)
        p->destroyRequested = 1;
}

void TetherActor_StartAuxiliaryState(void)
{
    struct TetherActor *p = gUnk_02000488;
    if (p) {
        if (p->state != 5) {
            if (p->statusFlags == 0)
                TetherActor_BeginAuxiliaryState(p);
        }
    }
}

/* TetherActor_GetSavedKind: Returns the saved kind at GameState offset 0x5AA as a signed halfword */

s16 TetherActor_GetSavedKind(void)
{
    return gUnk_02000710->unk5AA;
}

s32 TetherActor_GetConfiguredKind(void)
{
    return gUnk_02000710->unk5AA;
}

extern const u16 gUnk_086059F0[];

// MATCHING_CONCESSION: register-pinning

void TetherActor_InitKindFromScript(struct TetherActor *actor, u32 entry)
{
    register s32 kind asm("r0");
    s32 value;
    actor->entry = entry;
    if (Script_SeekToKeyword('k'))
        kind = Script_GetValue();
    else
        kind = TetherActor_GetConfiguredKind();
    gUnk_02000710->unk5AA = kind;
    asm("" ::: "memory");
    actor->kind = gUnk_02000710->unk5AA;
    value = Script_SeekToKeyword('c');
    if (value)
        value = Script_GetValue();
    actor->kindValue = value;
    actor->kindLimit = gUnk_086059F0[actor->kind];
    if (Script_SeekToKeyword('a'))
        TetherActor_SetState(actor, Script_GetValue());
    else
        TetherActor_SetState(actor, 0);
    if (Script_SeekToKeyword('A')) {
        actor->valueA = Script_GetValue();
        actor->valueB = Script_GetValue();
    } else {
        actor->valueA = 50;
        actor->valueB = 50;
    }
}

extern const u8 gUnk_08605A04[];

// MATCHING_CONCESSION: register-pinning

void TetherActor_InitInteractionFromScript(struct TetherActor *actor)
{
    s32 value = Script_SeekToKeyword('r');
    register u32 capacity asm("r0");
    if (value) {
        actor->interactionTarget.x = Script_GetValue();
        actor->interactionTarget.y = Script_GetValue();
        actor->interactionTarget.z = Script_GetValue();
    } else {
        actor->interactionTarget.x = value;
        actor->interactionTarget.y = value;
        actor->interactionTarget.z = value;
    }
    value = Script_SeekToKeyword('O');
    if (value)
        actor->interactionScriptId = Script_GetValue();
    else
        actor->interactionScriptId = value;
    capacity = gUnk_08605A04[actor->kind] * 120;
    actor->maximumValue = capacity;
    actor->currentValue = capacity;
    if (actor->kind == 2 || actor->kind == 9) {
        actor->interactionEnabled = 0;
        if ((u8)(actor->state - 3) <= 1)
            actor->state = 0;
    } else {
        actor->interactionEnabled = 1;
        if ((u8)(actor->state - 3) <= 1)
            TetherActor_SelectInteractionState(actor);
    }
    capacity = gUnk_08605A04[actor->kind] * 60;
    actor->secondaryMaximum = capacity;
    actor->secondaryValue = capacity;
    actor->recoveryDuration = 300;
    actor->recoveryTimer = 0;
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_InitDisplayFromScript(struct TetherActor *actor)
{
    struct {
        u32 zero;
        struct DisplayPosition position;
    } packet;
    struct DisplayAnimation *display;
    u32 zero;
    struct DisplayPosition *initialPosition;
    u16 *paletteIndex = &actor->paletteIndex;
    u32 value;
    zero = 0;
    *paletteIndex = 10;
    initialPosition = &packet.position;
    packet.zero = zero;
    {
        register u32 *fill asm("r0") = &packet.zero;
        register struct DisplayPosition *destination asm("r1");
        destination = initialPosition;
        CpuSet(fill, destination, 0x5000002);
    }
    if (actor->kind == 9) {
        display = &actor->display.animation;
        DisplayAnimation_InitializeKeyframes(display, 0x35E3, 0, initialPosition);
    } else {
        const u16 *palette;
        u16 *buffer;
        const u16 *source;
        u16 *destination;
        s32 i;
        display = &actor->display.animation;
        asm("" : "+r"(display));
        DisplayAnimation_InitializeSequence(display, 0x2499, 0x3D95, 0, initialPosition);
        TetherActor_SetEntry(actor, 0);
        actor->appliedPalette = *paletteIndex;
        DisplayAnimation_SelectPaletteBank(display, actor->appliedPalette + 0x283);
        if (display->kind == 2)
            palette = actor->display.colors.keyframePalette;
        else
            palette = actor->display.colors.palette;
        buffer = actor->palette;
        source = palette;
        destination = buffer;
        i = 15;
        do {
            *destination++ = *source++;
        } while (--i >= 0);
        DisplayAnimation_SetTileSource(display, (u32)buffer);
    }
    DisplayAnimation_SetPriority(display, 2);
    value = Script_SeekToKeyword('d');
    if (value)
        actor->heading = Script_GetValue();
    else
        actor->heading = value;
    actor->shakeX = 0;
    actor->displayOffsetY = 0;
    actor->shakeZ = 0;
}

void TetherActor_InitBodyFromScript(struct TetherActor *actor)
{
    s32 found = Script_SeekToKeyword('p');
    struct ContactPosition *position;
    s32 x, z, cell;
    u8 *properties;
    u32 surface, height;
    struct TetherBodyConfig *initialConfig;
    struct TetherBodyConfig *config;
    u32 displayKind;
    if (found) {
        actor->savedPosition.x = Script_GetValue();
        actor->savedPosition.y = Script_GetValue();
        actor->savedPosition.z = Script_GetValue();
    } else {
        actor->savedPosition.x = found;
        actor->savedPosition.y = found;
        actor->savedPosition.z = found;
    }
    position = &actor->savedPosition;
    x = position->x >> 8;
    z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        cell = 0;
    else {
        u32 row = gUnk_030052F4->rows[z];
        cell = row + x;
    }
    properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = cell * 4 + 12;
        properties = map->cells + offset;
    }
    surface = *properties >> 4;
    height = (*properties & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    actor->savedPosition.y = height;

    *(struct ContactPosition *)actor->display.animation.position = actor->savedPosition;
    actor->origin = actor->savedPosition;
    actor->targetPosition = actor->savedPosition;
    initialConfig = &actor->bodyConfig;
    SceneBody_Init(initialConfig, actor->entry, &actor->savedPosition, 0, 6, actor);
    displayKind = actor->display.animation.kind;
    config = initialConfig;
    if (displayKind == 0)
        goto displayDone;
    if (displayKind == 1)
        SceneBody_SetSprite(config, (u8 *)&actor->display.animation.resource + 0x1C);
    else if (displayKind == 2)
        SceneBody_SetKeyframeDisplay(config, (u8 *)&actor->display.animation.resource + 0x20);
displayDone:
    actor->bodyConfig.angle = ((actor->heading + 5) & 7) << 5;
    actor->interactionProbe.bodyId = actor->bodyConfig.id;
    if (actor->kind != 3) {
        if (!actor->kindValue)
            SceneActorContacts_RegisterNode(actor->auxiliaryRegistration, config, 1);
        else
            SceneActorContacts_RegisterNode(actor->auxiliaryRegistration, config, 4);
        actor->auxiliaryRegistered = 1;
    }
    if (actor->kind == 9) {
        CellObject_Init(&actor->movementProbeStorage, &actor->position);
        actor->movementProbe = &actor->movementProbeStorage;
    }
    TerrainOverlay_QueueAddActor(config);
}

struct ContactCollider;

void TetherActor_InitChargeCollider(struct TetherActor *p)
{
    struct TetherColliderVector s1, s2;
    struct ContactCollider *q = &p->chargeCollider;
    u32 t;
    s1.a.lo = 0x5a;
    s1.a.hi = 0x20;
    s1.b.lo = 0x5a;
    s2.a.lo = 0x64;
    s2.a.hi = 0xc8;
    s2.b.lo = 0x64;
    if (p->kind == 3)
        t = 0x10;
    else if (p->kind == 9)
        t = 1;
    else if (p->kindValue == 0)
        t = 0x10;
    else
        t = 2;
    Collider_Init(q, p->entry, 0x4081, 0, t, &s1, &s2);
    Collider_SetBodyProperties(q, 0, 0, 0);
    Collider_SetCallback(q, TetherActor_OnChargeContact, p);
    Collider_SetPosition(q, &p->position, 0);
    Collider_Refresh(q);
}

void TetherActor_InitSecondaryCollider(struct TetherActor *p)
{
    struct TetherColliderVector s1, s2;
    struct ContactCollider *q = &p->secondaryCollider;
    s1.a.lo = 0x80;
    s1.a.hi = 0x20;
    s1.b.lo = 0x80;
    *(u32 *)&s2.a = 0x200000;
    *(u32 *)&s2.b &= 0xFFFF0000;
    Collider_Init(q, p->entry, 0x2001, 0, 0x10, &s1, &s2);
    Collider_SetAttackProperties(q, p->valueA, p->valueB, 0, 0x40000, 0x1e);
    Collider_SetCallback(q, 0, p);
}

void TetherActor_InitStateParameters(struct TetherActor *p)
{
    u16 zero;
    u16 two;
    u16 *first;
    first = &p->stateOption;
    zero = 0;
    two = 2;
    *first = two;
    p->stateCounter = zero;
    p->stateRadius = 0x40;
    p->stateSpeed = 3;
    p->stateDuration = 6;
    p->stateRepeat = two;
    p->stateDelay = two;
    p->stateRecovery = 0x3c;
    p->stateStep = 8;
    p->statePhase = zero;
    p->stateLimitA = 0xff;
    p->stateLimitB = 0xff;
    p->stateFlagA = zero;
    p->stateLimitC = 0xff;
    p->stateFlagB = zero;
    p->stateLimitD = 0xff;
    p->stateFlagC = zero;
    p->stateLimitE = 0xff;
    p->stateFlagD = zero;
}

void TetherActor_AccumulateCharge(struct TetherActor *p, u32 a)
{
    u16 *q = &p->charge;
    u32 v = *q + a;
    *q = v;
    if (p->chargeReady == 0) {
        if ((u16)v > 0x4B0) {
            *q = 0x4B0;
            if (p->interactionLock == 0) {
                if (p->state != 2) {
                    p->chargeReady = 1;
                    p->pendingEffectHandle = ActorAttachment_UpdateFromDescriptor(
                        p->pendingEffectHandle, &p->bodyConfig, 1, 0, (u32)q, 0x4B0, 0xFA, 0x122);
                }
            }
        }
    } else {
        if ((u16)v > 0x4B0)
            *q = 0x4B0;
    }
}

void TetherActor_UpdateChargeDecay(struct TetherActor *p)
{
    u16 *q = &p->charge;
    u32 z;
    if (*q != 0) {
        u32 v = *q - 1;
        z = 0;
        *q = v;
        if ((u16)v == 1) {
            p->chargeReady = z;
            *q = z;
            p->pendingEffectHandle = ActorAttachment_UpdateFromDescriptor(
                p->pendingEffectHandle, &p->bodyConfig, 0x7F, z, z, z, 0xFA, 0x122);
        }
    }
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_UpdateInteractionCooldown(struct TetherActor *actor)
{
    u16 *field = &actor->interactionCooldown;
    u32 timer = *field;
    register u32 saved asm("r3") = timer;
    register u32 result asm("r0");
    if (!saved)
        goto done;
    if (actor->state != 1)
        goto other;
    if (saved <= 2)
        goto zero;
    result = timer - 2;
    goto store;
zero:
    result = 0;
    goto store;
other:
    result = timer - 1;
store:
    *field = result;
done:;
}

void TetherActor_UpdateChargeAndCooldown(struct TetherActor *actor)
{
    if (!(gUnk_030053F4 & 512)) {
        TetherActor_UpdateChargeDecay(actor);
        TetherActor_UpdateInteractionCooldown(actor);
        if (actor->contactCooldown) {
            actor->chargeCollider.flags = 4 | actor->chargeCollider.flags;
            --actor->contactCooldown;
        } else
            actor->chargeCollider.flags = 0xFFFB & actor->chargeCollider.flags;
    } else
        actor->pendingEffectHandle = ActorAttachment_UpdateFromDescriptor(
            actor->pendingEffectHandle, &actor->bodyConfig, 127, 0, 0, 0, 250, 290);
}

void TetherActor_UpdateSecondaryColliderValues(struct TetherActor *p)
{
    struct ContactCollider *q = &p->secondaryCollider;
    struct GameState *g = gUnk_02000710;
    s32 v = (s16)g->savedZ;
    p->valueA = v * 10 + 5 - (u16)g->unk876;
    Collider_SetAttackProperties(q, p->valueA, p->valueB, 0, 0x40000, 0x1E);
}

void TetherActor_OnChargeContact(struct ContactCollider *collider, void *other,
                                 struct TetherActor *inputActor)
{
    struct TetherActor *owner = inputActor;
    struct TetherActor *actor;
    u32 allowed;
    u32 responseMask;
    u32 flags;
    u32 mask = 0x800;
    u32 amount, cooldown;
    if (collider->flags & mask)
        goto done;
    actor = owner;
    if (actor->unknown1B == 1)
        goto done;
    allowed = 0;
    if (!actor->kindValue) {
        if (collider->contactFlags & 1)
            goto charge;
        if (!(collider->statRewardFlags & 512))
            goto allowGate;
        goto charge;
    } else
        allowed = 1;
allowGate:
    if (!allowed)
        goto done;
charge:
    if (actor->contactCooldown || !collider->unknown3C)
        goto done;
    responseMask = 1;
    flags = collider->statRewardFlags;
    responseMask &= flags;
    if (responseMask) {
        amount = 200;
        cooldown = 8;
    } else if (flags & 2) {
        amount = 300;
        cooldown = 12;
    } else if (flags & 4) {
        amount = 500;
        cooldown = 8;
    } else if ((flags &= 16) != 0) {
        amount = 500;
        cooldown = 8;
    } else {
        amount = 400;
        cooldown = 8;
    }
    TetherActor_AccumulateCharge(actor, amount);
    actor->paletteOverrideTimer = 4;
    actor->contactCooldown = cooldown;
done:;
}

extern const s16 gUnk_086149C4[];

void TetherActor_UpdateSampledPosition(struct TetherActor *p)
{
    u32 i;
    s32 x;
    s32 y;
    s32 t;
    p->sampledPosition = p->position;
    i = ((p->heading + 1) & 7) << 5;
    t = gUnk_086149C4[(i + 0x40) & 0xFF] << 7;
    if (t >= 0)
        x = t >> 12;
    else
        x = -(-t >> 12);
    p->sampledPosition.x += x;
    t = gUnk_086149C4[i] << 7;
    if (t >= 0)
        y = t >> 12;
    else
        y = -(-t >> 12);
    p->sampledPosition.z += y;
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_HandleFlaggedCell(struct TetherActor *actor)
{
    struct ContactPosition *position = &actor->position;
    s32 x = position->x >> 8;
    s16 *clampX;
    s32 z = position->z >> 8;
    s16 **positionZReference;
    s32 cell;
    u8 *properties;
    if ((((x < 0) || (z < 0)) || (((u32)x) >= gUnk_030052F8)) || (((u32)z) >= gUnk_030052FC)) {
        cell = 0;
    } else {
        cell = gUnk_030052F4->rows[z] + x;
    }
    properties = TerrainModifiers_FindCellOverride(cell, 1);
    if (properties) {
        properties += 4;
    } else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = (cell * 4) + 12;
        properties = map->cells + offset;
    }
    {
        u32 attributes = *((u16 *)(properties + 2));
        attributes &= 128;
        if (!attributes) {
            goto done;
        }
    }
    if (!flaggedCellHandlingEnabled()) {
        goto done;
    }
    {
        s32 column =
            Mod(actor->movementProbeStorage.previousCell, ((s16 *)gUnk_030052F4->cells)[2]);
        s32 row = Div(actor->movementProbeStorage.previousCell, ((s16 *)gUnk_030052F4->cells)[2]);
        s32 cellX = column << 8;
        s32 minX = cellX + 1;
        s32 maxX = cellX + 255;
        s32 cellZ = row << 8;
        s32 minZ = cellZ + 1;
        s32 maxZ = cellZ + 255;
        register s16 *positionX asm("r6") = &actor->position.x;
        s16 *positionZ;
        s32 pointX = *positionX;
        s32 dx;
        s32 dz;
        pointX -= 127;
        clampX = positionX;
        dx = cellX - pointX;
        positionZ = &actor->position.z;
        pointX = *positionZ;
        pointX -= 127;
        dz = cellZ - pointX;
        {
            register s32 direction asm("r0") = (u8)Math_Atan2(dx, dz);
            s32 angle;
            direction = (direction - 32) & (~63);
            asm("" : "+r"(direction));
            angle = direction;
            angle += 64;
            if ((*clampX) < minX) {
                *clampX = minX;
            } else if ((*clampX) > maxX) {
                *clampX = maxX;
            }
            positionZReference = &positionZ;
            if ((*(*positionZReference)) < minZ) {
                *positionZ = minZ;
            } else if ((*positionZ) > maxZ) {
                *(*positionZReference) = maxZ;
            }
            TetherActor_RequestKindNineAction(100, angle);
            if (actor->requestMode) {
                Sound_PlaySong(0x3F4);
                Sound_PlaySong(0x4ED);
                --actor->requestMode;
            }
            actor->requestStatus = 1;
            if (actor->onActionScriptId) {
                Script_Call(actor->onActionScriptId, 0);
            }
        }
    }
done:;
}

// MATCHING_CONCESSION: register-pinning

/* Matching adaptation: pinned offset arithmetic and word copies preserve address lifetimes. */

void TetherActor_UpdateWorldPosition(struct TetherActor *actor)
{
    struct ContactPosition *position;
    u32 oldCell;
    u32 oldSlope;
    u32 oldHeight;
    u32 oldAttributes;
    u32 offset;
    if (!actor->statusFlags && actor->state != 5) {
        s32 cell;
        u8 *properties;
        u32 surface, height;
        position = &actor->position;
        cell = cellIndex_0804AF80(position->x >> 8, position->z >> 8);
        properties = cellProperties(cell);
        surface = *properties >> 4;
        height = (*properties & 15) << 8;
        switch (surface) {
            case 1:
                height -= (u8)position->z;
                break;
            case 2:
                height -= (u8)position->x;
                break;
        }
        actor->position.y = height;
    }
    {
        struct DisplayPosition **slot = &actor->display.animation.position;
        u16 *display = (u16 *)*slot;
        register u32 value asm("r0");
        register u32 coordinate asm("r3");
        position = &actor->position;
        offset = 0x37E;
        value = (u16)actor->shakeX;
        coordinate = (u16)position->x;
        display[0] = value + coordinate;
        asm("" ::: "memory");
        display = (u16 *)*slot;
        {
            u16 *axis = (u16 *)&actor->position.y;
            offset += 2;
            value = *(u16 *)((u8 *)actor + offset);
            coordinate = *axis;
            display[1] = value + coordinate;
        }
        asm("" ::: "memory");
        display = (u16 *)*slot;
        {
            u16 *axis = (u16 *)&actor->position.z;
            value = (u16)actor->shakeZ;
            {
                u32 z = *axis;
                display[2] = value + z;
            }
        }
        offset = 0x130;
        Collider_SetPosition(&actor->chargeCollider, position, 0);
    }
    oldCell = actor->movementProbeStorage.cell;
    oldSlope = actor->movementProbeStorage.slope;
    oldHeight = actor->movementProbeStorage.height;
    offset -= 2;
    oldAttributes = actor->movementProbeStorage.attributes;
    if (actor->kind == 9) {
        if (!actor->movementProbe) {
            CellObject_Init(&actor->movementProbeStorage, position);
            actor->movementProbe = &actor->movementProbeStorage;
        }
        Obj_StepPosition(&actor->bodyConfig);
    } else {
        u8 *properties;
        u32 currentCell;
        u32 index = cellIndex_0804AF80(position->x >> 8, position->z >> 8);
        {
            u16 *field = &actor->movementProbeStorage.cell;
            *field = index;
            currentCell = *field;
        }
        {
            u8 *raw = TerrainModifiers_FindCellOverride(currentCell, 1);
            if (raw)
                properties = raw + 4;
            else {
                struct EngineMap *map = gUnk_030052F4;
                u32 offset = currentCell * 4 + 12;
                properties = map->cells + offset;
            }
        }
        actor->movementProbeStorage.slope = *properties >> 4;
        {
            u32 byte = *properties;
            u32 height = 15;
            register u32 heightOffset asm("r4");
            height &= byte;
            heightOffset = 0x129;
            actor->movementProbeStorage.height = height;
            asm("" : : "r"(heightOffset));
        }
        actor->movementProbeStorage.attributes = *(u16 *)(properties + 2);
    }
    actor->movementProbeStorage.previousCell = oldCell;
    {
        register u32 slopeOffset asm("r3") = 0x12A;
        u8 *destination = (u8 *)actor + slopeOffset;
        register u32 sample asm("r4") = oldSlope;
        *destination = sample;
        {
            u32 heightOffset = 0x128;
            register u32 height asm("r2");
            destination = &actor->movementProbeStorage.previousHeight;
            height = oldHeight;
            *destination = height;
        }
        slopeOffset += 2;
        destination = (u8 *)actor + slopeOffset;
        sample = oldAttributes;
        *(u16 *)destination = sample;
    }
    if (actor->kind == 9)
        TetherActor_HandleFlaggedCell(actor);
    TetherActor_UpdateSampledPosition(actor);
    {
        register u32 offset asm("r0") = 0x3F0;
        struct DisplayPosition *target;
        const struct DisplayPosition *source = (const struct DisplayPosition *)&actor->position;
        register u32 first asm("r0");
        u32 second;
        asm("" : "+r"(offset));
        target = (struct DisplayPosition *)((u8 *)actor + offset);
        first = source->words[0];
        second = *(u32 *)((u8 *)actor + 0x54);
        target->words[0] = first;
        target->words[1] = second;
    }
    {
        register u32 headingOffset asm("r1") = 0x260;
        u32 heading;
        asm("" : "+r"(headingOffset));
        {
            register u8 *headingAddress asm("r0") = (u8 *)actor + headingOffset;
            heading = *headingAddress;
        }
        actor->targetPosition.pad = heading;
    }
}

// MATCHING_CONCESSION: register-pinning

u32 TetherActor_TestTerrainTransition(struct TetherActor *actor)
{
    s32 k2 = 1;
    struct ContactPosition *position;
    u8 *properties;
    s32 height, difference;
    u32 surface, result;
    if (!actor->terrainCheckReady) {
        ++actor->terrainCheckReady;
        goto no;
    }
    if (actor->state == 5) {
        goto no;
    }
    if (actor->statusFlags) {
        goto no;
    }
    {
        struct ContactPosition *first = &actor->position;
        s32 x = first->x >> 8;
        s32 z = first->z >> 8;
        s32 cell;
        position = first;
        if ((((x < 0) || (z < 0)) || (((u32)x) >= gUnk_030052F8)) || (((u32)z) >= gUnk_030052FC)) {
            cell = 0;
        } else {
            cell = gUnk_030052F4->rows[z] + x;
        }
        properties = TerrainModifiers_FindCellOverride(cell, 1);
        if (properties) {
            properties += 4;
        } else {
            struct EngineMap *map = gUnk_030052F4;
            u32 offset = cell * 4 + 12;
            properties = map->cells + offset;
        }
    }
    {
        u32 attributes = *(u16 *)(properties + 2);
        attributes &= 64;
        if (attributes) {
            goto yes;
        }
    }
    if (actor->state == k2) {
        goto no;
    }
    properties = propertiesAt_0804B1CC(position);
    surface = *properties >> 4;
    height = (*properties & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    difference = height - actor->position.y;
    if (difference < 0) {
        difference = -difference;
    }
    if (difference <= 255) {
        goto no;
    }
yes:
    result = 1;
    goto done;
no:
    result = 0;
done:
    return result;
}

u32 TetherActor_SelectNextState(struct TetherActor *p)
{
    if (p->kind == 9 && p->actionRequested != 0) {
        p->actionRequested = 0;
        return 0xb;
    }
    if (TetherActor_TestTerrainTransition(p) != 0 && p->kind != 9)
        TetherActor_BeginAuxiliaryState(p);
    switch (p->state) {
        case 0:
        case 3:
        case 4:
            if (gUnk_02000580[0]->tetherMode == 6)
                goto r1;
            break;
        case 1:
            if (gUnk_02000580[0]->tetherMode == 6) {
            r1:
                return 1;
            }
            return 0;
        case 0x10:
            if (gUnk_02000580[0]->tetherMode == 6)
                goto r11;
            break;
        case 0x11:
            if (gUnk_02000580[0]->tetherMode == 6) {
            r11:
                return 0x11;
            }
            return 0x10;
    }
    return p->state;
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_UpdateLockedShake(struct TetherActor *actor)
{
    s32 amplitude;
    register const u16 *randomTable asm("r6");
    if (gUnk_030053F4 & 512)
        return;
    if (actor->lockFrame <= 63)
        amplitude = 16;
    else {
        actor->glowMode = 2;
        amplitude = (actor->lockFrame >> 2) + 16;
    }
    randomTable = (const u16 *)RANDOM_TABLE_ADDR;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    actor->shakeX = Mod(randomTable[gUnk_03005308], amplitude) - (amplitude >> 1);
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    actor->shakeZ = Mod(randomTable[gUnk_03005308], amplitude) - (amplitude >> 1);
    if ((++actor->lockSoundFrame & 7) == 0) {
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        {
            u32 sample = randomTable[gUnk_03005308];
            sample &= 1;
            if (sample)
                Sound_PlaySong(0x2B9);
        }
    }
    if (++actor->lockFrame > 127) {
        actor->interactionLock = 0;
        if (actor->state == 1)
            actor->transitionReason = 2;
        else
            actor->transitionReason = 1;
        TetherActor_SetState(actor, 2);
        actor->recoveryTimer = actor->recoveryDuration;
    }
}

void TetherActor_UpdateInteractionValue(struct TetherActor *p)
{
    if (!(gUnk_030053F4 & 0x200)) {
        if (p->interactionEnabled != 0) {
            if (p->interactionReset == 0) {
                if (p->statusFlags == 0) {
                    u16 *q = &p->currentValue;
                    if (*q != 0)
                        *q = *q - 1;
                    else
                        TetherActor_SelectInteractionState(p);
                }
            }
        }
    }
}

extern u32 gUnk_030053F4;

void TetherActor_UpdateIdleState(struct TetherActor *p)
{
    if (p->state != 0)
        TetherActor_SetState(p, 0);
    {
        u16 *q = &p->chargeCollider.flags;
        u32 v = *q;
        u32 m = 0xFFFB;
        u32 z;
        m &= v;
        z = 0;
        *q = m;
        p->glowMode = z;
    }
    TetherActor_SetEntry(p, 0);
    if (!(gUnk_030053F4 & 0x200)) {
        if (p->kind != 9) {
            u16 *r3 = &p->interactionLock;
            if (*r3 != 0) {
                TetherActor_UpdateLockedShake(p);
            } else {
                u32 v = p->chargeReady;
                if (v != 0) {
                    TetherActor_UpdateShake(p);
                } else if (p->blendRequested != 0) {
                    u32 t = p->secondaryMaximum;
                    u32 t2;
                    p->secondaryValue = t;
                    t2 = p->maximumValue;
                    p->currentValue = t2;
                    *r3 = v;
                } else {
                    TetherActor_UpdateInteractionValue(p);
                }
            }
        }
    }
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_BlockPlayerAcrossTerrain(struct TetherActor *actor)
{
    u8 *properties = cellProperties(
        cellIndex_0804B6A4(gUnk_02000580[0]->position.x >> 8, gUnk_02000580[0]->position.z >> 8));
    u32 attributes = *(u16 *)(properties + 2);
    struct ContactPosition *position;
    s32 playerCell;
    s32 actorCell;
    register s32 rowStride asm("r1");
    attributes &= 1;
    position = &actor->position;
    if (attributes == 0) {
        s32 playerHeight =
            surfaceHeight((const struct ContactPosition *)&gUnk_02000580[0]->position);
        s32 height, difference;
        asm("" : "+r"(actor));
        {
            struct ContactPosition *heightPosition = &actor->position;
            s32 x = heightPosition->x >> 8, z = heightPosition->z >> 8;
            u8 *properties;
            u32 surface;
            position = heightPosition;
            properties = cellProperties(cellIndex_0804B6A4(x, z));
            surface = *properties >> 4;
            height = (*properties & 15) << 8;
            switch (surface) {
                case 1:
                    height -= (u8)heightPosition->z;
                    break;
                case 2:
                    height -= (u8)heightPosition->x;
                    break;
            }
        }
        difference = playerHeight - height;
        if (difference < 0)
            difference = -difference;
        if (difference <= 255)
            return;
    }
    {
        s32 x;
        s32 z;
        u32 raw;
        raw = (u16)gUnk_02000580[0]->position.x;
        x = (s16)raw >> 8;
        raw = (u16)gUnk_02000580[0]->position.z;
        z = (s16)raw >> 8;
        playerCell = cellIndex_0804B6A4(x, z);
        raw = (u16)position->x;
        x = (s16)raw >> 8;
        raw = (u16)position->z;
        z = (s16)raw >> 8;
        actorCell = cellIndex_0804B6A4(x, z);
    }
    if (actorCell == playerCell + 1) {
        gUnk_02000580[0]->position.x = ((position->x >> 8) << 8) + 2;
        return;
    }
    if (actorCell == playerCell - 1) {
        gUnk_02000580[0]->position.x = (((position->x >> 8) + 1) << 8) - 2;
        return;
    }
    rowStride = *(s16 *)(gUnk_030052F4->cells + 4);
    if (actorCell == playerCell + rowStride)
        gUnk_02000580[0]->position.z = ((actor->position.z >> 8) << 8) + 2;
    else if (actorCell == playerCell - rowStride)
        gUnk_02000580[0]->position.z = (((actor->position.z >> 8) + 1) << 8) - 2;
    else {
        asm("" : : "r"(rowStride), "r"(playerCell));
        gUnk_02000580[0]->position.x = position->x;
        gUnk_02000580[0]->position.z = actor->position.z;
    }
}

static inline s32 TetherActor_Cosine70(s32 angle)
{
    s32 result = truncateShift(gUnk_086149C4[(angle + 64) & 255] * 70, 12);
    return result;
}

static inline s32 TetherActor_Sine70(s32 angle)
{
    s32 result = truncateShift(gUnk_086149C4[angle & 255] * 70, 12);
    return result;
}

static inline s32 TetherActor_Cosine150(s32 angle)
{
    s32 result = truncateShift(gUnk_086149C4[(angle + 64) & 255] * 150, 12);
    return result;
}

void TetherActor_LimitPlayerDistance(struct TetherActor *actor)
{
    s32 angle = (((*gUnk_02000580)->facing + 1) & 7) << 5;
    s32 offsetX = TetherActor_Cosine70(angle);
    s32 offsetZ = TetherActor_Sine70(angle);
    s32 dx = (*gUnk_02000580)->position.x + offsetX - actor->position.x;
    s32 dz = (*gUnk_02000580)->position.z + offsetZ - actor->position.z;
    if (dx * dx + dz * dz > 22500) {
        s32 radialZ, targetX, targetZ;
        angle = Math_Atan2(dx, dz);
        /* The radial X offset reuses dx's register. */
        dx = TetherActor_Cosine150(angle);
        radialZ = truncateShift(gUnk_086149C4[angle & 255] * 150, 12);
        targetX = actor->position.x - offsetX + dx;
        targetZ = actor->position.z - offsetZ + radialZ;
        (*gUnk_02000580)->position.x = truncateShift((*gUnk_02000580)->position.x * 3 + targetX, 2);
        (*gUnk_02000580)->position.z = truncateShift((*gUnk_02000580)->position.z * 3 + targetZ, 2);
        TetherActor_BlockPlayerAcrossTerrain(actor);
        (*gUnk_02000580)->colliderPosition = (*gUnk_02000580)->position;
    }
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_UpdateSecondaryLock(struct TetherActor *input)
{
    register struct TetherActor *actor asm("r2") = input;
    if (!actor->statusFlags) {
        u16 *value = &actor->secondaryValue;
        u32 loaded = *value;
        register u32 current asm("r3");
        current = loaded;
        if (current)
            *value = loaded - 1;
        else {
            *value = actor->secondaryMaximum;
            actor->interactionLock = 1;
            {
                register u16 *frame asm("r0") = &actor->lockFrame;
                *frame = current;
            }
        }
    }
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_UpdateFollowingState(struct TetherActor *actor)
{
    if (actor->state != 1)
        TetherActor_SetState(actor, 1);
    if (actor->interactionReset && actor->interactionEnabled) {
        s32 x = actor->position.x;
        s32 differenceX = absolute_0804BB30(x - actor->interactionTarget.x);
        /* The second comparison also uses X in the original. */
        s32 differenceZ = absolute_0804BB30(x - actor->interactionTarget.z);
        if (differenceX > 256 || differenceZ > 256)
            actor->interactionReset = 0;
    }
    actor->glowMode = 0;
    if (actor->parameter >= 0)
        TetherActor_LimitPlayerDistance(actor);
    else {
        register s16 *x asm("r2") = &actor->position.x;
        s16 *z;
        register s32 dx asm("r1") = *x;
        s32 target = actor->targetPosition.x;
        dx = absolute_0804BB30(dx - target);
        z = &actor->position.z;
        if (dx > 2 || absolute_0804BB30(*z - actor->targetPosition.z) > 2)
            actor->moving = 1;
        *x = actor->targetPosition.x;
        *z = actor->targetPosition.z;
    }
    actor->heading = actor->targetPosition.pad;
    if (actor->kind == 5 && actor->moving)
        TetherActor_SetHeadingEntry(actor, 36);
    else
        TetherActor_SetEntry(actor, 0);
    if (!(gUnk_030053F4 & 512) && actor->kind != 9) {
        u16 *lock = &actor->interactionLock;
        if (*lock)
            TetherActor_UpdateLockedShake(actor);
        else {
            u32 ready = actor->chargeReady;
            if (ready)
                TetherActor_UpdateShake(actor);
            else if (actor->blendRequested) {
                actor->secondaryValue = actor->secondaryMaximum;
                actor->currentValue = actor->maximumValue;
                *lock = ready;
            } else
                TetherActor_UpdateSecondaryLock(actor);
        }
    }
}

// MATCHING_CONCESSION: register-pinning

u32 TetherActor_TestStepPosition(struct ContactPosition *origin, struct ContactPosition *input,
                                 u32 inputMask)
{
    register struct ContactPosition *candidate asm("r6");
    u32 mask;
    u8 *properties;
    u32 attributes;
    s32 height, difference;
    u32 surface;
    candidate = input;
    asm("" : : "r"(candidate));
    mask = inputMask;
    properties = propertiesAt_0804BCD4(candidate);
    attributes = *(u16 *)(properties + 2);
    mask &= attributes;
    if (mask)
        goto no;
    if ((attributes & 128) && terrainFlagEnabled())
        goto no;
    properties = propertiesAt_0804BCD4(candidate);
    surface = *properties >> 4;
    height = (*properties & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)candidate->z;
            break;
        case 2:
            height -= (u8)candidate->x;
            break;
    }
    difference = origin->y - height;
    if (difference < 0)
        difference = -difference;
    if (difference > 50)
        goto no;
    if (NamedObject_FindNearPosition(candidate))
        goto no;
    return 1;
no:
    return 0;
}

u32 TetherActor_TestStepDelta(struct ContactPackedVector *origin, struct ContactPackedVector *delta,
                              u32 mask)
{
    struct ContactPackedVector candidate;
    if (!delta->x) {
        candidate = *origin;
        candidate.z += delta->z;
        if (TetherActor_TestStepPosition((void *)origin, (void *)&candidate, mask))
            goto yes;
        goto no;
    } else if (!delta->z) {
        candidate = *origin;
        *(u16 *)&candidate += (u16)delta->x;
        if (TetherActor_TestStepPosition((void *)origin, (void *)&candidate, mask))
            goto yes;
        goto no;
    } else {
        candidate = *origin;
        *(u16 *)&candidate += (u16)delta->x;
        if (!TetherActor_TestStepPosition((void *)origin, (void *)&candidate, mask))
            goto no;
        candidate = *origin;
        ((u16 *)&candidate)[2] += (u16)delta->z;
        if (!TetherActor_TestStepPosition((void *)origin, (void *)&candidate, mask))
            goto no;
        *(u16 *)&candidate += (u16)delta->x;
    }
    if (TetherActor_TestStepPosition((void *)origin, (void *)&candidate, mask))
        goto yes;
no:
    return 0;
yes:
    return 1;
}

u32 TetherActor_TestNearDelta(s32 a, s32 b)
{
    if (a < 0)
        a = -a;
    if (a > 0x20)
        return 0;
    if (b < 0)
        b = -b;
    if (b > 0x20)
        return 0;
    return 1;
}

void TetherActor_UpdateRoamingState(struct TetherActor *actor)
{
    struct ContactPackedVector delta;
    if (actor->state != 3)
        TetherActor_SetState(actor, 3);
    if (gUnk_030053F4 & 512)
        return;
    if (actor->chargeReady) {
        actor->glowMode = 0;
        actor->roamFrame = 0;
        TetherActor_UpdateShake(actor);
    } else {
        actor->glowMode = 1;
        if (actor->roamFrame > 90) {
            s32 dx = actor->interactionProbe.target.x - actor->position.x;
            s32 dz = actor->interactionProbe.target.z - actor->position.z;
            if (dx || dz) {
                u32 angle = Math_Atan2(dx, dz);
                delta.x = scale(gUnk_086149C4[(angle + 64) & 255] * 3);
                delta.y = 0;
                delta.z = scale(gUnk_086149C4[angle & 255] * 3);
                if (TetherActor_TestStepDelta((struct ContactPackedVector *)&actor->position,
                                              &delta, 0x841)) {
                    actor->position.x += delta.x;
                    actor->position.z += delta.z;
                    actor->moving = 1;
                } else
                    actor->moving = 0;
                {
                    s32 sector = ((u16)angle + 112) >> 5;
                    u32 mask = 7;
                    mask &= sector;
                    actor->heading = mask;
                }
            }
            if (!(actor->interactionProbe.flags & 1) && TetherActor_TestNearDelta(dx, dz) &&
                actor->interactionScriptId)
                Script_Call(actor->interactionScriptId, 0);
        }
        if (++actor->roamFrame > 119)
            actor->roamFrame = 0;
        if (actor->kind == 5 && actor->moving)
            TetherActor_SetHeadingEntry(actor, 36);
        else
            TetherActor_SetEntry(actor, 0);
    }
    NavMesh_UpdateProbe(&actor->interactionProbe, &actor->position);
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_UpdateWanderingState(struct TetherActor *actor)
{
    if (actor->state != 4) {
        TetherActor_SetState(actor, 4);
    }
    if (gUnk_030053F4 & 512) {
        return;
    }
    if (actor->chargeReady) {
        actor->glowMode = 0;
        actor->roamFrame = 0;
        TetherActor_UpdateShake(actor);
        return;
    }
    actor->glowMode = 1;
    if (actor->roamFrame == 90) {
        if (actor->turnAfterBlock) {
            gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
            actor->heading =
                (actor->heading + Mod(((u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308], 5) + 2) & 7;
            actor->turnAfterBlock = 0;
        } else {
            gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
            actor->heading =
                (actor->heading + Mod(((u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308], 3) + 7) & 7;
        }
    } else if (actor->roamFrame > 90) {
        u32 angle;
        const s16 *table;
        struct ContactPackedVector delta;
        struct ContactPosition *position;
        {
            u32 value = (actor->heading + 5) & 7;
            angle = value << 5;
        }
        do {
            table = gUnk_086149C4;
        } while (0);
        delta.x = scale(table[(angle + 64) & 255] * 3);
        delta.y = 0;
        delta.z = scale(table[angle] * 3);
        position = &actor->position;
        if (TetherActor_TestStepDelta((struct ContactPackedVector *)position, &delta, 0x843)) {
            position->x += delta.x;
            actor->position.z += delta.z;
            actor->moving = 1;
        } else {
            actor->roamFrame = 30;
            actor->turnAfterBlock = 1;
        }
    }
    if ((actor->kind == 5) && actor->moving) {
        TetherActor_SetHeadingEntry(actor, 36);
    } else {
        TetherActor_SetEntry(actor, 0);
    }
    {
        u16 *frame = &actor->roamFrame;
        if (++*frame > 119) {
            TetherActor_SelectInteractionState(actor);
            *frame = 0;
        }
    }
}

// MATCHING_CONCESSION: register-pinning

/* Matching adaptation: existing pinned offsets keep collider, shake and counter addresses live. */

void TetherActor_UpdateRecoveryState(struct TetherActor *actor)
{
    u32 frozen;
    u32 rawTimer;
    u32 highTimer;
    register u16 *timer asm("r8");
    register u32 counterOffset asm("r6");
    if (actor->state != 2)
        TetherActor_SetState(actor, 2);
    frozen = gUnk_030053F4 & 512;
    if (frozen)
        return;
    rawTimer = actor->recoveryTimer;
    highTimer = rawTimer << 16;
    if ((highTimer >> 16) > 64) {
        struct ContactCollider *collider;
        register u32 colliderOffset asm("r6");
        actor->glowMode = 2;
        if (actor->parameter >= 0)
            SceneActorControl_RequestDeparture(actor->parameter);
        if (actor->transitionReason == 2) {
            actor->secondaryCollider.bounds.x = 256;
            {
                register u32 offset asm("r6") = 0x1A2;
                register u16 *height asm("r2");
                asm("" : "+r"(offset));
                height = (u16 *)((u8 *)actor + offset);
                *height = 32;
            }
            actor->secondaryCollider.bounds.z = 256;
            actor->transitionReason = 1;
        } else if (actor->transitionReason == 1) {
            actor->secondaryCollider.bounds.x = 128;
            actor->secondaryCollider.bounds.y = 32;
            actor->secondaryCollider.bounds.z = 128;
            actor->transitionReason = frozen;
        }
        colliderOffset = 0x184;
        collider = &actor->secondaryCollider;
        Collider_SetPosition(collider, &actor->position, 0);
        Collider_Refresh(collider);
        {
            u32 offset = 0x400;
            timer = (u16 *)(offset + (u32)actor);
        }
        if ((*timer & 31) == 0) {
            gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
            {
                u32 sample = Mod(((u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308], 3);
                register u8 *heading asm("r2");
                colliderOffset += 0xDC;
                heading = (u8 *)actor + colliderOffset;
                *heading = (*heading + sample - 1) & 7;
            }
            TetherActor_SetEntry(actor, 0);
        }
        {
            const u16 *table;
            register const u16 *rawTable asm("r0");
            u32 *randomIndex;
            register u32 index asm("r1");
            u32 mask;
            register u32 amplitudeMask asm("r4");
            register u32 shakeOffset asm("r6");
            rawTable = (const u16 *)RANDOM_TABLE_ADDR;
            asm("" : "+r"(rawTable));
            table = rawTable;
            randomIndex = &gUnk_03005308;
            index = *randomIndex + 1;
            mask = 1023;
            index &= mask;
            {
                register u32 sample asm("r0") = table[index];
                s16 *shake;
                amplitudeMask = 63;
                sample &= amplitudeMask;
                sample -= 31;
                shakeOffset = 0x37E;
                shake = (s16 *)((u8 *)actor + shakeOffset);
                *shake = sample;
            }
            index = (index + 1) & mask;
            *randomIndex = index;
            actor->shakeZ = (table[index] & amplitudeMask) - 31;
            asm("" : : "r"(shakeOffset));
        }
        {
            register u16 *frame asm("r6") = timer;
            if ((*frame & 7) == 0)
                TetherActor_EmitRandomEffect(actor);
        }
        {
            u32 chargeHigh = (u32)actor->charge << 16;
            if (chargeHigh) {
                s32 reduction = chargeHigh >> 19;
                register u16 *current asm("r6") = timer;
                u32 remaining = *current;
                if ((s32)remaining < reduction + 64) {
                    *current = 64;
                } else {
                    u16 *destination;
                    register u32 value asm("r0") = remaining - reduction;
                    destination = timer;
                    *destination = value;
                }
                actor->charge = 0;
                {
                    register u32 readyOffset asm("r6") = 0x384;
                    actor->chargeReady = 0;
                    asm("" : : "r"(readyOffset));
                }
            }
        }
    } else {
        s32 amplitude = highTimer >> 17;
        register const u16 *table asm("r8");
        u32 *randomIndex;
        u32 randomMask;
        if (amplitude < 2)
            amplitude = 2;
        table = (const u16 *)RANDOM_TABLE_ADDR;
        {
            u32 *slot = &gUnk_03005308;
            u32 index;
            u32 mask;
            randomIndex = slot;
            index = *slot + 1;
            mask = 1023;
            randomMask = mask;
            index &= mask;
            *slot = index;
            actor->shakeX = Mod(table[index], amplitude) - (amplitude >> 1);
        }
        {
            u32 *slot = randomIndex;
            u32 index = *slot + 1;
            u32 mask = randomMask;
            index &= mask;
            *slot = index;
            actor->shakeZ = Mod(table[index], amplitude) - (amplitude >> 1);
        }
        actor->glowMode = frozen;
        actor->charge = frozen;
        actor->chargeReady = frozen;
        {
            u32 soundFrame = actor->lockSoundFrame + 1;
            register u32 truncation asm("r6");
            u32 mask;
            actor->lockSoundFrame = soundFrame;
            truncation = 65535;
            asm("" : "+r"(truncation));
            mask = truncation;
            asm("" : "+r"(mask));
            soundFrame &= mask;
            mask = 7;
            soundFrame &= mask;
            if (soundFrame == 0) {
                u32 *slot = randomIndex;
                u32 index = *slot + 1;
                u32 mask = randomMask;
                u32 sample;
                index &= mask;
                *slot = index;
                sample = table[index];
                sample &= 1;
                if (sample)
                    Sound_PlaySong(0x2B9);
            }
        }
    }
    counterOffset = 0x400;
    asm("" : "+r"(counterOffset));
    {
        register u16 *counter asm("r1") = (u16 *)((u8 *)actor + counterOffset);
        if (--*counter == 0) {
            if (actor->unknown1B == 1) {
                TetherActor_SetState(actor, 16);
                {
                    u32 offset = 0x46A;
                    u16 *directed = &actor->directedTimer;
                    *directed = 30;
                }
            } else
                TetherActor_SelectInteractionState(actor);
        }
    }
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_UpdateDisplayDeparture(struct TetherActor *actor)
{
    u16 *frame;
    register u32 current asm("r0");
    actor->heading = 0;
    actor->chargeCollider.flags |= 4;
    frame = &actor->stateFrame;
    ++*frame;
    TetherActor_SetEntry(actor, 0);
    current = *frame;
    asm("" : "+r"(current) : : "memory");
    if (current <= 27)
        return;
    if (current <= 43) {
        u32 savedFrame = current;
        s32 phase = savedFrame - 28;
        struct DisplayAnimation *display = &actor->display.animation;
        s32 first, second;
        DisplayAnimation_SetFlags(display, 2);
        first = 64 - phase * 4;
        second = 64;
        if (phase > 8) {
            second = phase * 8;
            {
                u32 height = (savedFrame - 36) * 30;
                actor->displayOffsetY = height;
            }
        }
        DisplayAnimation_SetPalette(display, first, second, 0);
    } else if (current <= 51)
        actor->displayOffsetY = (*frame - 44) * 200 + 240;
    else
        DisplayAnimation_SetFlags(&actor->display.animation, 1);
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_UpdateDisplayReturn(struct TetherActor *actor)
{
    u32 zero;
    u8 *heading = &actor->heading;
    u16 *frame;
    u32 current;
    zero = 0;
    *heading = zero;
    actor->chargeCollider.flags |= 4;
    TetherActor_SetEntry(actor, 0);
    frame = &actor->stateFrame;
    current = *frame;
    asm("" : "+r"(current) : : "memory");
    if (current <= 75)
        DisplayAnimation_SetFlags(&actor->display.animation, 1);
    else if (current <= 83) {
        u32 phase = *frame - 76;
        struct DisplayAnimation *display = &actor->display.animation;
        DisplayAnimation_ClearFlags(display, 1);
        DisplayAnimation_SetFlags(display, 2);
        DisplayAnimation_SetPalette(display, 4, 120, 0);
        actor->displayOffsetY = (8 - phase) * 200 + 240;
    } else if (current <= 99) {
        s32 phase = *frame - 84;
        s32 remaining = 15 - phase;
        s32 first = 64 - remaining * 4, second;
        if (phase <= 7) {
            second = remaining * 8;
            actor->displayOffsetY = (8 - phase) * 30;
        } else {
            second = 64;
            actor->displayOffsetY = zero;
        }
        DisplayAnimation_SetPalette(&actor->display.animation, first, second, 0);
    }
    if (++actor->stateFrame > 103) {
        DisplayAnimation_ClearFlags(&actor->display.animation, 2);
        TetherActor_SetState(actor, 0);
    }
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_UpdateAuxiliaryTransition(struct TetherActor *actor)
{
    u32 visible;
    u32 emit;
    struct DisplayAnimation *display;
    struct DisplayAnimation *activeDisplay;
    register u16 *frame asm("r4");
    register u32 rawFrame asm("r2");
    u32 highFrame;
    u32 current;
    if (actor->state != 5)
        TetherActor_SetState(actor, 5);
    if (gUnk_030053F4 & 512)
        return;
    actor->glowMode = 2;
    actor->heading = 0;
    actor->chargeCollider.flags |= 4;
    visible = 0;
    emit = visible;
    frame = &actor->stateFrame;
    rawFrame = *frame;
    highFrame = rawFrame << 16;
    current = highFrame >> 16;
    if (current <= 63) {
        actor->position.y = (actor->position.y * 7 + actor->auxiliaryOrigin.y) >> 3;
        goto show;
    }
    if (current <= 127) {
        s32 shake;
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        {
            u32 bit = ((const u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308];
            bit &= 1;
            shake = (bit << 4) + bit - 8;
        }
        actor->shakeX = shake;
        actor->shakeZ = -shake;
        visible = 1;
        if ((*frame & 15) == 0)
            emit = visible;
    } else if (current <= 191) {
        if ((highFrame >> 17) & 3)
            visible = 1;
        if ((rawFrame & 15) == 0)
            emit = 1;
    } else {
        if ((highFrame >> 17) & 1)
            visible = 1;
        if ((rawFrame & 15) == 0)
            emit = 1;
    }
    if (visible) {
    show:
        activeDisplay = &actor->display.animation;
        DisplayAnimation_ClearFlags(activeDisplay, 1);
        actor->auxiliarySprite.flags &= ~1;
        {
            struct ContactPosition *position = &actor->auxiliarySprite.position;
            s32 x, z;
            u8 *properties;
            u32 surface;
            s32 height;
            x = position->x >> 8;
            z = position->z >> 8;
            display = activeDisplay;
            properties = propertiesAt_0804C748(x, z);
            surface = *properties >> 4;
            height = (*properties & 15) << 8;
            switch (surface) {
                case 1:
                    height -= (u8)position->z;
                    break;
                case 2:
                    height -= (u8)position->x;
                    break;
            }
            actor->auxiliarySprite.position.y = height;
        }
    } else {
        activeDisplay = &actor->display.animation;
        DisplayAnimation_SetFlags(activeDisplay, 1);
        {
            u32 *flags = &actor->auxiliarySprite.flags;
            register u32 value asm("r0") = *flags;
            value |= 1;
            *flags = value;
        }
        display = activeDisplay;
    }
    if (emit)
        TetherActor_EmitPointEffect((const struct ContactPackedVector *)&actor->position,
                                    &actor->stateOption);
    TetherActor_SetEntry(actor, 0);
    if (++actor->stateFrame > 255) {
        TetherActor_RemoveAuxiliarySprite(actor);
        TetherActor_SetState(gUnk_02000488, 0);
        if (actor->interactionScriptId) {
            u32 script = actor->interactionScriptId;
            actor->interactionScriptId = 0;
            Script_Call(script, 0);
            actor->destroyRequested = 1;
            DisplayAnimation_SetFlags(display, 1);
        } else {
            DisplayAnimation_SetFlags(display, 1);
            actor->destroyRequested = 1;
        }
    }
}

void *ActorAttachment_ReleaseResource(void *, u32, u32);

// MATCHING_CONCESSION: register-pinning

void TetherActor_UpdateDescendingState(struct TetherActor *actor)
{
    u16 *frame;
    register u32 raw asm("r0");
    register u32 incremented asm("r1");
    register u32 current asm("r0");
    u32 zero;
    if (actor->state != 6)
        TetherActor_SetState(actor, 6);
    zero = gUnk_030053F4 & 512;
    if (zero)
        return;
    actor->glowMode = zero;
    frame = &actor->stateFrame;
    raw = *frame;
    incremented = raw + 1;
    *frame = incremented;
    current = (u16)incremented;
    if (current <= 5) {
        current = (u16)incremented;
        if (current == 1) {
            u16 *value = &actor->effectValue;
            void **handle;
            *value = zero;
            handle = &actor->pendingEffectHandle;
            *handle = ActorAttachment_UpdateFromDescriptor(*handle, &actor->bodyConfig, 1, 0, value,
                                                           200, 250, 290);
        }
    } else {
        current = (u16)incremented;
        if (current == 6) {
            void **handle = &actor->pendingEffectHandle;
            *handle = ActorAttachment_ReleaseResource(*handle, 1, actor->bodyConfig.id);
        }
        actor->displayOffsetY -= 30;
        if (*frame > 8)
            actor->destroyRequested = 1;
    }
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_UpdateRapidDescendingState(struct TetherActor *actor)
{
    u16 *frame;
    register u32 raw asm("r0");
    register u32 incremented asm("r1");
    register u32 current asm("r0");
    u32 zero;
    if (actor->state != 7)
        TetherActor_SetState(actor, 7);
    {
        u16 *glow = &actor->glowMode;
        zero = 0;
        *glow = zero;
    }
    frame = &actor->stateFrame;
    raw = *frame;
    incremented = raw + 1;
    *frame = incremented;
    current = (u16)incremented;
    if (current <= 5) {
        current = (u16)incremented;
        if (current == 1) {
            u16 *value = &actor->effectValue;
            void **handle;
            *value = zero;
            handle = &actor->pendingEffectHandle;
            *handle = ActorAttachment_UpdateFromDescriptor(*handle, &actor->bodyConfig, 1, 0, value,
                                                           200, 250, 290);
        }
    } else {
        current = (u16)incremented;
        if (current == 6) {
            void **handle = &actor->pendingEffectHandle;
            *handle = ActorAttachment_ReleaseResource(*handle, 1, actor->bodyConfig.id);
        }
        actor->displayOffsetY -= 60;
        if (*frame > 29)
            actor->destroyRequested = 1;
    }
}

// MATCHING_CONCESSION: register-pinning

/* Matching adaptation: retain pinned field offsets and unsigned halfword accesses. */

void TetherActor_UpdateBounceReturn(struct TetherActor *actor)
{
    u16 *frame;
    u32 zero;
    register u32 frameValue asm("r0");
    if (actor->state != 8)
        TetherActor_SetState(actor, 8);
    {
        u16 *glow = &actor->glowMode;
        zero = 0;
        *glow = zero;
    }
    frame = &actor->stateFrame;
    frameValue = *frame;
    if (!frameValue) {
        u16 *height;
        actor->shakeX = zero;
        actor->shakeZ = zero;
        height = &actor->displayOffsetY;
        if (*height <= 30) {
            TetherActor_RemoveAuxiliarySprite(actor);
            *height = zero;
            *frame = 1;
        } else
            *height -= 30;
        return;
    }
    {
        register u32 incremented asm("r1") = frameValue + 1;
        u32 current;
        *frame = incremented;
        current = (u16)incremented;
        if (current <= 7) {
            current = (u16)incremented;
            if (current <= 3)
                actor->displayOffsetY = incremented << 4;
            else {
                u32 height = (7 - incremented) << 4;
                register u32 heightOffset asm("r6") = 0x380;
                register u16 *destination asm("r1");
                asm("" : "+r"(heightOffset));
                destination = (u16 *)((u8 *)actor + heightOffset);
                *destination = height;
            }
        } else
            actor->displayOffsetY = zero;
    }
    {
        u16 *counter;
        u32 offset = 0x256;
        register u16 *counterAddress asm("r2");
        asm("" : "+r"(offset));
        counterAddress = (u16 *)(offset + (u32)actor);
        counter = counterAddress;
        if (*counterAddress <= 11) {
            register const u16 *rawTable asm("r6") = (const u16 *)RANDOM_TABLE_ADDR;
            register const u16 *table asm("r12");
            u32 *randomIndex;
            register u32 index asm("r1");
            u32 mask;
            register u32 amplitudeMask asm("r4");
            register u32 shakeOffset asm("r6");
            asm("" : "+r"(rawTable));
            table = rawTable;
            randomIndex = &gUnk_03005308;
            index = *randomIndex + 1;
            mask = 1023;
            index &= mask;
            {
                u32 sample = table[index];
                s16 *shake;
                amplitudeMask = 31;
                sample &= amplitudeMask;
                sample -= 16;
                shakeOffset = 0x37E;
                shake = (s16 *)((u8 *)actor + shakeOffset);
                *shake = sample;
            }
            index = (index + 1) & mask;
            *randomIndex = index;
            actor->shakeZ = (table[index] & amplitudeMask) - 16;
            asm("" : : "r"(shakeOffset));
        }
        {
            register u16 *frameAlias asm("r6") = counter;
            asm("" : "+r"(frameAlias));
            if (*frameAlias == 2)
                TetherActor_EmitRingEffects(actor);
        }
        {
            u16 *frameAlias = counter;
            if (*frameAlias > 29) {
                register u16 *charge asm("r2");
                void **handle;
                TetherActor_SetState(actor, 0);
                {
                    u32 readyOffset = 0x384;
                    u16 *ready;
                    ready = &actor->chargeReady;
                    *ready = 1;
                }
                {
                    register u32 chargeOffset asm("r6") = 0x386;
                    u32 value;
                    asm("" : "+r"(chargeOffset));
                    charge = (u16 *)((u8 *)actor + chargeOffset);
                    value = 287;
                    *charge = value;
                    handle = &actor->pendingEffectHandle;
                    *handle = ActorAttachment_UpdateFromDescriptor(*handle, &actor->bodyConfig, 1,
                                                                   0, charge, value, 250, 290);
                }
            }
        }
    }
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_UpdateKindNineRecoil(struct TetherActor *actor)
{
    if (actor->state != 11) {
        TetherActor_SetState(actor, 11);
        if (!actor->movementProbe) {
            CellObject_Init(&actor->movementProbeStorage, &actor->position);
            actor->movementProbe = &actor->movementProbeStorage;
        }
        Sound_PlaySong(0x3F3);
    }
    if (!actor->stateFrame)
        actor->displayOffsetY = 300;
    {
        register u32 angle asm("r3") = actor->actionAngle;
        const s16 *table = gUnk_086149C4;
        actor->movementDelta.x = scale(table[(angle + 64) & 255] * actor->actionMagnitude);
        actor->movementDelta.z = scale(table[angle] * actor->actionMagnitude);
        asm("" : : "r"(angle));
    }
    actor->actionMagnitude = (actor->actionMagnitude * 3) >> 2;
    {
        register u16 *frame asm("r3") = &actor->stateFrame;
        u32 current;
        current = *frame;
        if (current > 3) {
            u16 *height = &actor->displayOffsetY;
            register u32 remaining asm("r2") = *height;
            u32 reduction = current << 3;
            if ((s32)remaining >= (s32)reduction)
                reduction = remaining - reduction;
            else
                reduction = 0;
            *height = reduction;
        }
    }
    ++actor->stateFrame;
    if (actor->actionMagnitude <= 19 && !actor->displayOffsetY) {
        actor->displayOffsetY = 0;
        Sound_PlaySong(0x2A8);
        TetherActor_SetState(actor, 0);
    }
}

void TetherActor_UpdatePassiveState(struct TetherActor *actor)
{
    if (actor->state != 12)
        TetherActor_SetState(actor, 12);
    actor->glowMode = 0;
    actor->interactionLock = 0;
    actor->chargeCollider.flags |= 4;
    actor->heading = 0;
    TetherActor_SetEntry(actor, 0);
}

void TetherActor_UpdateSpecialEntryTransition(struct TetherActor *p)
{
    u16 *q;
    u32 z, v, m;
    if (p->state != 0xe)
        TetherActor_SetState(p, 0xe);
    q = &p->chargeCollider.flags;
    v = *q;
    m = 4;
    z = 0;
    m |= v;
    *q = m;
    p->heading = z;
    if (TetherActor_SetEntry(p, 9))
        TetherActor_SetState(p, 0xd);
}

void TetherActor_UpdateSpecialEntryState(struct TetherActor *actor)
{
    if (actor->state != 13)
        TetherActor_SetState(actor, 13);
    if (actor->kind == 6)
        actor->glowMode = 1;
    else if (actor->kind == 7) {
        actor->blendHold = 1;
        actor->glowMode = 0;
    } else
        actor->glowMode = 0;
    actor->chargeCollider.flags |= 4;
    actor->heading = 0;
    TetherActor_SetEntry(actor, 27);
}

void TetherActor_UpdateDirectedEntryTransition(struct TetherActor *p)
{
    u16 *q;
    u32 z, v, m;
    if (p->state != 0xF)
        TetherActor_SetState(p, 0xF);
    q = &p->chargeCollider.flags;
    v = *q;
    m = 4;
    z = 0;
    m |= v;
    *q = m;
    p->heading = z;
    if (TetherActor_SetEntry(p, 0x12))
        TetherActor_SetState(p, p->directedStep);
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_UpdateDirectedMovement(struct TetherActor *actor)
{
    if (actor->directedTimer == 30) {
        s32 dx = actor->position.x - actor->directedTarget.x;
        s32 dz = actor->position.z - actor->directedTarget.z;
        if (dx || dz) {
            u32 heading = ((((Math_Atan2(dx, dz) + 16) & 255) >> 5) + 3) & 7;
            actor->directedHeading = heading;
            actor->heading = heading;
        }
        actor->moving = 1;
    } else if (actor->directedTimer <= 29) {
        u32 angle;
        register const s16 *table asm("r2");
        {
            u32 value = (actor->directedHeading + 5) & 7;
            angle = value << 5;
        }
        table = gUnk_086149C4;
        actor->position.x += scale(table[(angle + 64) & 255] * 5);
        actor->position.z += scale(table[angle] * 5);
        actor->heading = actor->directedHeading;
    }
    if (--actor->directedTimer == 0) {
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        {
            u32 sample = ((const u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308];
            sample &= 63;
            actor->directedTimer = sample + 45;
        }
    }
}

void TetherActor_UpdateDirectedState(struct TetherActor *p)
{
    u16 *q;
    if (p->state != 0x10)
        TetherActor_SetState(p, 0x10);
    *&p->chargeCollider.flags &= ~4;
    q = &p->glowMode;
    *q = 1;
    TetherActor_SetEntry(p, 0);
    if ((gUnk_030053F4 & 0x200) == 0) {
        u32 v = p->interactionLock;
        if (v != 0) {
            TetherActor_UpdateLockedShake(p);
        } else if (p->chargeReady != 0) {
            *q = v;
            TetherActor_UpdateShake(p);
        } else {
            TetherActor_UpdateDirectedMovement(p);
        }
    }
}

// MATCHING_CONCESSION: register-pinning

void TetherActor_UpdateReverseDirectedMovement(struct TetherActor *actor)
{
    if (actor->directedTimer == 30) {
        s32 dx = actor->position.x - actor->directedTarget.x;
        s32 dz = actor->position.z - actor->directedTarget.z;
        if (dx || dz) {
            u32 heading = ((((Math_Atan2(dx, dz) + 16) & 255) >> 5) + 3) & 7;
            actor->directedHeading = heading;
            actor->heading = (heading + 4) & 7;
        }
    } else if (actor->directedTimer <= 29) {
        u32 angle;
        register const s16 *table asm("r2");
        {
            u32 value = (actor->directedHeading + 5) & 7;
            angle = value << 5;
        }
        table = gUnk_086149C4;
        actor->position.x += scale(table[(angle + 64) & 255] * 6);
        actor->position.z += scale(table[angle] * 6);
        actor->heading = ((u8)actor->directedHeading + 4) & 7;
        TetherActor_LimitPlayerDistance(actor);
        actor->moving = 1;
    }
    if (--actor->directedTimer == 0) {
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        {
            u32 sample = ((const u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308];
            sample &= 63;
            actor->directedTimer = sample + 45;
        }
    }
}

void TetherActor_UpdateReverseDirectedState(struct TetherActor *actor)
{
    if (actor->state != 17)
        TetherActor_SetState(actor, 17);
    actor->glowMode = 1;
    actor->position = actor->targetPosition;
    if (absolute_0804D270(actor->position.x - actor->targetPosition.x) > 2 ||
        absolute_0804D270(actor->position.z - actor->targetPosition.z) > 2)
        actor->moving = 1;
    actor->heading = actor->targetPosition.pad;
    if (actor->kind == 5 && actor->moving)
        TetherActor_SetHeadingEntry(actor, 36);
    else
        TetherActor_SetEntry(actor, 0);
    if (!(gUnk_030053F4 & 512)) {
        u32 value = actor->interactionLock;
        if (value)
            TetherActor_UpdateLockedShake(actor);
        else if (actor->chargeReady) {
            actor->glowMode = value;
            TetherActor_UpdateShake(actor);
        } else {
            TetherActor_UpdateReverseDirectedMovement(actor);
            TetherActor_UpdateSecondaryLock(actor);
        }
    }
}

extern void (*const gUnk_08605A10[])(struct TetherActor *);

s32 TetherActor_Tick(struct TetherActor *actor)
{
    actor->blendHold = 0;
    actor->requestStatus = 0;
    if (segmentsOnly()) {
        TetherActor_UpdateSegments(actor);
        return 0;
    }
    if (actor->destroyRequested) {
        Task_Kill(actor);
        return 0;
    }
    if (actor->kind == 9 && actor->requestMode == 0 && actor->onActionScriptId == 0 &&
        gUnk_02000580[0] && gUnk_02000580[0]->kind == 1 && gUnk_02000580[0]->tetherMode != 24) {
        Player_EnterKindFour(gUnk_02000580[0]);
        gUnk_02000580[0]->tetherMode = 36;
        gUnk_02000580[0]->tetherPhase = 0;
        gUnk_02000580[0]->tetherFrame = 0;
    }
    actor->shakeX = 0;
    actor->shakeZ = 0;
    actor->moving = 0;
    {
        u32 state = TetherActor_SelectNextState(actor);
        Callback_InvokeActor(actor, gUnk_08605A10[state]);
    }
    TetherActor_AdjustSlopeMovement(actor);
    TetherActor_UpdateWorldPosition(actor);
    TetherActor_UpdateChargeAndCooldown(actor);
    TetherActor_UpdateEffects(actor);
    TetherActor_UpdateSecondaryColliderValues(actor);
    actor->interactionProbe.position = &actor->position;
    sub_0821DE30(&actor->interactionProbe.bodyId);
    if (actor->destroyRequested)
        Task_Kill(actor);
    return 0;
}

struct TetherActor *TetherActor_Destroy(struct TetherActor *p)
{
    if (p->auxiliaryRegistered != 0)
        SceneActorContacts_RemoveNode(p->auxiliaryRegistration);
    SceneBody_Unregister(&p->bodyConfig);
    DisplayAnimation_Destroy(&p->display.animation);
    Collider_Remove(&p->chargeCollider);
    TetherActor_RemoveSegments(p);
    TetherActor_RemoveAuxiliarySprite(p);
    return gUnk_02000488 = 0;
}

s32 TetherActor_InitializeFromScript(struct TetherActor *actor, u32 entry, u32 unused)
{
    TetherActor_InitKindFromScript(actor, entry);
    TetherActor_InitDisplayFromScript(actor);
    TetherActor_InitBodyFromScript(actor);
    TetherActor_InitInteractionFromScript(actor);
    TetherActor_InitChargeCollider(actor);
    TetherActor_InitSecondaryCollider(actor);
    TetherActor_InitializeSegments(actor);
    TetherActor_InitStateParameters(actor);
    actor->glowMode = 0;
    actor->glowFrame = 0;
    actor->contactCooldown = 0;
    actor->parameter = -1;
    actor->destroyRequested = 0;
    actor->blendRequested = 0;
    actor->actionRequested = 0;
    actor->actionAngle = 0;
    actor->actionMagnitude = 0;
    actor->requestStatus = 0;
    if (actor->kind == 9) {
        actor->requestMode = Script_GetKeywordValue('t', 0);
        actor->onActionScriptId = Script_GetKeywordValue('b', 0);
    } else
        actor->requestMode = 0;
    gUnk_02000488 = actor;
    return 0;
}

struct TetherActor *TetherActor_CreateFromScript(u32 a, u32 b)
{
    struct TetherActor *r;
    if (gUnk_02000488 != 0)
        return gUnk_02000488;
    r = Task_Create(9, 0x484);
    if (r) {
        Task_SetCallbacks(r, (void (*)(void *))TetherActor_Tick,
                          (void (*)(void *))TetherActor_Destroy);
        if (TetherActor_InitializeFromScript(r, a, b) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
