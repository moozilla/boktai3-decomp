#include "grid_attack_actor.h"
#include "player_cue_sequence.h"

void GridAttackActor_RemoveSprite(struct GridAttackActor *p)
{
    Sprite_Remove(p->drawable.sprite);
}

void Anim_RestartSequence(void *, void *, int, int, int);

void Collider_SetAttackProperties(void *, int, int, int, int, int);

void Collider_SetBodyProperties(void *, int, int, int);

void GridAttackActor_OnDamageCollision(struct ContactCollider *, struct ContactCollider *,
                                       struct GridAttackActor *);

void ContactSequencePool_SpawnActor(void *, void *, int);

int sub_08249350(int, int);

void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);

void SparkParticle_Spawn(void *, int, void *, int, int, int, int, int);

void SceneMessages_Send(void *);

void ProjectedDisplay_SetHeight(void *, int);

void OrbitContactActor_BeginExpansion(void *);

void GridAttackActor_SpawnTargetedProjectile(struct GridAttackActor *, struct GridAttackPoint *,
                                             int);

void ContactSequencePool_BeginAllDeaths(void *);

void SceneActors_RunFirstEventScript(void *, void *);

void GameState_SetEventFlag(int);

void SceneActors_RunSecondEventScript(void *, void *);

u8 Sprite_BlinkUntilExpired(int *, int, int, int);

void SceneActors_RunStateCallback(u32);

int ContactBurst_GetEnabled(void *);

int ContactSequencePool_GetActiveCount(void *);

void GridAttack_ProcessMessages(struct GridAttackActor *p);
void GridAttack_UpdateAnimationAndTargets(struct GridAttackActor *p);

void GridAttack_UpdateCollidersAndPalette(struct GridAttackActor *p);

void SceneActors_Destroy(struct GridAttackActor *);

struct GridAttackActor *SceneActors_GetBySlot(void);

s32 GridAttackActor_Initialize(struct GridAttackActor *, u32);

void GridAttackActor_SetAnimation(struct GridAttackActor *p, int sequence, int start, int speed)
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

static inline u32 testFlags(u16 *flags, u32 mask)
{
    return *flags & mask;
}
void GridAttackActor_OnDamageCollision(struct ContactCollider *source,
                                       struct ContactCollider *other, struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    Collision_CalculateDamageResponse(source, other);
    PlayerStats_AddContactHundredths(source, other, p->contactReward);
    p->damage = other->cost;
    if (testFlags(&source->flags, 0x100))
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
    {
        u32 mask = 4;
        if (source->reactionFlags & mask)
            p->special = 1;
    }
}

void GridAttackActor_OnBodyCollision(void)
{
}

void GridAttackActor_InitBodyCollider(struct GridAttackActor *p)
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
    Collider_SetCallback(collider, GridAttackActor_OnBodyCollision, p);
    Collider_SetAttackProperties(collider, p->attackPower, p->attackKind, 0, 0, p->attackParameter);
}

void GridAttackActor_InitDamageCollider(struct GridAttackActor *p, int mode)
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
    Collider_SetCallback(collider, GridAttackActor_OnDamageCollision, p);
    Collider_SetBodyProperties(collider, mode, 0x808, 4);
    Collider_Refresh(collider);
}

void GridAttackActor_GetGridPosition(s16 *p, int x, int z)
{
    if (x != 1 || z) {
        p[0] = (x << 8) + 0xa80;
        {
            int height = 256;
            p[1] = height;
        }
        p[2] = (z << 8) + 0xa80;
    }
}

void GridAttackActor_SpawnGridContact(struct GridAttackActor *p, int x, int z)
{
    struct SceneVec pos;
    GridAttackActor_GetGridPosition((s16 *)&pos, x, z);
    ContactSequencePool_SpawnActor(p->contactPool, &pos, x + z * 3);
}

static inline int GridAttack_RandomColumn(u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return sub_08249350(table[gRandomIndex], 3);
}

void GridAttackActor_SpawnGridPattern(struct GridAttackActor *p)
{
    if (p->targetPositions[0]->z < 0xc00) {
        GridAttackActor_SpawnGridContact(p, 0, 0);
        GridAttackActor_SpawnGridContact(p, 2, 0);
        GridAttackActor_SpawnGridContact(p, 0, 1);
        GridAttackActor_SpawnGridContact(p, 1, 1);
        GridAttackActor_SpawnGridContact(p, 2, 1);
        {
            u16 *table = gRandomTable;
            GridAttackActor_SpawnGridContact(p, GridAttack_RandomColumn(table), 2);
            GridAttackActor_SpawnGridContact(p, GridAttack_RandomColumn(table), 3);
            GridAttackActor_SpawnGridContact(p, GridAttack_RandomColumn(table), 4);
        }
    } else {
        GridAttackActor_SpawnGridContact(p, 0, 1);
        GridAttackActor_SpawnGridContact(p, 1, 1);
        GridAttackActor_SpawnGridContact(p, 2, 1);
        {
            u16 *table = gRandomTable;
            GridAttackActor_SpawnGridContact(p, GridAttack_RandomColumn(table), 2);
            GridAttackActor_SpawnGridContact(p, GridAttack_RandomColumn(table), 3);
            GridAttackActor_SpawnGridContact(p, GridAttack_RandomColumn(table), 4);
        }
    }
}

static inline void GridAttack_SetState(struct GridAttackActor *p, void (*fn)(void *), int id)
{
    int zero;
    p->state = id;
    zero = 0;
    p->callback = fn;
    p->timer = zero;
    p->changed = 1;
    p->stateReset = zero;
    p->pending = zero;
}

static inline void GridAttack_StartAction(struct GridAttackActor *p, int id, int zero)
{
    int one = 1;
    p->active = one;
    p->actionReset = zero;
    p->action = id;
    p->actionTimer = zero;
    p->entered = one;
}

