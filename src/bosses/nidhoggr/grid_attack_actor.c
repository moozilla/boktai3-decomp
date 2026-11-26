#include "engine/math/hit_spark_random.h"
#include "bosses/nidhoggr/nidhoggr_duel.h"
#include "player/cutscene/player_cue_sequence.h"
#include "player/player.h"
#include "player/player_slot_status.h"
#include "script/demo_manager.h"
#include "bosses/nidhoggr/nidhoggr_duel_burst.h"
#include "effects/sprites/projected_display.h"

void NidhoggrDuel_RemoveSprite(struct GridAttackActor *p)
{
    Sprite_Remove(&p->drawable);
}

void Anim_RestartSequence(void *, void *, int, int, int);

void Collider_SetAttackProperties(void *, int, int, int, int, int);

void Collider_SetBodyProperties(void *, int, int, int);

void NidhoggrDuel_OnDamageCollision(struct ContactCollider *, struct ContactCollider *,
                                       struct GridAttackActor *);

void NidhoggrDuelMinionPool_SpawnActor(void *, void *, int);

int sub_08249350(int, int);

void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);

void SparkParticle_Spawn(void *, int, void *, int, int, int, int, int);

void NidhoggrDuelOrb_BeginExpansion(void *);

void NidhoggrDuel_SpawnTargetedProjectile(struct GridAttackActor *, struct ContactPosition *,
                                             int);

void NidhoggrDuelMinionPool_BeginAllDeaths(void *);

void SceneActors_RunFirstEventScript(void *, void *);

void Bestiary_UnlockEntry(int);

void SceneActors_RunSecondEventScript(void *, void *);

u8 Sprite_BlinkUntilExpired(int *, int, int, int);

void SceneActors_RunStateCallback(u32);

int NidhoggrDuelMinionPool_GetActiveCount(void *);

void NidhoggrDuel_ProcessMessages(struct GridAttackActor *p);
void NidhoggrDuel_UpdateAnimationAndTargets(struct GridAttackActor *p);

void NidhoggrDuel_UpdateCollidersAndPalette(struct GridAttackActor *p);

void SceneActors_Destroy(struct GridAttackActor *);

struct GridAttackActor *SceneActors_GetBySlot(u32);

s32 NidhoggrDuel_Initialize(struct GridAttackActor *, u32);

void NidhoggrDuel_SetAnimation(struct GridAttackActor *p, int sequence, int start, int speed)
{
    p->animationMode = 0;
    p->animationSpeed = speed;
    if (start) {
        Anim_RestartSequence(&p->animation, p->resource, (u16)sequence, p->animationMode,
                             p->animationSpeed);
        p->animationEnded = 0;
    } else
        Anim_SetSequence(&p->animation, p->resource, (u16)sequence, p->animationMode,
                         p->animationSpeed);
}

void Collision_CalculateDamageResponse(void *, void *);
void PlayerStats_AddContactHundredths(struct ContactCollider *, struct ContactCollider *, s32);

void NidhoggrDuel_OnDamageCollision(struct ContactCollider *source,
                                       struct ContactCollider *other, struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    Collision_CalculateDamageResponse(source, other);
    PlayerStats_AddContactHundredths(source, other, p->contactReward);
    p->damage = other->cost;
    if (ContactCollider_TestFlags(source, 0x100))
        p->damageAngle = source->direction;
    else
        p->damageAngle = Math_Atan2(other->x - source->x, other->z - source->z);
    {
        u32 value = source->statRewardFlags;
        base->contactSource = value;
        value = source->reactionFlags;
        base->contactFlags = value;
    }
    PlayerStats_AddContactHundredths(source, other, base->contactRewardParameter);
    if (ContactCollider_TestReactionFlags(source, 4))
        p->special = 1;
}

void NidhoggrDuel_OnBodyCollision(void)
{
}

void NidhoggrDuel_InitBodyCollider(struct GridAttackActor *p)
{
    void *collider = &p->bodyCollider;
    struct GridAttackPacked size, offset;
    size.x = 64;
    size.y = 128;
    size.z = 64;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2401, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &p->drawable.position, 0);
    Collider_SetCallback(collider, NidhoggrDuel_OnBodyCollision, p);
    Collider_SetAttackProperties(collider, p->attackPower, p->attackKind, 0, 0, p->attackParameter);
}

void NidhoggrDuel_InitDamageCollider(struct GridAttackActor *p, int mode)
{
    void *collider = &p->damageCollider;
    struct GridAttackPacked size, offset;
    size.x = 96;
    size.y = 128;
    size.z = 96;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(collider, 0, 0x5101, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &p->drawable.position, 0);
    Collider_SetCallback(collider, NidhoggrDuel_OnDamageCollision, p);
    Collider_SetBodyProperties(collider, mode, 0x808, 4);
    Collider_Refresh(collider);
}

void NidhoggrDuel_GetGridPosition(s16 *p, int x, int z)
{
    struct SceneVec *position = (struct SceneVec *)p;
    if (x != 1 || z) {
        position->x = (x << 8) + 0xa80;
        {
            position->y = 256;
        }
        position->z = (z << 8) + 0xa80;
    }
}

void NidhoggrDuel_SpawnGridContact(struct GridAttackActor *p, int x, int z)
{
    struct SceneVec pos;
    NidhoggrDuel_GetGridPosition((s16 *)&pos, x, z);
    NidhoggrDuelMinionPool_SpawnActor(p->contactPool, &pos, x + z * 3);
}

static inline int GridAttack_RandomColumn(u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return sub_08249350(table[gRandomIndex], 3);
}

void NidhoggrDuel_SpawnGridPattern(struct GridAttackActor *p)
{
    if (p->targetPositions[0]->z < 0xc00) {
        NidhoggrDuel_SpawnGridContact(p, 0, 0);
        NidhoggrDuel_SpawnGridContact(p, 2, 0);
        NidhoggrDuel_SpawnGridContact(p, 0, 1);
        NidhoggrDuel_SpawnGridContact(p, 1, 1);
        NidhoggrDuel_SpawnGridContact(p, 2, 1);
        {
            u16 *table = gRandomTable;
            NidhoggrDuel_SpawnGridContact(p, GridAttack_RandomColumn(table), 2);
            NidhoggrDuel_SpawnGridContact(p, GridAttack_RandomColumn(table), 3);
            NidhoggrDuel_SpawnGridContact(p, GridAttack_RandomColumn(table), 4);
        }
    } else {
        NidhoggrDuel_SpawnGridContact(p, 0, 1);
        NidhoggrDuel_SpawnGridContact(p, 1, 1);
        NidhoggrDuel_SpawnGridContact(p, 2, 1);
        {
            u16 *table = gRandomTable;
            NidhoggrDuel_SpawnGridContact(p, GridAttack_RandomColumn(table), 2);
            NidhoggrDuel_SpawnGridContact(p, GridAttack_RandomColumn(table), 3);
            NidhoggrDuel_SpawnGridContact(p, GridAttack_RandomColumn(table), 4);
        }
    }
}

static inline void GridAttack_SetState(struct GridAttackActor *p, void (*fn)(void *), int id)
{
    p->state = id;
    p->callback = fn;
    p->timer = 0;
    p->changed = 1;
    p->stateReset = 0;
    p->pending = 0;
}

static inline void GridAttack_StartActionMode(struct GridAttackActor *p, int mode, int zero, int id)
{
    p->active = mode;
    p->actionReset = zero;
    p->action = id;
    p->actionTimer = zero;
    p->entered = 1;
}

