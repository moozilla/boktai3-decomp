#include "grid_attack_actor.h"

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

#ifdef NONMATCHING
// fuzzy: 97.1%
// The signed-x offset temporary uses r1 instead of r3 (two bytes differ).
void GridAttackActor_OnDamageCollision(struct ContactCollider *source,
                                       struct ContactCollider *other, struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    int angle;
    Collision_CalculateDamageResponse(source, other);
    PlayerStats_AddContactHundredths(source, other, p->contactReward);
    p->damage = other->cost;
    {
        u32 mask = 0x100;
        if (source->flags & mask)
            angle = source->direction;
        else {
            int x = other->x - source->x;
            int z = other->z - source->z;
            angle = Math_Atan2(x, z);
        }
    }
    p->damageAngle = angle;
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
#else
INCLUDE_ASM("asm/nonmatching", GridAttackActor_OnDamageCollision);
#endif

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
    Collider_SetPosition(collider, p->drawable.position, 0);
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
    Collider_SetPosition(collider, p->drawable.position, 0);
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
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return sub_08249350(table[gUnk_03005308], 3);
}

void GridAttackActor_SpawnGridPattern(struct GridAttackActor *p)
{
    if ((p->targetPosition)[2] < 0xc00) {
        GridAttackActor_SpawnGridContact(p, 0, 0);
        GridAttackActor_SpawnGridContact(p, 2, 0);
        GridAttackActor_SpawnGridContact(p, 0, 1);
        GridAttackActor_SpawnGridContact(p, 1, 1);
        GridAttackActor_SpawnGridContact(p, 2, 1);
        {
            u16 *table = gUnk_0203B400;
            GridAttackActor_SpawnGridContact(p, GridAttack_RandomColumn(table), 2);
            GridAttackActor_SpawnGridContact(p, GridAttack_RandomColumn(table), 3);
            GridAttackActor_SpawnGridContact(p, GridAttack_RandomColumn(table), 4);
        }
    } else {
        GridAttackActor_SpawnGridContact(p, 0, 1);
        GridAttackActor_SpawnGridContact(p, 1, 1);
        GridAttackActor_SpawnGridContact(p, 2, 1);
        {
            u16 *table = gUnk_0203B400;
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

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Stack copy scheduling, random masks and register lifetimes still differ.
void *BouncingObject_Spawn(void *, void *, void *, s32, s32, s32, s32);
void GridAttackActor_SpawnTargetedProjectile(struct GridAttackActor *actor,
                                             struct GridAttackPoint *point, int kind)
{
    struct GridAttackActor *p = actor;
    u32 zero;
    struct GridAttackPoint velocity;
    struct SceneVec position;
    struct GridAttackPoint target;
    struct GridAttackPoint original;
    register struct GridAttackPoint *vel asm("r7") = &velocity;
    volatile s16 *pos;
    zero = 0;
    CpuSet(&zero, vel, 0x05000002);
    pos = (volatile s16 *)(&position);
    original = *point;
    *((struct GridAttackPoint *)(&position)) = original;
    {
        const u16 *table = gUnk_0203B400;
        register u32 *randomIndex asm("r8") = &gUnk_03005308;
        register u32 index asm("r2");
        register s32 upper asm("r4") = ~255;
        register u32 mask asm("r5") = 63;
        u32 sample;
        index = ((*randomIndex) + 1) & 1023;
        sample = table[index];
        {
            u8 k2 = 128;
            pos[0] = ((pos[0] & upper) + ((sample & mask) + k2)) - 32;
        }
        pos[1] += 1024;
        index = (index + 1) & 1023;
        *randomIndex = index;
        sample = table[index];
        {
            u8 k1 = 128;
            pos[2] = ((pos[2] & upper) + ((sample & mask) + k1)) - 32;
        }
    }
    target = original;
    BouncingObject_Spawn(&position, vel, &target, p->projectileDamage, 50, kind, 0);
}

#else
INCLUDE_ASM("asm/nonmatching", GridAttackActor_SpawnTargetedProjectile);
#endif

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
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    {
        u32 offset = gUnk_03005308 * 2;
        {
            int value = *(u16 *)((u8 *)gUnk_0203B400 + offset);
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
        indexPtr = &gUnk_03005308;
        mask = 1023;
        smallMask = 3;
        table = gUnk_0203B400;
        index = (*indexPtr + 1) & mask;
        angle = (u8)table[index];
        index = (index + 1) & mask;
        count = (table[index] & smallMask) + 2;
        index = (index + 1) & mask;
        *indexPtr = index;
        speed = (table[index] & smallMask) + 20;
        SparkParticle_Spawn(graphics->position, 3, &size, angle, count, speed, 2, 2);
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
        GridAttack_SetFacingPinned(p, p->requestedAngle);
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
        GridAttack_SetFacingPinned(p, p->requestedAngle);
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
        GridAttack_SetFacingPinned(p, p->requestedAngle);
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

extern u32 gUnk_03005304;

extern u16 gUnk_086141BC[];

// MATCHING_CONCESSION: register-pinning

void GridAttackActor_UpdateTargetedAttack(struct GridAttackActor *p, int tick)
{
    struct GridAttackActor *base = p;
    if (p->changed) {
        p->changed = 0;
        GridAttack_SetFacingPinned(p, p->requestedAngle);
        GridAttackActor_ResetAttackAnimationHistory(p);
        GridAttackActor_SetAnimation(p, GridAttackActor_SelectAttackAnimation(p), 1, 8);
        gUnk_03005304 = (gUnk_03005304 + 1) & 1023;
        {
            int random = gUnk_086141BC[gUnk_03005304];
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
        GridAttackActor_SpawnTargetedProjectile(base, (struct GridAttackPoint *)p->targetPosition,
                                                base->targetAttackParameter);
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
        SceneActors_RunFirstEventScript(p, p->drawable.position);
        ContactSequencePool_BeginAllDeaths(p->contactPool);
        GridAttack_SetDeathEffect(p, 2, 349, 1);
        base->stage = 0;
        base->counter = 0;
        Sound_PlaySongChecked(958);
    }
    if (p->animationEnded && !base->stage) {
        GameState_SetEventFlag(80);
        SceneActors_RunSecondEventScript(p, base->drawable.position);
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
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        {
            u32 index = gUnk_03005308 * 2;
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

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Receiver loop, action constants and pointer lifetimes still differ.
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
void GridAttack_ProcessMessages(struct GridAttackActor *p)
{
    struct GridAttackActor *base = p;
    struct SceneEventReceiver *receiver = &p->receiver;
    s32 i = 0;
    typeof(*((u16 *)(((u8 *)p) + 0x54))) t1;
    typeof((*((u16 *)(((u8 *)p) + 0x56))) + base->height) t2;
    while (i < receiver->count[gUnk_0200047C->phase]) {
        struct SceneEventCommand *command = receiver->pending[gUnk_0200047C->phase][i];
        s32 action;
        SceneMessages_SetPendingTrigger(receiver, command);
        switch (command->kind) {
            case 0:
                GridAttack_StartMessageAction(p, 2, 0, 0);
                break;

            case 1:
                p->savedAngle = (((u8 *)(&command->value))[0] + 5) << 5;
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
                *((u16 *)(((u8 *)p) + 0x54)) = *((u16 *)(((u8 *)command) + 8));
                *((u16 *)(((u8 *)p) + 0x56)) = *((u16 *)(((u8 *)command) + 10));
                *((u16 *)(((u8 *)p) + 0x58)) = *((u16 *)(((u8 *)command) + 12));
                t1 = *((u16 *)(((u8 *)p) + 0x54));
                *((u16 *)(((u8 *)base) + 0x8A4)) = t1;
                t2 = (*((u16 *)(((u8 *)p) + 0x56))) + base->height;
                *((u16 *)(((u8 *)base) + 0x8A6)) = t2;
                *((u16 *)(((u8 *)base) + 0x8A8)) = *((u16 *)(((u8 *)p) + 0x58));
                Trigger_ConsumePending(&p->receiver, 1);
                break;
            }
        }

        i++;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", GridAttack_ProcessMessages);
#endif

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

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Packed playback, coordinate addressing and target-array allocation still differ.
// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "scene_vec.h"
// Drawable layout shared with ActorDisplay_UpdateForegroundSprite.
struct Drawable {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    u32 unknown3C;
    s32 ended;
    u32 mode;
};
static inline u32 GridAttack_StepPlayback(struct Drawable *owner,
                                          struct DisplaySequenceState *state)
{
    struct ContactSwarmSprite *sprite = &owner->sprite;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended, duration, flags, bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        register u32 maskBit asm("r3") = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
    }
    {
        u32 input = state->flags;
        register u32 bit asm("r3");
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 8;
        else
            owner->sprite.flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */
        asm("" : "+&r"(mask) : "r"(temp));
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                    ended = 1;
                } else {
                    ended = 0;
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}

s32 ProjectedDisplay_GetHeight(void *);
void SceneActors_UpdateTargetMetrics(void *);
static inline s32 GridAttack_Truncate12(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
static inline u32
GridAttack_StepPlayback_UpdateAnimationAndTargets(struct Drawable *owner,
                                                  struct DisplaySequenceState *state)
{
    struct ContactSwarmSprite *sprite = &owner->sprite;
    const volatile u16 *command; /* preserve observed packed-command reads */
    u32 ended;
    u32 duration;
    u32 flags;
    u32 bit;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        register u32 maskBit asm("r3") = 1;
        bit = maskBit;
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
    }
    {
        u32 input = state->flags;
        register u32 bit asm("r3");
        register u32 first asm("r1");
        bit = 2;
        first = bit;
        first &= input;
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 8;
        else
            owner->sprite.flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r1");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        /* Keep the live mask distinct from its source without binding r7. */
        asm("" : "+&r"(mask) : "r"(temp));
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                    ended = 1;
                } else {
                    ended = 0;
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else {
            ended = 0;
        }
    }
    return ended;
}

void GridAttack_UpdateAnimationAndTargets(struct GridAttackActor *input)
{
    struct GridAttackActor *p = input;
    register struct GridAttackActor *base asm("sb") = p;
    struct GridAttackTransform *transform = &p->transform;
    struct SceneVec *position = (void *)(&transform->coordinates);
    struct SceneVec *draw = (void *)p->drawable.position;
    s32 height;
    u32 i;
    asm("" : "+r"(p), "+r"(base));
    GridAttack_ProcessMessages(p);
    Obj_StepPosition(transform);
    base->animationEnded = GridAttack_StepPlayback_UpdateAnimationAndTargets(
        (struct Drawable *)(&p->drawable), &p->animation);
    if (p->projectedDisplay)
        height = ProjectedDisplay_GetHeight(p->projectedDisplay);
    else
        height = 0;
    *((s16 *)(((u8 *)p) + 0x8A4)) = position->x;
    *((s16 *)(((u8 *)p) + 0x8A6)) = (position->y + height) + 128;
    *((s16 *)(((u8 *)p) + 0x8A8)) = position->z;
    p->shadowPosition = *((struct GridAttackPoint *)draw);
    if (base->flags & 0x20000) {
        *((s16 *)(((u8 *)p) + 0x8A4)) +=
            GridAttack_Truncate12(gUnk_086149C4[(p->shakeAngle + 64) & 255] * (p->shake << 2));
        *((s16 *)(((u8 *)p) + 0x8A8)) +=
            GridAttack_Truncate12(gUnk_086149C4[p->shakeAngle] * (p->shake << 2));
    }
    if (p->reactionCooldown) {
        u32 angle;
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        angle = gUnk_0203B400[gUnk_03005308];
        *((s16 *)(((u8 *)p) + 0x8A4)) +=
            GridAttack_Truncate12(gUnk_086149C4[(angle + 64) & 255] * (p->reactionCooldown << 2));
        *((s16 *)(((u8 *)p) + 0x8A8)) +=
            GridAttack_Truncate12(gUnk_086149C4[angle & 255] * (p->reactionCooldown << 2));
        p->reactionCooldown--;
    }
    SceneActors_UpdateTargetMetrics(base);
    for (i = 0; i < p->targetCount; i++) {
        s16 *target = ((s16 **)(((u8 *)base) + 0x110))[i];
        if (target) {
            s32 x = target[0] - position->x;
            s32 z = target[2] - position->z;
            do {
                (((u8 *)base) + 0xCC)[i] = Math_Atan2(x, z);
            } while (0);
            do {
                ((s32 *)(((u8 *)base) + 0xC8))[i] = (x * x) + (z * z);
            } while (0);
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", GridAttack_UpdateAnimationAndTargets);
#endif

#ifdef NONMATCHING
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
        Collider_SetPosition(&base->damageCollider, base->drawable.position, 0);
        (&base->damageCollider)->direction = transform->angle;
    }
    body = &base->bodyCollider;
    if (!(p->flags & 8)) {
        Collider_SetPosition(body, base->drawable.position, 0);
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
                Palette_InterpolateBanks(palette, gUnk_030042E4,
                                         gUnk_030042E4 + (base->effectId << 5), base->effectVariant,
                                         base->effectDuration);
                base->animData.palette = palette;
                break;

            case 2:
                if (base->effectVariant > 0) {
                    base->effectVariant--;
                    Palette_InterpolateBanks(palette, gUnk_030042E4,
                                             gUnk_030042E4 + (base->effectId << 5),
                                             base->effectVariant, base->effectDuration);
                } else {
                    base->effectVariant = 0;
                    base->effectMode = 0;
                    base->effectId = 1;
                    CpuSet(gUnk_030042E4 + 0x2BA0, palette, 0x04000008);
                }
                base->animData.palette = palette;
                break;

            case 3:
                Palette_InterpolateBanks(palette, gUnk_030042E4,
                                         gUnk_030042E4 + (base->effectId << 5), base->effectVariant,
                                         base->effectDuration);
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

INCLUDE_ASM("asm/nonmatching", GridAttackActor_Initialize);

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