void *BouncingObject_Spawn(void *, void *, void *, s32, s32, s32, s32);
static inline void GridAttack_ClearPoint(const void *source, struct GridAttackPoint *dest)
{
    CpuSet(source, dest, 0x05000002);
}

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
    s32 upper = ~255;
    u32 rounded = upper & value;
    u32 mask = 63;
    u8 center = 128;
    *coordinate = (rounded + ((random & mask) + center)) - 32;
}
void GridAttackActor_SpawnTargetedProjectile(struct GridAttackActor *actor,
                                             struct GridAttackPoint *point, int kind)
{
    volatile u32 zero;
    struct GridAttackPoint velocity;
    struct SceneVec position;
    struct GridAttackPoint target;
    struct GridAttackPoint original;
    struct GridAttackPoint *vel = &velocity;
    volatile s16 *pos;
    zero = 0;
    GridAttack_ClearPoint((const void *)&zero, vel);
    pos = (volatile s16 *)&position;
    original = *point;
    *(struct GridAttackPoint *)&position = original;
    GridAttack_OffsetCoordinate(&pos[0], GridAttack_RandomValue());
    pos[1] += 1024;
    GridAttack_OffsetCoordinate(&pos[2], GridAttack_RandomValue());
    {
        struct GridAttackPoint *dest = &target;
        *dest = original;
        BouncingObject_Spawn(&position, vel, dest, actor->projectileDamage, 50, kind, 0);
    }
}

int GridAttackActor_OnDamage(struct GridAttackActor *p, int kind, int damage)
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
            GridAttack_StartAction(p, 0, 0);
            GridAttack_SetState(p, GridAttackActor_UpdateDamageHold, 5);
            break;
        case 2:
            GridAttack_StartAction(p, 1, 0);
            GridAttack_SetState(p, GridAttackActor_UpdateDamageRecovery, 6);
            {
                int angle = position->angle;
                p->damageAngle = angle;
            }
            break;
        case 1: {
            int angle = (p->damageSource)[0x2d];
            u8 *dst = &p->damageAngle;
            int zero = 0;
            *dst = angle;
            {
                u32 mask = 2;
                p->flags |= mask;
            }
            GridAttack_StartAction(p, 3, zero);
            GridAttack_SetState(p, GridAttack_UpdateDeathState, 8);
        } break;
    }
    return 1;
}

void GridAttackActor_ResetAttackAnimationHistory(struct GridAttackActor *p)
{
    p->attackAnimationHistory = 0;
}

int GridAttackActor_SelectAttackAnimation(struct GridAttackActor *p)
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

// MATCHING_CONCESSION: register-pinning

void GridAttackActor_EmitSpriteParticles(struct GridAttackActor *p)
{
    struct GridAttackDrawable *graphics = &p->drawable;
    struct GridAttackPacked size;
    int i;
    for (i = 0; i < 2; i++) {
        int angle, speed;
        int count;
        register u32 index asm("r1");
        u32 *indexPtr;
        u32 mask;
        int smallMask;
        register u16 *table asm("r4");
        size.x = 164;
        size.y = 16;
        size.z = 164;
        indexPtr = &gRandomIndex;
        mask = 1023;
        smallMask = 3;
        table = gRandomTable;
        index = (*indexPtr + 1) & mask;
        angle = (u8)table[index];
        index = (index + 1) & mask;
        count = (table[index] & smallMask) + 2;
        index = (index + 1) & mask;
        *indexPtr = index;
        speed = (table[index] & smallMask) + 20;
        SparkParticle_Spawn(&graphics->position, 3, &size, angle, count, speed, 2, 2);
    }
}

void GridAttackActor_RequestLinkedActorState(struct GridAttackActor *p)
{
    struct GridAttackPacket *packet = (void *)&p->packet;
    if (p->linkedActorId && p->linkedActorPhase != 0) {
        int zero = 0;
        p->linkedActorPhase = zero;
        packet->id = p->linkedActorId;
        packet->kind = 5;
        packet->zero = 0;
        packet->reserved = zero;
        packet->command = 1;
        packet->count = 1;
        packet->args[0] = 3;
        SceneMessages_Send(packet);
    }
}

void GridAttackActor_NotifyLinkedActorLatePhase(struct GridAttackActor *p)
{
    struct GridAttackPacket *packet = (void *)&p->packet;
    if (p->linkedActorId && p->linkedActorPhase != 1) {
        int zero = 0;
        p->linkedActorPhase = 1;
        packet->id = p->linkedActorId;
        packet->kind = 5;
        packet->zero = 0;
        packet->reserved = zero;
        packet->command = 92;
        packet->count = 0;
        SceneMessages_Send(packet);
    }
}

static inline void GridAttack_SetFacingPinned(struct GridAttackActor *p, int angle)
{
    u32 value;
    register int shifted asm("r0");
    p->transform.angle = angle;
    shifted = p->transform.angle + 0x60;
    asm("" : "+r"(shifted));
    shifted >>= 6;
    value = 3 & shifted;
    p->facing = value;
    shifted = p->transform.angle + 0x70;
    asm("" : "+r"(shifted));
    shifted >>= 5;
    value = 7 & shifted;
    p->facing8 = value;
}

// MATCHING_CONCESSION: register-pinning

void GridAttackActor_UpdateIdle(struct GridAttackActor *p)
{
    if (!p->locked)
        GridAttack_SetFacingPinned(p, p->requestedAngles[0]);
    if (p->changed) {
        p->changed = 0;
        if (p->locked)
            GridAttack_SetFacingPinned(p, p->savedAngle);
        GridAttackActor_SetAnimation(p, 2, 1, 0);
    }
    if (!p->tryUpdate(p)) {
        u32 mask = 64;
        p->flags |= mask;
        p->reset(p);
    }
}

// MATCHING_CONCESSION: register-pinning