void *BouncingObject_Spawn(void *, void *, void *, s32, s32, s32, s32);
static inline u32 GridAttack_RandomValue(void)
{
    const u16 *table = gRandomTable;
    u32 *index = &gRandomIndex;
    *index = (*index + 1) & 1023;
    return table[*index];
}
static inline void GridAttack_OffsetCoordinate(volatile s16 *coordinate, u32 random)
{
    u16 value = *coordinate;
    u32 rounded = ~255 & value;
    u8 center = 128;
    *coordinate = (rounded + ((random & 63) + center)) - 32;
}
void NidhoggrDuel_SpawnTargetedProjectile(struct GridAttackActor *actor,
                                             struct ContactPosition *point, int kind)
{
    struct ContactPosition velocity;
    struct SceneVec position;
    struct ContactPosition target;
    struct ContactPosition original;
    struct ContactPosition *vel = &velocity;
    s16 *pos;
    ZeroFill8(vel);
    pos = (s16 *)&position;
    original = *point;
    *(struct ContactPosition *)&position = original;
    GridAttack_OffsetCoordinate(&pos[0], GridAttack_RandomValue());
    pos[1] += 1024;
    GridAttack_OffsetCoordinate(&pos[2], GridAttack_RandomValue());
    {
        struct ContactPosition *dest = &target;
        *dest = original;
        BouncingObject_Spawn(&position, vel, dest, actor->projectileDamage, 50, kind, 0);
    }
}

int NidhoggrDuel_OnDamage(struct GridAttackActor *p, int kind, int damage)
{
    /* Keep the transform base live: direct actor fields change register allocation. */
    struct GridAttackTransform *position = &p->transform;
    struct GridAttackPacked size;
    switch (kind) {
        case 0:
            if (damage > 0) {
                size.x = 32;
                size.y = 0;
                size.z = 32;
                DamageNumberDisplay_Show(&p->damageCollider, damage, 0, &position->coordinates,
                                         &size, 1);
            }
            GridAttack_StartActionMode(p, 1, 0, 0);
            GridAttack_SetState(p, NidhoggrDuel_UpdateDamageHold, 5);
            break;
        case 2:
            GridAttack_StartActionMode(p, 1, 0, 1);
            GridAttack_SetState(p, NidhoggrDuel_UpdateDamageRecovery, 6);
            {
                int angle = position->angle;
                p->damageAngle = angle;
            }
            break;
        case 1: {
            int angle = (u8)p->players[0]->unk2D;
            u8 *dst = &p->damageAngle;
            int zero = 0;
            *dst = angle;
            GridAttack_AddFlags(p, 2);
            GridAttack_StartActionMode(p, 1, zero, 3);
            GridAttack_SetState(p, NidhoggrDuel_UpdateDeathState, 8);
        } break;
    }
    return 1;
}

void NidhoggrDuel_ResetAttackAnimationHistory(struct GridAttackActor *p)
{
    p->attackAnimationHistory = 0;
}

int NidhoggrDuel_SelectAttackAnimation(struct GridAttackActor *p)
{
    int start, i, index;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    {
        u32 offset = gRandomIndex * 2;
        {
            int value = *(u16 *)((u8 *)gRandomTable + offset);
            value &= 3;
            start = value;
        }
    }
    for (i = start; i < start + 4; i++) {
        index = i & 3;
        if (!((p->attackAnimationHistory >> index) & 1)) {
            p->attackAnimationHistory |= 1 << index;
            goto done;
        }
    }
    index = start;
    {
        int mask = 1 << index;
        p->attackAnimationHistory = mask;
    }
done:
    return index + 4;
}

void NidhoggrDuel_EmitSpriteParticles(struct GridAttackActor *p)
{
    struct ContactSwarmSprite *graphics = &p->drawable;
    struct GridAttackPacked size;
    int i;
    for (i = 0; i < 2; i++) {
        u32 angle, count;
        u32 index;
        u32 *indexPtr;
        u32 value;
        u32 speed;
        size.x = 164;
        size.y = 16;
        size.z = 164;
        indexPtr = &gRandomIndex;
        value = (u32)gRandomTable;
        index = HitSpark_NextIndex(*indexPtr, 1023);
        angle = (u8)HitSpark_Sample(value, index);
        index = HitSpark_NextIndex(index, 1023);
        count = (HitSpark_Sample(value, index) & 3) + 2;
        index = HitSpark_NextIndex(index, 1023);
        *indexPtr = index;
        speed = (HitSpark_Sample(value, index) & 3) + 20;
        value = 3;
        SparkParticle_Spawn(&graphics->position, value, &size, angle, count, speed, 2, 2);
    }
}

void NidhoggrDuel_RequestLinkedActorState(struct GridAttackActor *p)
{
    struct SceneEventCommand *packet = &p->packet;
    if (p->linkedActorId && p->linkedActorPhase != 0) {
        p->linkedActorPhase = 0;
        packet->id = p->linkedActorId;
        packet->receiverKind = 5;
        packet->trigger = 0;
        packet->unknown4 = 0;
        packet->kind = 1;
        packet->argumentCount = 1;
        packet->value = 3;
        SceneMessages_Send(packet);
    }
}

void NidhoggrDuel_NotifyLinkedActorLatePhase(struct GridAttackActor *p)
{
    struct SceneEventCommand *packet = &p->packet;
    if (p->linkedActorId && p->linkedActorPhase != 1) {
        p->linkedActorPhase = 1;
        packet->id = p->linkedActorId;
        packet->receiverKind = 5;
        packet->trigger = 0;
        packet->unknown4 = 0;
        packet->kind = 92;
        packet->argumentCount = 0;
        SceneMessages_Send(packet);
    }
}

static inline void GridAttack_SetFacing(struct GridAttackActor *p, int angle)
{
    u32 value;
    s32 shifted;
    p->transform.angle = angle;
    shifted = (p->transform.angle + 96) >> 6;
    value = 3;
    value &= shifted;
    p->facing = value;
    shifted = (p->transform.angle + 112) >> 5;
    value = 7;
    value &= shifted;
    p->facing8 = value;
}

void NidhoggrDuel_UpdateIdle(struct GridAttackActor *p)
{
    if (!p->locked)
        GridAttack_SetFacing(p, p->requestedAngles[0]);
    if (p->changed) {
        p->changed = 0;
        if (p->locked)
            GridAttack_SetFacing(p, p->savedAngle);
        NidhoggrDuel_SetAnimation(p, 2, 1, 0);
    }
    if (!p->tryUpdate(p)) {
        GridAttack_AddFlags(p, 64);
        p->reset(p);
    }
}

void NidhoggrDuel_UpdateGridAttack(struct GridAttackActor *p, int tick)
{
    if (p->changed) {
        p->changed = 0;
        GridAttack_SetFacing(p, p->requestedAngles[0]);
        NidhoggrDuel_ResetAttackAnimationHistory(p);
        NidhoggrDuel_SetAnimation(p, NidhoggrDuel_SelectAttackAnimation(p), 1, 8);
        Sound_PlaySongChecked(1355);
    }
    if (p->tryUpdate(p))
        return;
    GridAttack_AddFlags(p, 64);
    if (p->reset(p))
        return;
    if (tick == 8)
        NidhoggrDuel_SpawnGridPattern(p);
    if (p->animationEnded) {
        NidhoggrDuel_SetAnimation(p, NidhoggrDuel_SelectAttackAnimation(p), 1, 8);
        if (tick > 31)
            p->finished = 1;
    }
}

