#include "scene_actor_utilities.h"

void SceneActorAnimation_InitEnemySequence(struct SceneActorSequenceDrawable *drawable, u16 b)
{
    void *resource = ScriptProc_Start(0x922E, b);
    drawable->sequenceResource = resource;
    Anim_SetSequence(&drawable->drawable.sequence, resource, 0, 0, 0);
}

u8 SceneActorTerrain_CheckNeighborHeight(s32 direction, s32 cell, struct ContactPosition *position)
{
    struct ContactPosition center;
    u32 ceiling = position->y + 255;
    switch (direction) {
        case 0:
            cell++;
            SceneActorTerrain_GetCenter(cell, &center);
            break;
        case 1:
            cell += SceneActorTerrain_Width();
            SceneActorTerrain_GetCenter(cell, &center);
            break;
        case 2:
            cell--;
            SceneActorTerrain_GetCenter(cell, &center);
            break;
        case 3:
            cell -= SceneActorTerrain_Width();
            SceneActorTerrain_GetCenter(cell, &center);
            break;
    }
    if ((u32)SceneActorTerrain_GetHeight(cell, &center) > ceiling)
        return 0;
    return 1;
}

s32 SceneActorTerrain_FindReachableCell(struct ContactPosition *position, s32 direction,
                                        s32 maxCells)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    s32 startCell, i, startHeight, cell;

    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        startCell = 0;
    else
        startCell = gTerrainMap->rows[z] + x;
    startHeight = (*SceneActorTerrain_GetCell(startCell) & 15) << 8;
    cell = startCell;
    switch (direction) {
        case 0:
            for (i = 0; i < maxCells; i++) {
                if (startHeight - ((*SceneActorTerrain_GetCell(cell) & 15) << 8) < 0)
                    return cell - 1;
                cell++;
            }
            cell -= 1;
            return cell;
        case 1:
            for (i = 0; i < maxCells; i++) {
                if (startHeight - ((*SceneActorTerrain_GetCell(cell) & 15) << 8) < 0)
                    return cell - SceneActorTerrain_Width();
                cell += SceneActorTerrain_Width();
            }
            cell -= SceneActorTerrain_Width();
            return cell;
        case 2:
            for (i = 0; i < maxCells; i++) {
                if (startHeight - ((*SceneActorTerrain_GetCell(cell) & 15) << 8) < 0)
                    return cell + 1;
                cell--;
            }
            cell += 1;
            return cell;
        case 3:
            for (i = 0; i < maxCells; i++) {
                if (startHeight - ((*SceneActorTerrain_GetCell(cell) & 15) << 8) < 0)
                    return cell + SceneActorTerrain_Width();
                cell -= SceneActorTerrain_Width();
            }
            cell += SceneActorTerrain_Width();
            return cell;
    }
    return startCell;
}

static inline s32 SceneTerrain_RoundDistance(s32 x)
{
    if (x < 0)
        x = -x;
    x += 128;
    if (x >= 0)
        return x >> 8;
    return -((-x) >> 8);
}

s32 SceneTerrain_IsPathBlocked(struct ContactPosition *a, struct ContactPosition *b, s32 direction,
                               s32 targetCell)
{
    s32 count;
    switch (direction) {
        case 0:
        case 2:
            count = SceneTerrain_RoundDistance(a->x - b->x) + 1;
            break;
        case 1:
        case 3:
            count = SceneTerrain_RoundDistance(a->z - b->z) + 1;
            break;
        default:
            count = 0;
            break;
    }
    if (SceneActorTerrain_FindReachableCell(a, direction, count) != targetCell)
        return 1;
    return 0;
}

bool32 SceneActorMotion_AimTowardTarget(struct ContactPosition *from, struct ContactPosition *to,
                                        struct ContactPosition *vel, u8 *angleOut, u32 rangeX,
                                        u32 rangeZ, s32 speed)
{
    u16 ax = to->x - from->x, az = to->z - from->z;
    u8 angle = Math_Atan2((s16)ax, (s16)az);
    s32 d;
    vel->x = (gUnk_086149C4[(angle + 0x40) & 0xff] * speed) >> 16;
    vel->z = (gUnk_086149C4[angle] * speed) >> 16;
    *angleOut = angle;
    d = from->x - to->x;
    if (d < 0)
        d = -d;
    if (d <= rangeX) {
        d = from->z - to->z;
        if (d < 0)
            d = -d;
        if (d <= rangeZ)
            return TRUE;
    }
    return FALSE;
}

s16 Math_GetVectorLength(s16 x, s16 y)
{
    u8 angle = Math_Atan2(x, y);
    s32 result;
    if (angle == 64 || angle == 192) {
        s32 vertical = y;
        if (vertical < 0)
            vertical = -vertical;
        return vertical;
    } else {
        result = Div(x << 12, gUnk_086149C4[(angle + 64) & 255]);
        if (result < 0)
            result = -result;
    }
    return result;
}