void GridAttackActor_UpdateGridAttack(struct GridAttackActor *p, int tick)
{
    if (p->changed) {
        p->changed = 0;
        GridAttack_SetFacingPinned(p, p->requestedAngles[0]);
        GridAttackActor_ResetAttackAnimationHistory(p);
        GridAttackActor_SetAnimation(p, GridAttackActor_SelectAttackAnimation(p), 1, 8);
        Sound_PlaySongChecked(1355);
    }
    if (p->tryUpdate(p))
        return;
    {
        u32 mask = 64;
        p->flags |= mask;
    }
    if (p->reset(p))
        return;
    if (tick == 8)
        GridAttackActor_SpawnGridPattern(p);
    if (p->animationEnded) {
        GridAttackActor_SetAnimation(p, GridAttackActor_SelectAttackAnimation(p), 1, 8);
        if (tick > 31)
            p->finished = 1;
    }
}

// MATCHING_CONCESSION: register-pinning

void GridAttackActor_UpdateElevatedGridAttack(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    if (p->changed) {
        int zero = 0;
        p->changed = zero;
        GridAttack_SetFacingPinned(p, p->requestedAngles[0]);
        GridAttackActor_SetAnimation(p, 3, 1, 8);
        p->stage = zero;
        p->counter = zero;
        GridAttackActor_ResetAttackAnimationHistory(p);
        Sound_PlaySongChecked(955);
    }
    {
        u16 *dst = (u16 *)&p->phaseTimer;
        register int zero asm("r2") = 0;
        int wordZero = 0;
        *dst = 240;
        p->damage = wordZero;
        p->special = zero;
    }
    {
        u32 mask = 12;
        p->flags |= mask;
    }
    if (p->reset(p))
        return;
    switch (p->stage) {
        case 0:
            p->counter++;
            ProjectedDisplay_SetHeight(p->projectedDisplay, p->counter * 4);
            if (p->counter > 31) {
                p->stage++;
                p->counter = 0;
                OrbitContactActor_BeginExpansion(p->orbitContact);
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
                GridAttackActor_SetAnimation(p, 3, 1, 12);
            }
            break;
        case 3:
            if (p->counter == 6)
                GridAttackActor_SpawnGridPattern(p);
        case 4:
        case 5:
        case 6:
            if (p->counter > 5) {
                p->stage++;
                p->counter = 0;
                GridAttackActor_SetAnimation(p, GridAttackActor_SelectAttackAnimation(p), 1, 8);
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

// MATCHING_CONCESSION: register-pinning

void GridAttackActor_UpdateTargetedAttack(struct GridAttackActor *p, int tick)
{
    struct GridAttackActor *base = p;
    if (p->changed) {
        p->changed = 0;
        GridAttack_SetFacingPinned(p, p->requestedAngles[0]);
        GridAttackActor_ResetAttackAnimationHistory(p);
        GridAttackActor_SetAnimation(p, GridAttackActor_SelectAttackAnimation(p), 1, 8);
        gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
        {
            int random = gUnk_086141BC[gEffectRandomTableIndex];
            if (random <= 32767)
                Sound_PlaySongChecked(1356);
        }
    }
    if (p->tryUpdate(p))
        return;
    {
        u32 mask = 64;
        p->flags |= mask;
    }
    if (p->reset(p))
        return;
    if (tick == 8)
        GridAttackActor_SpawnTargetedProjectile(
            base, (struct GridAttackPoint *)p->targetPositions[0], base->targetAttackParameter);
    if (p->animationEnded)
        p->finished = 1;
}

void GridAttackActor_UpdateDamageHold(struct GridAttackActor *p)
{
    if (p->changed) {
        p->changed = 0;
        GridAttackActor_SetAnimation(p, 8, 1, 8);
    }
    p->damage = 0;
    p->special = 0;
    {
        u32 mask = 12;
        u32 flags = p->flags | mask;
        p->flags = flags | 64;
    }
}

void GridAttackActor_UpdateDamageRecovery(struct GridAttackActor *p, int tick)
{
    if (p->changed) {
        p->changed = 0;
        GridAttackActor_SetAnimation(p, 8, 1, 8);
    }
    p->damage = 0;
    p->special = 0;
    {
        u32 mask = 12;
        p->flags |= mask;
    }
    if (!p->reset(p) && tick > 19)
        p->finished = 1;
}

void GridAttack_UpdateStaggerState(struct GridAttackActor *p)
{
    if (p->changed) {
        p->changed = 0;
        GridAttackActor_SetAnimation(p, 8, 1, 8);
        ContactSequencePool_BeginAllDeaths(p->contactPool);
        Sound_PlaySongChecked(1359);
    }
    {
        u8 *dst = &p->reactionCooldown;
        int zero = 0;
        *dst = 6;
        p->damage = zero;
        p->special = zero;
    }
    {
        u32 mask = 12;
        p->flags |= mask;
    }
    if (p->animationEnded)
        GridAttack_SetState((void *)p, GridAttackActor_UpdateIdle, 1);
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

void GridAttack_UpdateDeathState(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    if (p->changed) {
        p->changed = 0;
        GridAttackActor_SetAnimation(p, 10, 1, 8);
        SceneActors_RunFirstEventScript(p, &p->drawable.position);
        ContactSequencePool_BeginAllDeaths(p->contactPool);
        GridAttack_SetDeathEffect(p, 2, 349, 1);
        base->stage = 0;
        base->counter = 0;
        Sound_PlaySongChecked(958);
    }
    if (p->animationEnded && !base->stage) {
        GameState_SetEventFlag(80);
        SceneActors_RunSecondEventScript(p, &base->drawable.position);
        base->stage++;
    }
    {
        u32 mask = 13;
        p->flags |= mask;
    }
}

void GridAttack_UpdateHiddenState(struct GridAttackActor *p)
{
    if (p->changed)
        p->changed = 0;
    p->damage = 0;
    p->special = 0;
    {
        u32 mask = 28;
        p->flags |= mask;
    }
    p->reset(p);
}

void GridAttack_UpdateIdleState(struct GridAttackActor *s)
{
    if (s->changed) {
        s->changed = 0;
        GridAttackActor_SetAnimation(s, 2, 1, 0);
    }
    {
        u32 zero;
        u32 *p = (u32 *)&s->damage;
        zero = 0;
        *p = zero;
        s->special = zero;
    }
    {
        u32 mask = 12;
        u32 *flags = (u32 *)&s->flags;
        *flags |= mask;
    }
    Callback_InvokeActor(s, (void *)s->reset);
}

void GridAttack_UpdateAnimationNineState(struct GridAttackActor *p)
{
    /* Keep the transform base live: direct actor fields change register allocation. */
    struct GridAttackTransform *position = &p->transform;
    if (p->changed) {
        p->changed = 0;
        GridAttackActor_SetAnimation(p, 9, 1, 8);
        p->savedPosition = position->coordinates;
    }
    p->damage = 0;
    p->special = 0;
    {
        u32 mask = 12;
        p->flags |= mask;
    }
    if (!p->reset(p)) {
        p->reactionCooldown = 6;
        if (p->animationEnded)
            p->finished = 1;
    }
}

void GridAttack_UpdateAnimationElevenState(struct GridAttackActor *s)
{
    if (s->changed) {
        s->changed = 0;
        GridAttackActor_SetAnimation(s, 11, 1, 8);
    }
    {
        u32 zero;
        u32 *p = (u32 *)&s->damage;
        zero = 0;
        *p = zero;
        s->special = zero;
    }
    {
        u32 mask = 12;
        u32 *flags = (u32 *)&s->flags;
        *flags |= mask;
    }
    Callback_InvokeActor(s, (void *)s->reset);
}

void GridAttack_UpdateAlternateIdleState(struct GridAttackActor *s)
{
    if (s->changed) {
        s->changed = 0;
        GridAttackActor_SetAnimation(s, 2, 1, 0);
    }
    {
        u32 zero;
        u32 *p = (u32 *)&s->damage;
        zero = 0;
        *p = zero;
        s->special = zero;
    }
    {
        u32 mask = 12;
        u32 *flags = (u32 *)&s->flags;
        *flags |= mask;
    }
    Callback_InvokeActor(s, (void *)s->reset);
}

void GridAttack_UpdateRandomAnimationState(struct GridAttackActor *p, int tick)
{
    if (p->changed) {
        p->changed = 0;
        GridAttackActor_ResetAttackAnimationHistory(p);
        GridAttackActor_SetAnimation(p, GridAttackActor_SelectAttackAnimation(p), 1, 8);
    }
    p->damage = 0;
    p->special = 0;
    {
        u32 mask = 12;
        p->flags |= mask;
    }
    if (!p->reset(p) && p->animationEnded) {
        GridAttackActor_SetAnimation(p, GridAttackActor_SelectAttackAnimation(p), 1, 8);
        if (tick > 31)
            p->finished = 1;
    }
}

void GridAttack_UpdateAnimationThreeState(struct GridAttackActor *p)
{
    if (p->changed) {
        p->changed = 0;
        GridAttackActor_SetAnimation(p, 3, 1, 8);
    }
    p->damage = 0;
    p->special = 0;
    {
        u32 mask = 12;
        p->flags |= mask;
    }
    if (!p->reset(p) && p->animationEnded)
        p->finished = 1;
}

void GridAttack_UpdateFastAnimationThreeState(struct GridAttackActor *p)
{
    if (p->changed) {
        p->changed = 0;
        GridAttackActor_SetAnimation(p, 3, 1, 12);
    }
    p->damage = 0;
    p->special = 0;
    {
        u32 mask = 12;
        p->flags |= mask;
    }
    if (!p->reset(p) && p->animationEnded)
        p->finished = 1;
}

void GridAttack_UpdateTimedHideState(struct GridAttackActor *p, int tick)
{
    struct GridAttackActor *base = p;
    /* Keep the transform base live: direct actor fields change register allocation. */
    struct GridAttackTransform *position = &p->transform;
    int done;
    if (p->changed) {
        p->changed = 0;
        GridAttackActor_SetAnimation(p, 11, 1, 8);
        p->savedPosition = position->coordinates;
    }
    done = 0;
    if (Sprite_BlinkUntilExpired(&done, tick, 10, 30)) {
        u32 mask = 16;
        p->flags |= mask;
    } else if (done) {
        u32 mask = 16;
        p->flags |= mask;
    }
    base->damage = 0;
    base->special = 0;
    {
        u32 mask = 12;
        p->flags |= mask;
    }
    if (!p->reset(p) && tick > 59)
        p->finished = 1;
}

extern u32 gUnk_02000560;

void GridAttack_RunStateIfUnpaused(u32 a)
{
    u32 m = 4;
    if ((gUnk_02000560 & m) == 0)
        SceneActors_RunStateCallback(a);
}

extern const u32 gUnk_08604FA8[];

u32 GridAttack_ApplyRequestedState(struct GridAttackActor *p)
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

int GridAttack_ApplyPendingDamage(struct GridAttackActor *p)
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
                            int one = 1;
                            react = one;
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
            {
                u32 mask = 2;
                base->flags |= mask;
            }
            GridAttack_SetState(base, GridAttack_UpdateDeathState, 8);
            return 1;
        }
        if (special) {
            u32 mask = 0x80000;
            base->flags |= mask;
        }
        if (react) {
            {
                u32 mask = 1;
                base->flags |= mask;
            }
            GridAttack_SetState(base, GridAttack_UpdateStaggerState, 7);
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

int GridAttack_SelectNextAction(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    int trigger = p->forceAction;
    if (trigger) {
        int zero = 0;
        p->forceAction = zero;
        GridAttack_SelectAction(p, 2, zero);
        return 1;
    } else {
        int active = ContactBurst_GetEnabled(p->contactBurst);
        if (active) {
            GridAttack_SelectAction(p, 3, 0);
        } else if (p->attackCount >= p->attackLimit) {
            if (ContactSequencePool_GetActiveCount(p->contactPool) > 0)
                ContactSequencePool_BeginAllDeaths(p->contactPool);
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

static inline void GridAttack_StartActionMode(struct GridAttackActor *p, int mode, int zero, int id)
{
    p->active = mode;
    p->actionReset = zero;
    p->action = id;
    p->actionTimer = zero;
    p->entered = 1;
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

void GridAttack_ProcessMessages(struct GridAttackActor *p)
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

void GridAttack_UpdateStateNineAction(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntry(p)) {
        u32 v = 9;
        p->requested = v;
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

void GridAttack_UpdateIdleAction(struct GridAttackActor *p)
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

void GridAttack_UpdateStateElevenAction(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntryBool(p)) {
        u32 k = 11;
        p->requested = k;
    }
    if (p->finished) {
        GridAttack_StartActionMode(p, 2, 0, 4);
        Trigger_ConsumePending(&p->receiver, 1);
    } else
        p->actionTimer++;
}

void GridAttack_UpdateStateTwelveAction(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntryBool(p)) {
        u32 k = 0xC;
        p->requested = k;
    }
    if (p->finished)
        Trigger_ConsumePending(&p->receiver, 1);
    else
        p->actionTimer++;
}

void GridAttack_UpdateStateThirteenAction(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntryBool(p)) {
        u32 k = 0xD;
        p->requested = k;
    }
    if (p->finished)
        Trigger_ConsumePending(&p->receiver, 1);
    else
        p->actionTimer++;
}

void GridAttack_UpdateAnimationFourteen(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntryBool(p)) {
        u32 k = 14;
        p->requested = k;
    }
    if (p->finished) {
        GridAttack_StartActionMode(p, 2, 0, 4);
        Trigger_ConsumePending(&p->receiver, 1);
    } else
        p->actionTimer++;
}

void GridAttack_UpdateAnimationFifteen(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntryBool(p)) {
        u32 k = 0xF;
        p->requested = k;
    }
    if (p->finished)
        Trigger_ConsumePending(&p->receiver, 1);
    else
        p->actionTimer++;
}

void GridAttack_UpdateAnimationSixteen(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntryBool(p)) {
        u32 k = 16;
        p->requested = k;
    }
    if (p->finished) {
        GridAttack_StartActionMode(p, 2, 0, 4);
        Trigger_ConsumePending(&p->receiver, 1);
    } else
        p->actionTimer++;
}

void GridAttack_UpdateAnimationSeventeen(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntryBool(p)) {
        u32 k = 0x11;
        p->requested = k;
    }
    if (p->finished) {
        u32 two = 2;
        u32 z = 0;
        p->active = two;
        p->actionReset = z;
        p->action = z;
        p->actionTimer = z;
        p->entered = 1;
        Trigger_ConsumePending(&p->receiver, 1);
    } else
        p->actionTimer++;
}

extern const u32 gUnk_08604FF0[];

void GridAttack_DispatchSpecialAnimation(struct GridAttackActor *p)
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

int GridAttack_TryStartDamageReaction(struct GridAttackActor *p)
{
    if (p->health <= 0 && p->healthCheck) {
        u32 mask = 2;
        p->flags |= mask;
        GridAttack_SetState(p, GridAttack_UpdateDeathState, 8);
    }
    p->healthCheck = 0;
    {
        u32 mask = 2;
        if (p->flags & mask) {
            GridAttack_StartReaction(p, 1, 3, 0);
            return 1;
        } else if (p->flags & 1) {
            GridAttack_StartReaction(p, 1, 2, 0);
            return 1;
        }
    }
    return 0;
}

void GridAttack_UpdateWaitAction(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    p->healthCheck = 1;
    if (GridAttack_TakeActionEntryBool(p) == 1)
        base->requested = 1;
    if (base->actionTimer > p->actionDuration) {
        if (GridAttack_SelectNextAction(p))
            return;
    }
    base->actionTimer++;
}

void GridAttack_UpdateActionTwo(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    p->healthCheck = 1;
    if (GridAttack_TakeActionEntryBool(p)) {
        u32 id = 2;
        p->requested = id;
    }
    if (p->finished) {
        int duration = 60, zero = 0;
        base->active = zero;
        base->actionReset = zero;
        base->action = zero;
        base->actionTimer = zero;
        base->entered = 1;
        base->actionDuration = duration;
    } else
        p->actionTimer++;
}

void GridAttack_UpdateDeathAction(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    p->healthCheck = 1;
    if (GridAttack_TakeActionEntryBool(p)) {
        u32 id = 3;
        p->requested = id;
    }
    if (p->finished) {
        int duration = 60, zero = 0;
        base->active = zero;
        base->actionReset = zero;
        base->action = zero;
        base->actionTimer = zero;
        base->entered = 1;
        base->actionDuration = duration;
    } else
        p->actionTimer++;
}

void GridAttack_UpdateAttackAction(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    p->healthCheck = 1;
    if (GridAttack_TakeActionEntryBool(p)) {
        u32 id = 4;
        p->requested = id;
    }
    if (p->finished) {
        int duration, zero;
        int value = p->attackRange;
        if (value < 64)
            duration = base->farAttackDelay;
        else
            duration = base->nearAttackDelay;
        zero = 0;
        base->active = zero;
        base->actionReset = zero;
        base->action = zero;
        base->actionTimer = zero;
        base->entered = 1;
        base->actionDuration = duration;
    } else
        p->actionTimer++;
}

void GridAttack_UpdateAnimationFive(struct GridAttackActor *p)
{
    if (GridAttack_TakeActionEntry(p)) {
        u32 v = 5;
        p->requested = v;
    }
    p->actionTimer++;
}

void GridAttack_UpdateRecoveryAction(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    if (GridAttack_TakeActionEntryBool(p)) {
        u32 id = 6;
        p->requested = id;
    }
    if (p->finished) {
        int duration = 30, zero = 0;
        base->active = zero;
        base->actionReset = zero;
        base->action = zero;
        base->actionTimer = zero;
        base->entered = 1;
        base->actionDuration = duration;
    }
    p->actionTimer++;
}

void GridAttack_UpdateDamageAction(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    if (GridAttack_TakeActionEntryBool(p) == 1) {
        u32 id = 7;
        p->requested = id;
        base->forceAction = 1;
    }
    {
        u32 mask = 1;
        if (!(p->flags & mask)) {
            int duration = 30, zero = 0;
            base->active = zero;
            base->actionReset = zero;
            base->action = zero;
            base->actionTimer = zero;
            base->entered = 1;
            base->actionDuration = duration;
        } else
            p->actionTimer++;
    }
}

void GridAttack_UpdatePassiveAction(struct GridAttackActor *p)
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

void GridAttack_DispatchNormalAction(struct GridAttackActor *p)
{
    Callback_InvokeActor(p, (void *)gUnk_08605014[p->action]);
}

extern const u32 gUnk_08605024[];

void GridAttack_DispatchReactionAction(struct GridAttackActor *p)
{
    Callback_InvokeActor(p, (void *)gUnk_08605024[p->action]);
}

extern const u32 gUnk_08605034[];

void GridAttack_UpdateNormalActions(struct GridAttackActor *p)
{
    u32 m = 4;
    if (!(gUnk_02000560 & m)) {
        if (GridAttack_TryStartDamageReaction(p) == 0) {
            Callback_InvokeActor(p, (void *)gUnk_08605034[p->actionReset]);
        }
    }
}

extern const u32 gUnk_08605038[];

void GridAttack_UpdateReactionActions(struct GridAttackActor *p)
{
    Callback_InvokeActor(p, (void *)gUnk_08605038[p->actionReset]);
}

extern const u32 gUnk_0860503C[];

void GridAttack_UpdateSpecialActions(struct GridAttackActor *p)
{
    Callback_InvokeActor(p, (void *)gUnk_0860503C[p->actionReset]);
}

extern u32 gUnk_08605040[];

void GridAttack_UpdateActionController(struct GridAttackActor *p)
{
    p->controllerReset = 0;
    Callback_InvokeActor(p, (void *)gUnk_08605040[p->active]);
}

s32 ProjectedDisplay_GetHeight(void *);
void SceneActors_UpdateTargetMetrics(void *);
static inline s32 GridAttack_Truncate12(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

static inline u32 GridAttack_TestFlags(struct GridAttackActor *actor, u32 mask)
{
    return actor->flags & mask;
}

void GridAttack_UpdateAnimationAndTargets(struct GridAttackActor *p)
{
    struct GridAttackActor *actor = p;
    struct GridAttackActor *owner = p;
    struct GridAttackTransform *body = &actor->transform;
    s32 height;
    u32 i;
    GridAttack_ProcessMessages(actor);
    Obj_StepPosition(body);
    owner->animationEnded =
        PlayerCue_StepSequenceEnded((void *)&actor->drawable, &actor->animation);
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

#ifdef NONMATCHING
// fuzzy: 54.2%
// fix.py best (sweep after the inlined-insn fix)
// Collider flag flow, palette branch layout and register allocation still differ.
s32 ContactSequencePool_GetActiveCount(void *);
void ContactBurst_SetEnabled(void *, s32);
s32 ContactBurst_GetEnabled(void *);
void SceneActors_ConfigureEffect(void *, s32, s32, s32);
void SceneActors_UpdateEffects(void *);
void CameraTracking_Update(void *);
void GridAttackActor_EmitSpriteParticles(struct GridAttackActor *);
void Palette_InterpolateBanks(void *, const void *, const void *, s32, s32);
void GridAttack_UpdateCollidersAndPalette(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    struct GridAttackTransform *transform = &p->transform;
    struct ContactCollider *damage;
    struct ContactCollider *body;
    if (ContactSequencePool_GetActiveCount(p->contactPool) > p->contactCountLimit) {
        ContactBurst_SetEnabled(p->contactBurst, 1);
        p->flags |= 4;
        p->attackCount = 0;
    } else {
        if ((!p->attackCount) && ContactBurst_GetEnabled(p->contactBurst)) {
            SceneActors_ConfigureEffect(p, 1, 0, 0);
            Sound_PlaySongChecked(0x54D);
        }
        ContactBurst_SetEnabled(base->contactBurst, 0);
        if (base->attackCount < base->attackLimit)
            base->attackCount++;
    }
    if (ContactBurst_GetEnabled(base->contactBurst))
        p->flags &= ~0x40;
    SceneActors_UpdateEffects(p);
    if (base->stun) {
        p->flags |= 4;
        base->stun--;
    }
    damage = &base->damageCollider;
    if (p->flags & 4)
        (&base->damageCollider)->flags |= 4;
    else {
        (&base->damageCollider)->flags &= ~4;
        Collider_SetPosition(&base->damageCollider, &base->drawable.position, 0);
        (&base->damageCollider)->direction = transform->angle;
    }
    body = &base->bodyCollider;
    if (!(p->flags & 8)) {
        Collider_SetPosition(body, &base->drawable.position, 0);
        body->direction = transform->angle;
        Collider_Refresh(body);
    }
    if (p->flags & 16) {
        *((u32 *)base->drawable.sprite) |= 1;
        *((u32 *)(((u8 *)base) + 0x8F0)) &= ~1;
        (&base->damageCollider)->flags &= ~8;
    } else {
        *((u32 *)base->drawable.sprite) &= ~1;
        if (p->locked)
            *((u32 *)(((u8 *)base) + 0x8F0)) &= ~1;
        else
            *((u32 *)(((u8 *)base) + 0x8F0)) |= 1;
        if (p->flags & 32)
            (&base->damageCollider)->flags |= 8;
        else
            (&base->damageCollider)->flags &= ~8;
    }
    if (base->shake) {
        base->shake--;
        p->flags |= 0x20000;
    }
    if (base->flash > 0) {
        if (!(--base->flash))
            base->animData.palette = base->palette;
    } else {
        u8 *palette = base->palette;
        switch (base->effectMode) {
            case 1:
                if (base->effectVariant >= (1 << base->effectDuration))
                    break;
                base->effectVariant++;
                Palette_InterpolateBanks(palette, gPaletteBankData,
                                         gPaletteBankData + (base->effectId << 5),
                                         base->effectVariant, base->effectDuration);
                base->animData.palette = palette;
                break;

            case 2:
                if (base->effectVariant > 0) {
                    base->effectVariant--;
                    Palette_InterpolateBanks(palette, gPaletteBankData,
                                             gPaletteBankData + (base->effectId << 5),
                                             base->effectVariant, base->effectDuration);
                } else {
                    base->effectVariant = 0;
                    base->effectMode = 0;
                    base->effectId = 1;
                    CpuSet(gPaletteBankData + 0x2BA0, palette, 0x04000008);
                }
                base->animData.palette = palette;
                break;

            case 3:
                Palette_InterpolateBanks(palette, gPaletteBankData,
                                         gPaletteBankData + (base->effectId << 5),
                                         base->effectVariant, base->effectDuration);
                base->animData.palette = palette;
                if (!base->effectDirection) {
                    if ((++base->effectVariant) > 31) {
                        base->effectVariant = 32;
                        base->effectDirection = 1;
                    }
                } else if ((--base->effectVariant) <= 0) {
                    base->effectVariant = 0;
                    base->effectDirection = 0;
                }
                break;
        }
    }
    if (base->projectedDisplay)
        base->drawable.sprite[7] = 1;
    else
        base->drawable.sprite[7] = 2;
    if (!p->locked)
        CameraTracking_Update(((u8 *)base) + 0x8D0);
    if ((p->locked && base->messageParameter) && (!(p->effectTimer & 15)))
        GridAttackActor_EmitSpriteParticles(base);
    p->contactSource = 0;
    p->contactFlags = 0;
    p->effectTimer++;
}

#else
INCLUDE_ASM("asm/nonmatching", GridAttack_UpdateCollidersAndPalette);
#endif

int GridAttack_Update(struct GridAttackActor *p)
{
    u16 *t;
    GridAttack_UpdateAnimationAndTargets(p);
    GridAttack_UpdateActionController(p);
    GridAttack_RunStateIfUnpaused((u32)p);
    GridAttack_UpdateCollidersAndPalette(p);
    if (p->phaseTimer <= 0xef)
        GridAttackActor_RequestLinkedActorState(p);
    else
        GridAttackActor_NotifyLinkedActorLatePhase(p);
    t = (u16 *)&p->phaseTimer;
    (*t)++;
    if (*t > 0x12b)
        *t = 0;
    return 0;
}

u32 GridAttackActor_Destroy(struct GridAttackActor *p)
{
    Collider_Remove(&p->damageCollider);
    GridAttackActor_RemoveSprite(p);
    SceneActors_Destroy(p);
    return 0;
}

void SceneActors_Register(void *, s32);
s32 SceneActors_Initialize(void *, u32, s32, void *, s32);
void SceneActors_SetHealthLimits(void *, s32, s32, s32);
void SceneActors_SetEventScripts(void *, s32, s32);
void SceneActors_SetDamageCallback(void *, void *, void *, s32);
void CameraFollow_Initialize(void *, u32, void *, u32);
void *ContactSequencePool_Create(s32, u32, s32, u32, u32);
void *OrbitContactActor_Create(const struct ContactPosition *, u32);

// Sets the mode fields for the scripted start states (shared tail of the switch cases).
static inline void Sub0802BFAC_ResetMode(struct GridAttackActor *p)
{
    p->timer = 0;
    p->changed = 1;
    p->stateReset = 0;
    p->pending = 0;
}

#ifdef NONMATCHING
// fuzzy: 53.3%
// scope shape 243 (differences once low registers are set aside)
// confidence=low: builds a grid attack actor from its script keywords and starts its idle state.
s32 GridAttackActor_Initialize(struct GridAttackActor *arg0, u32 arg1)
{
    struct GridAttackActor *p = arg0;
    struct GridAttackPacked pk;
    u32 ctx = arg1;
    s32 kind;
    s32 count;
    s32 i;
    s32 health;
    u8 facing;
    s32 poolHealth;
    s32 poolValue;
    s32 poolResource;
    s32 orbitValue;
    u8 *base;

    kind = Script_GetKeywordValue(0x74, 3);
    if (kind != 3)
        return -1;
    SceneActors_Register(p, 3);
    p->locked = Script_GetKeywordValue(0x69, 0);
    count = Script_GetKeywordValue(0x50, 1);
    p->targetCount = count;
    {
        u32 *target = (u32 *)&p->targetPositions[0];
        u32 *slot = (u32 *)&p->damageSource;
        for (i = 0; i < count; i++) {
            u32 value = (u32)gPlayers[i];
            if (value) {
                *slot = value;
                value += 0x30;
            } else
                *slot = value;
            *target = value;
            target++;
            slot++;
        }
    }

    if (Script_SeekToKeyword(0x70)) {
        pk.x = Script_GetValue();
        pk.y = Script_GetValue();
        pk.z = Script_GetValue();
        facing = ((Script_GetValue() & 7) << 5) + 0xA0;
    } else {
        pk.x = *(u16 *)((u8 *)gGameState + 0x30);
        pk.y = *(u16 *)((u8 *)gGameState + 0x32);
        pk.z = *(u16 *)((u8 *)gGameState + 0x34);
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

    SceneActors_Initialize(p, ctx, kind, &pk, facing);
    AnimData_Load(&p->animData, 0x4147);
    Sprite_Init(&p->drawable, &p->animData, 1);
    *(u32 *)p->drawable.sprite &= ~2;
    *(struct GridAttackPoint *)&p->drawable.position = *(struct GridAttackPoint *)&pk;
    p->drawable.sprite[7] = 1;
    p->resource = ScriptProc_Start(0x922E, 0xC5E2);
    Anim_SetSequence(&p->animation, p->resource, 2, 0, 0);

    if (p->locked) {
        switch (Script_GetKeywordValue(0x44, 0)) {
            case 0:
                p->active = 2;
                p->actionReset = 0;
                p->action = 1;
                p->actionTimer = 0;
                p->entered = 1;
                p->state = 10;
                p->callback = (void (*)(void *))GridAttack_UpdateIdleState;
                Sub0802BFAC_ResetMode(p);
                break;
            case 1:
                p->active = 2;
                p->actionReset = 0;
                p->action = 0;
                p->actionTimer = 0;
                p->entered = 1;
                p->state = 9;
                p->callback = (void (*)(void *))GridAttack_UpdateHiddenState;
                Sub0802BFAC_ResetMode(p);
                break;
            case 2:
                p->active = 2;
                p->actionReset = 0;
                p->action = 4;
                p->actionTimer = 0;
                p->entered = 1;
                p->state = 13;
                p->callback = (void (*)(void *))GridAttack_UpdateAlternateIdleState;
                Sub0802BFAC_ResetMode(p);
                break;
            case 3:
                p->active = 2;
                p->actionReset = 0;
                p->action = 3;
                p->actionTimer = 0;
                p->entered = 1;
                p->state = 12;
                p->callback = (void (*)(void *))GridAttack_UpdateAnimationElevenState;
                Sub0802BFAC_ResetMode(p);
                break;
        }
    } else {
        p->active = 0;
        p->actionReset = 0;
        p->action = 0;
        p->actionTimer = 0;
        p->entered = 1;
        p->actionDuration = 90;
        p->state = 1;
        p->callback = (void (*)(void *))GridAttackActor_UpdateIdle;
        p->stateReset = 0;
        Sub0802BFAC_ResetMode(p);
    }

    p->tryUpdate = (void *)GridAttack_ApplyPendingDamage;
    p->reset = (void *)GridAttack_ApplyRequestedState;
    SceneActors_SetHealthLimits(p, health, health, p->bodyMode);
    GridAttackActor_InitBodyCollider(p);
    GridAttackActor_InitDamageCollider(p, p->bodyMode);

    if (Script_SeekToKeyword(0x63)) {
        s32 a = Script_GetValue();
        s32 b = Script_GetValue();
        SceneActors_SetEventScripts(p, a, b);
        ctx = Script_GetValue();
    } else {
        SceneActors_SetEventScripts(p, 0, 0);
        ctx = 0;
    }

    if (Script_SeekToKeyword(0x41)) {
        for (i = 0; i <= 3 && Script_GetPc() != 0; i++)
            p->scriptArgs[i] = Script_GetValue();
    }

    p->contactReward = Script_GetKeywordValue(0x53, 0);

    base = gPaletteBankData;
    for (i = 0; i < 16; i++)
        CpuSet(base + 0x2BA0 + i * 0x20, p->palette + i * 0x20, 0x04000008);
    base = gPaletteBankData;
    for (i = 0; i < 16; i++)
        CpuSet(base + 0x3240, p->normalDamageGraphics + i * 0x20, 0x04000008);
    base = gPaletteBankData;
    for (i = 0; i < 16; i++)
        CpuSet(base + 0x3240, p->specialDamageGraphics + i * 0x20, 0x04000008);

    *(u8 **)((u8 *)p + 0x878) = p->palette;
    p->effectMode = 0;
    p->effectId = 0x15D;
    p->effectVariant = 0;
    p->effectDuration = 5;

    *(struct GridAttackPoint *)&p->shadowPosition =
        *(struct GridAttackPoint *)&p->drawable.position;
    CameraFollow_Initialize(p->shadow, p->targetPositions[0], &p->shadowPosition, 1);
    ((u32 *)p->shadow)[4] = 0;
    ((u32 *)p->shadow)[5] = 0x8C;

    SceneActors_SetDamageCallback(p, (u8 *)p + 0x818, (void *)GridAttackActor_OnDamage, 0);
    if (Script_GetKeywordValue(0x43, 1) != 0)
        p->projectedDisplay = ProjectedDisplay_Create(&p->transform.coordinates);
    else
        p->projectedDisplay = 0;

    p->contactPool =
        ContactSequencePool_Create(poolHealth, poolValue, poolResource, ctx, (s16)p->contactReward);
    p->orbitContact = OrbitContactActor_Create(&p->transform.coordinates, orbitValue);
    p->contactBurst = ContactBurst_Create((struct ContactPosition *)&p->drawable.position);
    p->linkedActorId = Script_GetKeywordValue(0x54, 0);
    p->linkedActorPhase = 0;
    p->phaseTimer = 0;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", GridAttackActor_Initialize);
#endif

struct GridAttackActor *GridAttackActor_Create(u32 a)
{
    struct GridAttackActor *p;
    Script_GetKeywordValue(0x74, 3);
    p = SceneActors_GetBySlot();
    if (p != 0)
        return p;
    p = Task_Create(8, 0x924);
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))GridAttack_Update,
                          (void (*)(void *))GridAttackActor_Destroy);
        if (GridAttackActor_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