void NidhoggrDuel_UpdateElevatedGridAttack(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    if (p->changed) {
        p->changed = 0;
        GridAttack_SetFacing(p, p->requestedAngles[0]);
        NidhoggrDuel_SetAnimation(p, 3, 1, 8);
        p->stage = 0;
        p->counter = 0;
        NidhoggrDuel_ResetAttackAnimationHistory(p);
        Sound_PlaySongChecked(955);
    }
    p->phaseTimer = 240;
    p->damage = 0;
    p->special = 0;
    GridAttack_AddFlags(p, 12);
    if (p->reset(p))
        return;
    switch (p->stage) {
        case 0:
            p->counter++;
            ProjectedDisplay_SetHeight(p->projectedDisplay, p->counter * 4);
            if (p->counter > 31) {
                p->stage++;
                p->counter = 0;
                NidhoggrDuelOrb_BeginExpansion(p->orbitContact);
            }
            break;
        case 1:
            p->counter++;
            if (p->counter > 3) {
                p->stage++;
                p->counter = 0;
            }
            break;
        case 2:
            if (p->counter == 10)
                Sound_PlaySongChecked(1358);
            if (p->counter > 129) {
                p->stage++;
                p->counter = 0;
                NidhoggrDuel_SetAnimation(p, 3, 1, 12);
            }
            break;
        case 3:
            if (p->counter == 6)
                NidhoggrDuel_SpawnGridPattern(p);
        case 4:
        case 5:
        case 6:
            if (p->counter > 5) {
                p->stage++;
                p->counter = 0;
                NidhoggrDuel_SetAnimation(p, NidhoggrDuel_SelectAttackAnimation(p), 1, 8);
            }
            break;
        case 7:
            if (p->counter > 9) {
                p->stage++;
                p->counter = 0;
                Sound_PlaySongChecked(955);
            }
            break;
        case 8:
            p->counter++;
            ProjectedDisplay_SetHeight(p->projectedDisplay, (32 - p->counter) * 4);
            if (p->counter > 31) {
                p->stage++;
                p->counter = 0;
            }
            break;
        case 9:
            if (p->counter > 9) {
                p->stage++;
                p->counter = 0;
                base->finished = 1;
            }
            break;
    }
    p->counter++;
}

extern u16 gUnk_086141BC[];

void NidhoggrDuel_UpdateTargetedAttack(struct GridAttackActor *p, int tick)
{
    struct GridAttackActor *base = p;
    if (p->changed) {
        p->changed = 0;
        GridAttack_SetFacing(p, p->requestedAngles[0]);
        NidhoggrDuel_ResetAttackAnimationHistory(p);
        NidhoggrDuel_SetAnimation(p, NidhoggrDuel_SelectAttackAnimation(p), 1, 8);
        gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
        {
            int random = gUnk_086141BC[gEffectRandomTableIndex];
            if (random <= 32767)
                Sound_PlaySongChecked(1356);
        }
    }
    if (p->tryUpdate(p))
        return;
    GridAttack_AddFlags(p, 64);
    if (p->reset(p))
        return;
    if (tick == 8)
        NidhoggrDuel_SpawnTargetedProjectile(
            base, (struct ContactPosition *)p->targetPositions[0], base->targetAttackParameter);
    if (p->animationEnded)
        p->finished = 1;
}

void NidhoggrDuel_UpdateDamageHold(struct GridAttackActor *p)
{
    if (p->changed) {
        p->changed = 0;
        NidhoggrDuel_SetAnimation(p, 8, 1, 8);
    }
    p->damage = 0;
    p->special = 0;
    GridAttack_AddFlags(p, 12);
    GridAttack_AddFlags(p, 64);
}

void NidhoggrDuel_UpdateDamageRecovery(struct GridAttackActor *p, int tick)
{
    if (p->changed) {
        p->changed = 0;
        NidhoggrDuel_SetAnimation(p, 8, 1, 8);
    }
    p->damage = 0;
    p->special = 0;
    GridAttack_AddFlags(p, 12);
    if (!p->reset(p) && tick > 19)
        p->finished = 1;
}

void NidhoggrDuel_UpdateStaggerState(struct GridAttackActor *p)
{
    if (p->changed) {
        p->changed = 0;
        NidhoggrDuel_SetAnimation(p, 8, 1, 8);
        NidhoggrDuelMinionPool_BeginAllDeaths(p->contactPool);
        Sound_PlaySongChecked(1359);
    }
    p->reactionCooldown = 6;
    p->damage = 0;
    p->special = 0;
    GridAttack_AddFlags(p, 12);
    if (p->animationEnded)
        GridAttack_SetState((void *)p, NidhoggrDuel_UpdateIdle, 1);
    else
        p->flags |= 1;
}

static inline void GridAttack_SetDeathEffect(struct GridAttackActor *p, int mode, int id,
                                             int duration)
{
    p->effectId = id;
    p->effectMode = mode;
    p->effectDuration = duration;
    if (p->effectMode == 2) {
        u32 value = duration == 5 ? 1 : 2;
        p->effectVariant = value;
    } else if (p->effectMode == 1)
        p->effectVariant = 0;
}

void NidhoggrDuel_UpdateDeathState(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    if (p->changed) {
        p->changed = 0;
        NidhoggrDuel_SetAnimation(p, 10, 1, 8);
        SceneActors_RunFirstEventScript(p, &p->drawable.position);
        NidhoggrDuelMinionPool_BeginAllDeaths(p->contactPool);
        GridAttack_SetDeathEffect(p, 2, 349, 1);
        base->stage = 0;
        base->counter = 0;
        Sound_PlaySongChecked(958);
    }
    if (p->animationEnded && !base->stage) {
        Bestiary_UnlockEntry(80);
        SceneActors_RunSecondEventScript(p, &base->drawable.position);
        base->stage++;
    }
    GridAttack_AddFlags(p, 13);
}

void NidhoggrDuel_UpdateHiddenState(struct GridAttackActor *p)
{
    if (p->changed)
        p->changed = 0;
    p->damage = 0;
    p->special = 0;
    GridAttack_AddFlags(p, 28);
    p->reset(p);
}

void NidhoggrDuel_UpdateIdleState(struct GridAttackActor *s)
{
    if (s->changed) {
        s->changed = 0;
        NidhoggrDuel_SetAnimation(s, 2, 1, 0);
    }
    {
        u32 *p = (u32 *)&s->damage;
        *p = 0;
        s->special = 0;
    }
    GridAttack_AddFlags(s, 12);
    Callback_InvokeActor(s, (void *)s->reset);
}

void NidhoggrDuel_UpdateAnimationNineState(struct GridAttackActor *p)
{
    /* Keep the transform base live: direct actor fields change register allocation. */
    struct GridAttackTransform *position = &p->transform;
    if (p->changed) {
        p->changed = 0;
        NidhoggrDuel_SetAnimation(p, 9, 1, 8);
        p->savedPosition = position->coordinates;
    }
    p->damage = 0;
    p->special = 0;
    GridAttack_AddFlags(p, 12);
    if (!p->reset(p)) {
        p->reactionCooldown = 6;
        if (p->animationEnded)
            p->finished = 1;
    }
}

void NidhoggrDuel_UpdateAnimationElevenState(struct GridAttackActor *s)
{
    if (s->changed) {
        s->changed = 0;
        NidhoggrDuel_SetAnimation(s, 11, 1, 8);
    }
    {
        u32 *p = (u32 *)&s->damage;
        *p = 0;
        s->special = 0;
    }
    GridAttack_AddFlags(s, 12);
    Callback_InvokeActor(s, (void *)s->reset);
}