static inline void ClearFlags16(u16 *f, u32 mask)
{
    *f &= mask;
}

void SceneActorRoute_ClampTargetToAnchor(struct SceneActorRegistryResetView *actor,
                                         struct ContactPosition *target)
{
    struct SceneActorBody *body = &actor->body;
    s32 dx = (u16)((struct SceneActorRouteCursor *)body->currentRoute)->point->x - body->position.x;
    s32 dz = (u16)((struct SceneActorRouteCursor *)body->currentRoute)->point->z - body->position.z;
    s32 len = (s16)Math_GetVectorLength((s16)dx, (s16)dz);
    void *mesh;
    struct ContactPosition *pos;
    s32 r;
    dx = target->x - body->position.x;
    dz = target->z - body->position.z;
    if (len <= (s16)Math_GetVectorLength((s16)dx, (s16)dz)) {
        target->x = (u16)((struct SceneActorRouteCursor *)body->currentRoute)->point->x;
        target->y = 0;
        target->z = (u16)((struct SceneActorRouteCursor *)body->currentRoute)->point->z;
    }
    mesh = actor->group.route;
    pos = &actor->body.position;
    if (!Nav_IsPositionInMesh(mesh, pos, target)) {
        ClearFlags16(&actor->flags2D4, ~0x200);
    } else {
        r = Terrain_ApplyBlocked(mesh, pos, target);
        if (r != 0) {
            if (r != 2)
                Memory_Clear(&actor->body.motion, 8);
        } else {
            ClearFlags16(&actor->flags2D4, ~0x200);
        }
    }
}

static inline s32 FacingDelta(s16 target, u16 current)
{
    return target - current;
}

void SceneActor_SetPositionXZ(struct SceneActorFacingBody *actor, s32 x, s32 y)
{
    s32 dx = FacingDelta(x, actor->x);
    s32 dy = FacingDelta(y, actor->z);
    actor->facing = Math_Atan2((s16)dx, (s16)dy);
}

void SceneActorReaction_ResetResponseState(struct SceneActorResponseState *state, const void *table,
                                           u32 index)
{
    u32 z = 0;
    state->state = z;
    state->phase = z;
    state->behavior = z;
    state->threshold = z;
    state->control = z;
}

static inline u32 Random(void)
{
    u16 *table = gRandomTable;
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)table);
}

void SceneActorRoute_SelectRandomNextPoint(struct SceneActorRandomRoute *route)
{
    s32 previous = route->cursor.pointIndex;
    s32 i;
    s32 next;

    for (i = 0; i < 4; i++) {
        next = Mod(Random(), route->cursor.definition->pointCount);
        if (next != previous && next != route->previousIndex)
            break;
    }
    if (next == previous || next == route->previousIndex) {
        next = previous + 1;
        if (next >= route->cursor.definition->pointCount)
            next = 0;
    }
    route->previousIndex = previous;
    route->cursor.pointIndex = next;
    route->cursor.unknown3 = 0;
    route->cursor.point =
        (void *)(Path_GetPointData(route->cursor.definition) + route->cursor.pointIndex * 8);
}

static inline struct SceneCell *CellProperties(s32 cell)
{
    return (struct SceneCell *)SceneActorTerrain_GetCell(cell);
}

static inline s32 Absolute(s32 value)
{
    if (value < 0)
        value = -value;
    return value;
}

u8 SceneActorTerrain_CanTraverseCell(s32 startCell, s32 cell, s32 startHeight, u32 startShape)
{
    struct ContactPosition center;
    u32 shape;
    s32 height;
    s32 allowHeightStep;

    /* The original query samples slopes before the position is filled. */
    shape = CellProperties(cell)->height >> 4;
    height = SceneActorTerrain_GetHeight(cell, &center);
    SceneActorTerrain_GetCenter(cell, &center);
    allowHeightStep = 0;
    if ((shape == 2 && startShape == 0) || (startShape == 2 && shape == 0)) {
        if (Absolute(startCell - cell) == 1)
            allowHeightStep = 1;
    } else if ((shape == 1 && startShape == 0) || (startShape == 1 && shape == 0)) {
        if (Absolute(startCell - cell) == SceneActorTerrain_Width())
            allowHeightStep = 1;
    } else if ((shape == 1 && startShape == 1) || (shape == 2 && startShape == 2))
        allowHeightStep = 1;

    {
        u32 flags = CellProperties(cell)->flags;
        if (flags & 0x43)
            return 0;
    }
    if (allowHeightStep) {
        s32 difference;
        height >>= 8;
        startHeight >>= 8;
        difference = Absolute(height - startHeight);
        if (difference > 1)
            return 0;
    } else if (height != startHeight)
        return 0;
    return 1;
}