void NidhoggrDuel_UpdateAlternateIdleState(struct GridAttackActor *s)
{
    if (s->changed) {
        s->changed = 0;
        NidhoggrDuel_SetAnimation(s, 2, 1, 0);
    }
    {
        u32 *p = (u32 *)&s->damage;
        *p = 0;
        s->special = 0;
    }
    GridAttack_AddFlags(s, 12);
    Callback_InvokeActor(s, (void *)s->reset);
}

void NidhoggrDuel_UpdateRandomAnimationState(struct GridAttackActor *p, int tick)
{
    if (p->changed) {
        p->changed = 0;
        NidhoggrDuel_ResetAttackAnimationHistory(p);
        NidhoggrDuel_SetAnimation(p, NidhoggrDuel_SelectAttackAnimation(p), 1, 8);
    }
    p->damage = 0;
    p->special = 0;
    GridAttack_AddFlags(p, 12);
    if (!p->reset(p) && p->animationEnded) {
        NidhoggrDuel_SetAnimation(p, NidhoggrDuel_SelectAttackAnimation(p), 1, 8);
        if (tick > 31)
            p->finished = 1;
    }
}

void NidhoggrDuel_UpdateAnimationThreeState(struct GridAttackActor *p)
{
    if (p->changed) {
        p->changed = 0;
        NidhoggrDuel_SetAnimation(p, 3, 1, 8);
    }
    p->damage = 0;
    p->special = 0;
    GridAttack_AddFlags(p, 12);
    if (!p->reset(p) && p->animationEnded)
        p->finished = 1;
}

void NidhoggrDuel_UpdateFastAnimationThreeState(struct GridAttackActor *p)
{
    if (p->changed) {
        p->changed = 0;
        NidhoggrDuel_SetAnimation(p, 3, 1, 12);
    }
    p->damage = 0;
    p->special = 0;
    GridAttack_AddFlags(p, 12);
    if (!p->reset(p) && p->animationEnded)
        p->finished = 1;
}

void NidhoggrDuel_UpdateTimedHideState(struct GridAttackActor *p, int tick)
{
    struct GridAttackActor *base = p;
    /* Keep the transform base live: direct actor fields change register allocation. */
    struct GridAttackTransform *position = &p->transform;
    int done;
    if (p->changed) {
        p->changed = 0;
        NidhoggrDuel_SetAnimation(p, 11, 1, 8);
        p->savedPosition = position->coordinates;
    }
    done = 0;
    if (Sprite_BlinkUntilExpired(&done, tick, 10, 30)) {
        GridAttack_AddFlags(p, 16);
    } else if (done) {
        GridAttack_AddFlags(p, 16);
    }
    base->damage = 0;
    base->special = 0;
    GridAttack_AddFlags(p, 12);
    if (!p->reset(p) && tick > 59)
        p->finished = 1;
}

extern u32 gUnk_02000560;

void NidhoggrDuel_RunStateIfUnpaused(u32 a)
{
    if (!PlayerSlot_ReadStatus(0, 4))
        SceneActors_RunStateCallback(a);
}

extern const u32 gUnk_08604FA8[];

u32 NidhoggrDuel_ApplyRequestedState(struct GridAttackActor *p)
{
    if ((u8)(p->requested - 1) > 0x11)
        return 0;
    {
        u32 v = gUnk_08604FA8[p->requested];
        p->state = p->requested;
        p->callback = (void (*)(void *))v;
        p->timer = 0;
        p->changed = 1;
        p->stateReset = 0;
        p->pending = 0;
        return 1;
    }
}

int NidhoggrDuel_ApplyPendingDamage(struct GridAttackActor *p)
{
    struct GridAttackActor *base = (void *)p;
    /* Keep the transform base live: direct actor fields change register allocation. */
    struct GridAttackTransform *position = &p->transform;
    void *collider = &p->damageCollider;
    int damage, special;
    struct GridAttackPacked size;
    {
        int *damagePtr = &p->damage;
        damage = *damagePtr;
        {
            u8 *specialPtr = &p->special;
            int value;
            value = *specialPtr;
            special = value;
            *damagePtr = 0;
            *specialPtr = 0;
        }
    }
    if (damage > 0) {
        int react = 0;
        p->health -= damage;
        {
            int *accum = &p->accumulated;
            {
                int value = *accum + damage;
                *accum = value;
                {
                    int *threshold = &p->threshold;
                    if (value >= *threshold) {
                        value = react;
                        *accum = value;
                        {
                            react = 1;
                        }
                    }
                }
            }
        }
        p->stun = 15;
        p->flash = 10;
        if (special)
            p->animData.palette = p->specialDamageGraphics;
        else
            p->animData.palette = p->normalDamageGraphics;
        size.x = 16;
        size.y = 0;
        size.z = 16;
        DamageNumberDisplay_Show(collider, damage, 0, &position->coordinates, &size, 1);
        if (base->health <= 0) {
            base->health = 0;
            GridAttack_AddFlags(base, 2);
            GridAttack_SetState(base, NidhoggrDuel_UpdateDeathState, 8);
            return 1;
        }
        if (special) {
            GridAttack_AddFlags(base, 0x80000);
        }
        if (react) {
            {
                base->flags |= 1;
            }
            GridAttack_SetState(base, NidhoggrDuel_UpdateStaggerState, 7);
            return 1;
        }
        p->shake = 3;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 index = gRandomIndex * 2;
            u32 value;
            value = *(u16 *)((u8 *)((u16 *)RANDOM_TABLE_ADDR) + index);
            p->shakeAngle = value;
        }
        Sound_PlaySongChecked(318);
    }
    return 0;
}

static inline void GridAttack_SelectAction(struct GridAttackActor *p, int id, int zero)
{
    p->action = id;
    p->actionTimer = zero;
    p->entered = 1;
}

int NidhoggrDuel_SelectNextAction(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    int trigger = p->forceAction;
    if (trigger) {
        p->forceAction = 0;
        GridAttack_SelectAction(p, 2, 0);
        return 1;
    } else {
        int active = NidhoggrDuelBurst_GetEnabled(p->contactBurst);
        if (active) {
            GridAttack_SelectAction(p, 3, 0);
        } else if (p->attackCount >= p->attackLimit) {
            if (NidhoggrDuelMinionPool_GetActiveCount(p->contactPool) > 0)
                NidhoggrDuelMinionPool_BeginAllDeaths(p->contactPool);
            GridAttack_SelectAction(p, 1, 0);
        } else {
            GridAttack_SelectAction(base, 3, 0);
        }
    }
    return 0;
}

static inline u8 GridAttack_TakeActionEntry(struct GridAttackActor *p)
{
    if (p->entered) {
        p->entered = 0;
        p->finished = 0;
        return TRUE;
    }
    return FALSE;
}

static inline void GridAttack_BeginIdleWait(struct GridAttackActor *actor, u16 duration)
{
    GridAttack_StartActionMode(actor, 0, 0, 0);
    actor->actionDuration = duration;
}