static inline s32 CellAt(struct ContactPosition *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

u8 SceneActorTerrain_IsBelowForwardFloor(struct SceneActorBody *body)
{
    struct ContactPackedVector offset;
    struct ContactPackedVector point;
    s32 cell;
    s32 height;
    if (body->motion.x != 0) {
        if (body->motion.x < 0) {
            offset.x = -32;
            offset.z = 0;
        } else {
            offset.x = 32;
            offset.z = 0;
        }
    } else if (body->motion.z != 0) {
        if (body->motion.z < 0) {
            offset.x = 0;
            offset.z = -32;
        } else {
            offset.x = 0;
            offset.z = 32;
        }
    } else
        return 0;
    point.x = body->position.x + body->motion.x + offset.x;
    point.y = body->position.y + body->motion.y;
    point.z = body->position.z + body->motion.z + offset.z;
    cell = CellAt((struct ContactPosition *)&point);
    height = (CellProperties(cell)->height & 15) << 8;
    if (body->position.y < height)
        return 1;
    return 0;
}

u8 InventorySlot_PickRandom(s16 *slot, s16 *kind, s16 *param)
{
    u16 start;
    s16 n;
    s16 idx;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    start = Mod(*(u16 *)((u8 *)gRandomTable + gRandomIndex * 2), 16);
    for (n = 0; n < 16; n++) {
        idx = (s16)start;
        *kind = Inventory_GetItemId(idx);
        if (*kind >= 0 && *kind != 9)
            goto found;
        start = Mod(idx + 1, 16);
    }
    return 0;
found:
    *slot = start;
    if (idx) {
        *param = Inventory_GetItemPackedState(*slot);
        return 1;
    } else {
        *param = Inventory_GetItemPackedState(*slot);
        return 1;
    }
}

static inline s32 ActorParticles_NextRandom(void)
{
    const u16 *table = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}

void ActorParticles_SpawnDirectionalBurst(struct SceneActorRegistryResetView *actor, s32 amount)
{
    struct SceneExtent velocity, spread;
    u32 count;
    u32 angle;
    if (amount > 0) {
        angle = (actor->field2B6 + 128) & 255;
        velocity.x = gUnk_086149C4[(angle + 64) & 255] >> 10;
        velocity.y = ActorParticles_NextRandom() & 7;
        velocity.z = gUnk_086149C4[angle & 255] >> 10;
        spread.x = (ActorParticles_NextRandom() & 7) + 3;
        spread.y = (ActorParticles_NextRandom() & 7) + 3;
        spread.z = (ActorParticles_NextRandom() & 7) + 3;
        count = (ActorParticles_NextRandom() & 1) + 2;
        sub_08014634(count, ActorMode_SelectOddStateVariant(actor),
                     (struct SceneExtent *)&actor->body.position, &velocity, &spread, 45, 31);
    }
}

static inline s32 ActorParticles_FirstRandom(void)
{
    const u16 *table = (const u16 *)RANDOM_TABLE_ADDR;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}

static inline s32 ActorParticles_Half(s16 n)
{
    if (n >= 0)
        return n >> 1;
    return (s16) - ((-n) >> 1);
}

void ActorParticles_SpawnBurst(struct SceneActorRegistryResetView *actor)
{
    struct SceneExtent velocity, spread;
    u32 count;
    u32 sampleOrigin;
    velocity.x = ActorParticles_Half(actor->body.motion.x);
    PARTICLE_RANDOM_CAPTURE_ORIGIN(sampleOrigin);
    velocity.y = ActorParticles_FirstRandom() & 7;
    velocity.z = ActorParticles_Half(actor->body.motion.z);
    spread.x = (ParticleRandom_ReadRelativeSample(sampleOrigin, gRandomTable) & 3) + 15;
    spread.y = (ParticleRandom_ReadRelativeSample(sampleOrigin, gRandomTable) & 7) + 15;
    spread.z = (ParticleRandom_ReadRelativeSample(sampleOrigin, gRandomTable) & 3) + 15;
    count = (ParticleRandom_ReadRelativeSample(sampleOrigin, gRandomTable) & 3) + 4;
    sub_08014634(count, ActorMode_SelectOddStateVariant(actor),
                 (struct SceneExtent *)&actor->body.position, &velocity, &spread, 45, 31);
}

u32 ActorMode_SelectOddStateVariant(struct SceneActorRegistryResetView *actor)
{
    u32 result;
    switch (actor->paletteOffset) {
        case 1:
        case 3:
        case 5:
            result = 3;
            break;
        case 0:
        default:
            result = 4;
            break;
    }
    return result;
}

void ActorParticles_SpawnWideBurst(struct SceneActorRegistryResetView *actor)
{
    struct SceneExtent velocity, spread;
    u32 count;
    velocity.x = -12;
    velocity.y = ActorParticles_NextRandom() & 7;
    velocity.z = -12;
    spread.x = 24;
    spread.y = (ActorParticles_NextRandom() & 7) + 3;
    spread.z = 24;
    count = (ActorParticles_NextRandom() & 3) + 4;
    sub_08014634(count, ActorMode_SelectOddStateVariant(actor),
                 (struct SceneExtent *)&actor->body.position, &velocity, &spread, 60, 63);
}