void SceneActors_ConfigureEffect(void *, s32, s32, s32);
static inline void GridAttack_StartMessageAction(struct GridAttackActor *p, s32 mode, s32 zero,
                                                 s32 id)
{
    p->active = mode;
    p->actionReset = zero;
    p->action = id;
    p->actionTimer = zero;
    p->entered = 1;
}
struct GridAttackMessageActor {
    u8 unknown0[0x54];
    u16 x, y, z;
    u8 unknown5A[0x184 - 0x5a];
    u16 height;
    u8 unknown186[0x8a4 - 0x186];
    u16 drawX, drawY, drawZ;
};
static inline void GridAttack_SetMessageX(struct GridAttackActor *p, u16 value)
{
    ((struct GridAttackMessageActor *)p)->x = value;
}
static inline u16 GridAttack_GetMessageX(struct GridAttackActor *p)
{
    return ((struct GridAttackMessageActor *)p)->x;
}
static inline void GridAttack_SetDrawableX(struct GridAttackActor *p, u16 value)
{
    ((struct GridAttackMessageActor *)p)->drawX = value;
}
static inline void GridAttack_SetMessageY(struct GridAttackActor *p, u16 value)
{
    ((struct GridAttackMessageActor *)p)->y = value;
}
static inline u16 GridAttack_GetMessageY(struct GridAttackActor *p)
{
    return ((struct GridAttackMessageActor *)p)->y;
}
static inline void GridAttack_SetDrawableY(struct GridAttackActor *p, u16 value)
{
    ((struct GridAttackMessageActor *)p)->drawY = value;
}
static inline void GridAttack_SetMessageZ(struct GridAttackActor *p, u16 value)
{
    ((struct GridAttackMessageActor *)p)->z = value;
}
static inline u16 GridAttack_GetMessageZ(struct GridAttackActor *p)
{
    return ((struct GridAttackMessageActor *)p)->z;
}
static inline void GridAttack_SetDrawableZ(struct GridAttackActor *p, u16 value)
{
    ((struct GridAttackMessageActor *)p)->drawZ = value;
}

void NidhoggrDuel_ProcessMessages(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    struct SceneEventReceiver *receiver = &p->receiver;
    s32 i = 0;
    if (i < receiver->count[gSceneEventDispatchState->phase]) {
        do {
            struct SceneEventDispatchState *controller = gSceneEventDispatchState;
            struct SceneEventCommand *command = receiver->pending[controller->phase][i];
            SceneMessages_SetPendingTrigger(receiver, command);
            switch (command->kind) {
                case 0:
                    GridAttack_StartMessageAction(p, 2, 0, 0);
                    break;

                case 1:
                    p->savedAngle = ((u8)command->value + 5) << 5;
                    GridAttack_StartMessageAction(p, 2, 0, 1);
                    break;

                case 10:
                    GridAttack_StartMessageAction(p, 2, 0, 2);
                    break;

                case 11:
                    GridAttack_StartMessageAction(p, 2, 0, 3);
                    break;

                case 29:
                    GridAttack_StartMessageAction(p, 2, 0, 4);
                    break;

                case 30:
                    GridAttack_StartMessageAction(p, 2, 0, 5);
                    break;

                case 31:
                    GridAttack_StartMessageAction(p, 2, 0, 6);
                    break;

                case 32:
                    GridAttack_StartMessageAction(p, 2, 0, 7);
                    break;

                case 18:
                    GridAttack_StartMessageAction(p, 2, 0, 8);
                    break;

                case 9:
                    SceneActors_ConfigureEffect(p, command->value, 180, 180);
                    Trigger_ConsumePending(&p->receiver, 1);
                    break;

                case 17:
                    base->messageParameter = command->value;
                    Trigger_ConsumePending(&p->receiver, 1);
                    break;

                case 2: {
                    GridAttack_SetMessageX(p, ((u16 *)command)[4]);
                    GridAttack_SetMessageY(p, ((u16 *)command)[5]);
                    GridAttack_SetMessageZ(p, ((u16 *)command)[6]);
                    GridAttack_SetDrawableX(base, GridAttack_GetMessageX(p));
                    GridAttack_SetDrawableY(base, (u16)base->height + GridAttack_GetMessageY(p));
                    GridAttack_SetDrawableZ(base, GridAttack_GetMessageZ(p));
                    Trigger_ConsumePending(&p->receiver, 1);
                    break;
                }
            }

            i++;
        } while (i < receiver->count[gSceneEventDispatchState->phase]);
    }
}

void NidhoggrDuel_UpdateStateNineAction(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntry(p)) {
        GridAttack_RequestAnimation(p, 9);
        Trigger_ConsumePending(&p->receiver, 1);
    }
    p->actionTimer++;
}

static inline u8 GridAttack_TakeActionEntryViaPointer(struct GridAttackActor *p)
{
    u8 *q = &p->entered;
    if (*q) {
        *q = 0;
        q--;
        *q = 0;
        return 1;
    }
    return 0;
}

void NidhoggrDuel_UpdateIdleAction(struct GridAttackActor *p)
{
    u8 r = GridAttack_TakeActionEntryViaPointer(p);
    if (r == 1) {
        p->requested = r;
        Trigger_ConsumePending(&p->receiver, 1);
    }
    p->actionTimer++;
}

static inline u8 GridAttack_TakeActionEntryBool(struct GridAttackActor *p)
{
    if (p->entered) {
        p->entered = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}

void NidhoggrDuel_UpdateStateElevenAction(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntryBool(p)) {
        GridAttack_RequestAnimation(p, 11);
    }
    if (p->finished) {
        GridAttack_StartActionMode(p, 2, 0, 4);
        Trigger_ConsumePending(&p->receiver, 1);
    } else
        p->actionTimer++;
}

void NidhoggrDuel_UpdateStateTwelveAction(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntryBool(p)) {
        GridAttack_RequestAnimation(p, 0xC);
    }
    if (p->finished)
        Trigger_ConsumePending(&p->receiver, 1);
    else
        p->actionTimer++;
}

void NidhoggrDuel_UpdateStateThirteenAction(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntryBool(p)) {
        GridAttack_RequestAnimation(p, 0xD);
    }
    if (p->finished)
        Trigger_ConsumePending(&p->receiver, 1);
    else
        p->actionTimer++;
}

void NidhoggrDuel_UpdateAnimationFourteen(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntryBool(p)) {
        GridAttack_RequestAnimation(p, 14);
    }
    if (p->finished) {
        GridAttack_StartActionMode(p, 2, 0, 4);
        Trigger_ConsumePending(&p->receiver, 1);
    } else
        p->actionTimer++;
}

void NidhoggrDuel_UpdateAnimationFifteen(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntryBool(p)) {
        GridAttack_RequestAnimation(p, 0xF);
    }
    if (p->finished)
        Trigger_ConsumePending(&p->receiver, 1);
    else
        p->actionTimer++;
}

void NidhoggrDuel_UpdateAnimationSixteen(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntryBool(p)) {
        GridAttack_RequestAnimation(p, 16);
    }
    if (p->finished) {
        GridAttack_StartActionMode(p, 2, 0, 4);
        Trigger_ConsumePending(&p->receiver, 1);
    } else
        p->actionTimer++;
}

void NidhoggrDuel_UpdateAnimationSeventeen(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntryBool(p)) {
        GridAttack_RequestAnimation(p, 0x11);
    }
    if (p->finished) {
        GridAttack_StartActionMode(p, 2, 0, 0);
        Trigger_ConsumePending(&p->receiver, 1);
    } else
        p->actionTimer++;
}

extern const u32 gUnk_08604FF0[];

void NidhoggrDuel_DispatchSpecialAnimation(struct GridAttackActor *p)
{
    Callback_InvokeActor(p, (void *)gUnk_08604FF0[p->action]);
}

static inline void GridAttack_StartReaction(struct GridAttackActor *p, int one, int id, int zero)
{
    p->active = one;
    p->actionReset = zero;
    p->action = id;
    p->actionTimer = zero;
    p->entered = one;
}

int NidhoggrDuel_TryStartDamageReaction(struct GridAttackActor *p)
{
    if (p->health <= 0 && p->healthCheck) {
        GridAttack_AddFlags(p, 2);
        GridAttack_SetState(p, NidhoggrDuel_UpdateDeathState, 8);
    }
    p->healthCheck = 0;
    {
        if (GridAttack_TestFlags(p, 2)) {
            GridAttack_StartReaction(p, 1, 3, 0);
            return 1;
        } else if (p->flags & 1) {
            GridAttack_StartReaction(p, 1, 2, 0);
            return 1;
        }
    }
    return 0;
}

void NidhoggrDuel_UpdateWaitAction(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    p->healthCheck = 1;
    if (GridAttack_TakeActionEntryBool(p) == 1)
        base->requested = 1;
    if (base->actionTimer > p->actionDuration) {
        if (NidhoggrDuel_SelectNextAction(p))
            return;
    }
    base->actionTimer++;
}

void NidhoggrDuel_UpdateActionTwo(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    p->healthCheck = 1;
    if (GridAttack_TakeActionEntryBool(p)) {
        GridAttack_RequestAnimation(p, 2);
    }
    if (p->finished) {
        GridAttack_BeginIdleWait(base, 60);
    } else
        p->actionTimer++;
}

void NidhoggrDuel_UpdateDeathAction(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    p->healthCheck = 1;
    if (GridAttack_TakeActionEntryBool(p)) {
        GridAttack_RequestAnimation(p, 3);
    }
    if (p->finished) {
        GridAttack_BeginIdleWait(base, 60);
    } else
        p->actionTimer++;
}

void NidhoggrDuel_UpdateAttackAction(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    p->healthCheck = 1;
    if (GridAttack_TakeActionEntryBool(p)) {
        GridAttack_RequestAnimation(p, 4);
    }
    if (p->finished) {
        int duration;
        int value = p->attackRange;
        if (value < 64)
            duration = base->farAttackDelay;
        else
            duration = base->nearAttackDelay;
        GridAttack_BeginIdleWait(base, duration);
    } else
        p->actionTimer++;
}

void NidhoggrDuel_UpdateAnimationFive(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntry(p)) {
        GridAttack_RequestAnimation(p, 5);
    }
    p->actionTimer++;
}

void NidhoggrDuel_UpdateRecoveryAction(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    if (GridAttack_TakeActionEntryBool(p)) {
        GridAttack_RequestAnimation(p, 6);
    }
    if (p->finished) {
        GridAttack_BeginIdleWait(base, 30);
    }
    p->actionTimer++;
}

void NidhoggrDuel_UpdateDamageAction(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    if (GridAttack_TakeActionEntryBool(p) == 1) {
        GridAttack_RequestAnimation(p, 7);
        base->forceAction = 1;
    }
    {
        if (!GridAttack_TestFlags(p, 1)) {
            GridAttack_BeginIdleWait(base, 30);
        } else
            p->actionTimer++;
    }
}

void NidhoggrDuel_UpdatePassiveAction(struct GridAttackActor *p)
{
    u8 *q = &p->entered;
    if (*q) {
        *q = 0;
        q--;
        *q = 0;
    }
    p->actionTimer++;
}

extern const u32 gUnk_08605014[];

void NidhoggrDuel_DispatchNormalAction(struct GridAttackActor *p)
{
    Callback_InvokeActor(p, (void *)gUnk_08605014[p->action]);
}

extern const u32 gUnk_08605024[];

void NidhoggrDuel_DispatchReactionAction(struct GridAttackActor *p)
{
    Callback_InvokeActor(p, (void *)gUnk_08605024[p->action]);
}

extern const u32 gUnk_08605034[];

void NidhoggrDuel_UpdateNormalActions(struct GridAttackActor *p)
{
    if (!PlayerSlot_ReadStatus(0, 4)) {
        if (NidhoggrDuel_TryStartDamageReaction(p) == 0) {
            Callback_InvokeActor(p, (void *)gUnk_08605034[p->actionReset]);
        }
    }
}

extern const u32 gUnk_08605038[];

void NidhoggrDuel_UpdateReactionActions(struct GridAttackActor *p)
{
    Callback_InvokeActor(p, (void *)gUnk_08605038[p->actionReset]);
}

extern const u32 gUnk_0860503C[];

void NidhoggrDuel_UpdateSpecialActions(struct GridAttackActor *p)
{
    Callback_InvokeActor(p, (void *)gUnk_0860503C[p->actionReset]);
}

extern u32 gUnk_08605040[];

void NidhoggrDuel_UpdateActionController(struct GridAttackActor *p)
{
    p->controllerReset = 0;
    Callback_InvokeActor(p, (void *)gUnk_08605040[p->active]);
}

void SceneActors_UpdateTargetMetrics(void *);
static inline s32 GridAttack_Truncate12(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

void NidhoggrDuel_UpdateAnimationAndTargets(struct GridAttackActor *p)
{
    struct GridAttackActor *actor = p;
    struct GridAttackActor *owner = p;
    struct GridAttackTransform *body = &actor->transform;
    s32 height;
    u32 i;
    NidhoggrDuel_ProcessMessages(actor);
    Obj_StepPosition(body);
    owner->animationEnded = PlayerCue_StepSequenceEnded(&actor->drawable, &actor->animation);
    if (actor->projectedDisplay)
        height = ProjectedDisplay_GetHeight(actor->projectedDisplay);
    else
        height = 0;
    actor->drawable.position.x = body->coordinates.x;
    actor->drawable.position.y = body->coordinates.y + height + 128;
    actor->drawable.position.z = body->coordinates.z;
    actor->shadowPosition = actor->drawable.position;
    if (GridAttack_TestFlags(owner, 0x20000)) {
        actor->drawable.position.x += GridAttack_Truncate12(
            gUnk_086149C4[(actor->shakeAngle + 64) & 255] * (actor->shake << 2));
        actor->drawable.position.z +=
            GridAttack_Truncate12(gUnk_086149C4[actor->shakeAngle] * (actor->shake << 2));
    }
    if (actor->reactionCooldown) {
        u32 angle;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        angle = *(u16 *)((gRandomIndex << 1) + (u32)gRandomTable);
        actor->drawable.position.x += GridAttack_Truncate12(gUnk_086149C4[(angle + 64) & 255] *
                                                            (actor->reactionCooldown << 2));
        actor->drawable.position.z +=
            GridAttack_Truncate12(gUnk_086149C4[angle & 255] * (actor->reactionCooldown << 2));
        actor->reactionCooldown--;
    }
    SceneActors_UpdateTargetMetrics(owner);
    i = 0;
    if (i < actor->targetCount) {
        struct GridAttackTransform *base = &owner->transform;
        do {
            struct SceneVec *target = owner->targetPositions[i];
            if (target) {
                s32 x = target->x - base->coordinates.x;
                s32 z = target->z - base->coordinates.z;
                owner->requestedAngles[i] = Math_Atan2(x, z);
                owner->targetDistances[i] = x * x + z * z;
            }
            i++;
        } while (i < actor->targetCount);
    }
}

s32 NidhoggrDuelMinionPool_GetActiveCount(void *);
void SceneActors_ConfigureEffect(void *, s32, s32, s32);
void SceneActors_UpdateEffects(void *);

void NidhoggrDuel_EmitSpriteParticles(struct GridAttackActor *);
void Palette_InterpolateBanks(void *, const void *, const void *, s32, s32);

static inline void GridAttack_CopyPalette(const void *source, void *dest)
{
    CpuSet(source, dest, 0x04000008);
}
void NidhoggrDuel_UpdateCollidersAndPalette(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    struct GridAttackTransform *transform = &p->transform;
    struct ContactCollider *collider;
    if (NidhoggrDuelMinionPool_GetActiveCount(p->contactPool) > p->contactCountLimit) {
        NidhoggrDuelBurst_SetEnabled(p->contactBurst, 1);
        GridAttack_AddFlags(p, 4);
        p->attackCount = 0;
    } else {
        if ((!p->attackCount) && NidhoggrDuelBurst_GetEnabled(p->contactBurst)) {
            SceneActors_ConfigureEffect(p, 1, 0, 0);
            Sound_PlaySongChecked(0x54D);
        }
        NidhoggrDuelBurst_SetEnabled(base->contactBurst, 0);
        if (base->attackCount < base->attackLimit)
            base->attackCount++;
    }
    if (NidhoggrDuelBurst_GetEnabled(base->contactBurst))
        GridAttack_ClearFlags(p, 0x40);
    SceneActors_UpdateEffects(p);
    if (base->stun) {
        GridAttack_AddFlags(p, 4);
        base->stun--;
    }
    collider = &base->damageCollider;
    if (GridAttack_TestFlags(p, 4))
        collider->flags |= 4;
    else {
        ContactCollider_ClearFlags(collider, 4);
        Collider_SetPosition(collider, &base->drawable.position, 0);
        collider->direction = transform->angle;
    }
    collider = &base->bodyCollider;
    if (!(GridAttack_TestFlags(p, 8))) {
        Collider_SetPosition(collider, &base->drawable.position, 0);
        collider->direction = transform->angle;
        Collider_Refresh(collider);
    }
    if (GridAttack_TestFlags(p, 16)) {
        base->drawable.flags |= 1;
        CameraTracking_ClearRequests(&base->camera, 1);
        ContactCollider_ClearFlags(&base->damageCollider, 8);
    } else {
        base->drawable.flags &= ~1;
        if (p->locked)
            CameraTracking_ClearRequests(&base->camera, 1);
        else
            CameraTracking_SetRequests(&base->camera, 1);
        if (GridAttack_TestFlags(p, 32))
            ContactCollider_SetFlags(&base->damageCollider, 8);
        else
            ContactCollider_ClearFlags(&base->damageCollider, 8);
    }
    if (base->shake) {
        base->shake--;
        GridAttack_AddFlags(p, 0x20000);
    }
    if (base->flash > 0) {
        if (!(--base->flash))
            base->animData.palette = base->palette;
    } else {
        s32 zero = 0;
        u32 id = base->effectId;
        if (base->effectMode == 1) {
            s8 *amount = &base->effectVariant;
            s32 value = *amount;
            u8 *duration = &base->effectDuration;
            if (value < (1 << *duration)) {
                u8 *src;
                u8 *dest;
                void *palette;
                (*amount)++;
                src = gPaletteBankData;
                dest = src + (id << 5);
                palette = base->palette;
                Palette_InterpolateBanks(palette, src, dest, *amount, *duration);
                base->animData.palette = palette;
            }
        } else if (base->effectMode == 2) {
            void *palette;
            s8 *amount = &base->effectVariant;
            if (*amount > 0) {
                u8 *src;
                u8 *dest;
                (*amount)--;
                src = gPaletteBankData;
                dest = src + (id << 5);
                palette = base->palette;
                Palette_InterpolateBanks(palette, src, dest, *amount, base->effectDuration);
                base->animData.palette = palette;
            } else {
                const u8 *src;
                *amount = zero;
                base->effectMode = zero;
                base->effectId = 1;
                src = gPaletteBankData + 0x2BA0;
                palette = base->palette;
                GridAttack_CopyPalette(src, palette);
                base->animData.palette = palette;
            }
        } else if (base->effectMode == 3) {
            u8 *src = gPaletteBankData;
            u8 *dest = src + (id << 5);
            void *palette = base->palette;
            s8 *amount = &base->effectVariant;
            Palette_InterpolateBanks(palette, src, dest, *amount, base->effectDuration);
            base->animData.palette = palette;
            if (!base->effectDirection) {
                (*amount)++;
                if (*amount > 31) {
                    *amount = 32;
                    base->effectDirection = 1;
                }
            } else {
                (*amount)--;
                if (*amount <= 0) {
                    *amount = zero;
                    base->effectDirection = zero;
                }
            }
        }
    }
    if (base->projectedDisplay)
        base->drawable.priority = 1;
    else
        base->drawable.priority = 2;
    if (!p->locked)
        CameraTracking_Update(&base->camera);
    if ((p->locked && base->messageParameter) && (!(p->effectTimer & 15)))
        NidhoggrDuel_EmitSpriteParticles(base);
    p->contactSource = 0;
    p->contactFlags = 0;
    p->effectTimer++;
}

int NidhoggrDuel_Update(struct GridAttackActor *p)
{
    u16 *t;
    NidhoggrDuel_UpdateAnimationAndTargets(p);
    NidhoggrDuel_UpdateActionController(p);
    NidhoggrDuel_RunStateIfUnpaused((u32)p);
    NidhoggrDuel_UpdateCollidersAndPalette(p);
    if (p->phaseTimer <= 0xef)
        NidhoggrDuel_RequestLinkedActorState(p);
    else
        NidhoggrDuel_NotifyLinkedActorLatePhase(p);
    t = (u16 *)&p->phaseTimer;
    (*t)++;
    if (*t > 0x12b)
        *t = 0;
    return 0;
}

u32 NidhoggrDuel_Destroy(struct GridAttackActor *p)
{
    Collider_Remove(&p->damageCollider);
    NidhoggrDuel_RemoveSprite(p);
    SceneActors_Destroy(p);
    return 0;
}

void SceneActors_Register(void *, s32);
s32 SceneActors_Initialize(void *, u32, s32, void *, s32);
void SceneActors_SetHealthLimits(void *, s32, s32, s32);
void SceneActors_SetEventScripts(void *, s32, s32);
void SceneActors_SetDamageCallback(void *, void *, void *, s32);
void CameraFollow_Initialize(void *, void *, void *, u32);
void *NidhoggrDuelMinionPool_Create(s32, u32, s32, u32, u32);
void *NidhoggrDuelOrb_Create(const struct ContactPosition *, u32);

s32 NidhoggrDuel_Initialize(struct GridAttackActor *p, u32 ctx)
{
    struct GridAttackActor *actor = p;
    struct GridAttackPacked pk;

    s32 kind;
    s32 i;
    u32 count;
    u32 targetIndex;
    s32 health;
    u8 facing;
    s32 poolHealth;
    s32 poolValue;
    s32 poolResource;
    s32 orbitValue;
    struct ContactPosition *position;
    u8 *locked;
    u8 (**tryUpdate)(void *);
    u8 (**reset)(void *);
    struct SceneVec **positions;
    u32 *dst;
    const u8 *palette;

    kind = Script_GetKeywordValue(0x74, 3);
    if (kind != 3)
        return -1;
    SceneActors_Register(p, 3);
    p->locked = Script_GetKeywordValue(0x69, 0);
    count = Script_GetKeywordValue(0x50, 1);
    p->targetCount = count;
    targetIndex = 0;
    positions = p->targetPositions;
    locked = &p->locked;
    tryUpdate = &p->tryUpdate;
    reset = &p->reset;
    position = &p->transform.coordinates;
    if (targetIndex < count) {
        u32 targetCount = count;
        do {
            struct Player *player = gPlayers[targetIndex];
            if (player) {
                actor->players[targetIndex] = player;
                positions[targetIndex] = &player->position;
            } else {
                actor->players[targetIndex] = 0;
                positions[targetIndex] = 0;
            }
            targetIndex++;
        } while (targetIndex < targetCount);
    }

    if (Script_SeekToKeyword(0x70)) {
        pk.x = Script_GetValue();
        pk.y = Script_GetValue();
        pk.z = Script_GetValue();
        facing = ((u32)Script_GetValue() << 5) + 0xA0;
    } else {
        pk.x = (u16)gGameState->positionX;
        pk.y = (u16)gGameState->positionY;
        pk.z = (u16)gGameState->positionZ;
        facing = 0x40;
    }

    if (Script_SeekToKeyword(0x73)) {
        health = Script_GetValue();
        p->bodyMode = Script_GetValue();
        p->threshold = Script_GetValue();
        p->attackLimit = Script_GetValue();
        p->contactCountLimit = Script_GetValue();
    } else {
        health = 1000;
        p->bodyMode = 20;
        p->threshold = 60;
        p->attackLimit = 300;
        p->contactCountLimit = 2;
    }
    p->attackCount = p->attackLimit;

    if (Script_SeekToKeyword(0x61)) {
        p->attackPower = Script_GetValue();
        p->attackKind = Script_GetValue();
        p->attackParameter = Script_GetValue();
        orbitValue = Script_GetValue();
        p->projectileDamage = Script_GetValue();
        p->targetAttackParameter = Script_GetValue();
        p->nearAttackDelay = Script_GetValue();
        p->farAttackDelay = Script_GetValue();
        poolHealth = Script_GetValue();
        poolValue = Script_GetValue();
        poolResource = Script_GetValue();
    } else {
        p->attackPower = 50;
        p->attackKind = 50;
        p->attackParameter = 60;
        orbitValue = 100;
        p->projectileDamage = 40;
        p->targetAttackParameter = 120;
        p->nearAttackDelay = 60;
        p->farAttackDelay = 30;
        poolHealth = 120;
        poolValue = 20;
        poolResource = 50;
    }

    SceneActors_Initialize(actor, ctx, kind, &pk, facing);
    AnimData_Load(&p->animData, 0x4147);
    Sprite_Init(&p->drawable, &p->animData, 1);
    p->drawable.flags &= ~1;
    p->drawable.position = *(struct ContactPosition *)&pk;
    p->drawable.priority = 1;
    p->resource = ScriptProc_Start(0x922E, 0xC5E2);
    Anim_SetSequence(&p->animation, p->resource, 2, 0, 0);

    if (*locked) {
        switch (Script_GetKeywordValue(0x44, 0)) {
            case 0:
                GridAttack_StartActionMode(actor, 2, 0, 1);
                GridAttack_SetState(actor, (void *)NidhoggrDuel_UpdateIdleState, 10);
                break;
            case 1:
                GridAttack_StartActionMode(actor, 2, 0, 0);
                GridAttack_SetState(actor, (void *)NidhoggrDuel_UpdateHiddenState, 9);
                break;
            case 2:
                GridAttack_StartActionMode(actor, 2, 0, 4);
                GridAttack_SetState(actor, (void *)NidhoggrDuel_UpdateAlternateIdleState, 13);
                break;
            case 3:
                GridAttack_StartActionMode(actor, 2, 0, 3);
                GridAttack_SetState(actor, (void *)NidhoggrDuel_UpdateAnimationElevenState, 12);
                break;
        }
    } else {
        GridAttack_BeginIdleWait(p, 90);
        GridAttack_SetState(actor, (void *)NidhoggrDuel_UpdateIdle, 1);
    }

    *tryUpdate = (void *)NidhoggrDuel_ApplyPendingDamage;
    *reset = (void *)NidhoggrDuel_ApplyRequestedState;
    SceneActors_SetHealthLimits(actor, health, health, p->bodyMode);
    NidhoggrDuel_InitBodyCollider(p);
    NidhoggrDuel_InitDamageCollider(p, p->bodyMode);

    if (Script_SeekToKeyword(0x63)) {
        s32 first = Script_GetValue();
        s32 second = Script_GetValue();
        SceneActors_SetEventScripts(actor, first, second);
        ctx = Script_GetValue();
    } else {
        SceneActors_SetEventScripts(actor, 0, 0);
        ctx = 0;
    }

    if (Script_SeekToKeyword(0x41)) {
        for (i = 0; i <= 3 && Script_GetPc() != 0; i++)
            actor->scriptArgs[i] = Script_GetValue();
    }

    p->contactReward = Script_GetKeywordValue(0x53, 0);

    {
        u8 **bank = &gPaletteBankData;
        u32 off;
        dst = (u32 *)p->palette;
        off = 0x2BA0;
        i = 15;
        do {
            palette = *bank + off;
            CpuSet(palette, dst, 0x04000008);
            dst += 8;
            off += 0x20;
        } while (--i >= 0);
    }
    {
        palette = gPaletteBankData + 0x3240;
        dst = (u32 *)p->normalDamageGraphics;
        i = 15;
        do {
            CpuSet(palette, dst, 0x04000008);
            dst += 8;
        } while (--i >= 0);
    }
    {
        palette = gPaletteBankData + 0x3240;
        dst = (u32 *)p->specialDamageGraphics;
        i = 15;
        do {
            CpuSet(palette, dst, 0x04000008);
            dst += 8;
        } while (--i >= 0);
    }

    p->animData.palette = p->palette;
    p->effectMode = 0;
    p->effectId = 0x15D;
    p->effectVariant = 0;
    p->effectDuration = 5;

    p->shadowPosition = p->drawable.position;
    CameraFollow_Initialize(&p->camera, positions[0], &p->shadowPosition, 1);
    CameraTracking_SetRange(&p->camera, 0, 0x8C);

    SceneActors_SetDamageCallback(actor, &p->damageCollider, (void *)NidhoggrDuel_OnDamage, 0);
    if (Script_GetKeywordValue(0x43, 1) != 0)
        p->projectedDisplay = ProjectedDisplay_Create(position);
    else
        p->projectedDisplay = 0;

    p->contactPool =
        NidhoggrDuelMinionPool_Create(poolHealth, poolValue, poolResource, ctx, (s16)p->contactReward);
    p->orbitContact = NidhoggrDuelOrb_Create(position, orbitValue);
    p->contactBurst = NidhoggrDuelBurst_Create(&p->drawable.position);
    p->linkedActorId = Script_GetKeywordValue(0x54, 0);
    p->linkedActorPhase = 0;
    p->phaseTimer = 0;
    return 0;
}

struct GridAttackActor *NidhoggrDuel_Create(u32 a)
{
    struct GridAttackActor *p;
    p = SceneActors_GetBySlot(Script_GetKeywordValue(0x74, 3));
    if (p != 0)
        return p;
    p = Task_Create(8, 0x924);
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))NidhoggrDuel_Update,
                          (void (*)(void *))NidhoggrDuel_Destroy);
        if (NidhoggrDuel_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
