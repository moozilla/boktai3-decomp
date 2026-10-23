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
// fuzzy: 67.0%
// Stack copy scheduling, random masks and register lifetimes still differ.
void *BouncingObject_Spawn(void *, void *, void *, s32, s32, s32, s32);
void GridAttackActor_SpawnTargetedProjectile(struct GridAttackActor *actor,
                                             struct GridAttackPoint *point, int kind)
{
    register struct GridAttackActor *p asm("sl") = actor;
    u32 zero;
    struct GridAttackPoint velocity;
    struct SceneVec position;
    struct GridAttackPoint target;
    struct GridAttackPoint original;
    register struct GridAttackPoint *vel asm("r7") = &velocity;
    volatile s16 *pos;
    asm("" : : "r"(p), "r"(vel));
    zero = 0;
    CpuSet(&zero, vel, 0x05000002);
    pos = (volatile s16 *)&position;
    original = *point;
    *(struct GridAttackPoint *)&position = original;
    {
        register const u16 *table asm("sb") = gUnk_0203B400;
        register u32 *randomIndex asm("r8") = &gUnk_03005308;
        register u32 index asm("r2");
        register u32 indexMask asm("r6") = 1023;
        register s32 upper asm("r4") = ~255;
        register u32 mask asm("r5") = 63;
        u32 sample;
        asm("" : : "r"(table), "r"(randomIndex));
        index = (*randomIndex + 1) & indexMask;
        asm("" : : "r"(indexMask));
        sample = table[index];
        asm("" : : "r"(upper), "r"(mask));
        pos[0] = (pos[0] & upper) + ((sample & mask) + 128) - 32;
        pos[1] += 1024;
        index = (index + 1) & indexMask;
        *randomIndex = index;
        sample = table[index];
        pos[2] = (pos[2] & upper) + ((sample & mask) + 128) - 32;
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
// fuzzy: 73.6%
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
    while (i < receiver->count[gUnk_0200047C->phase]) {
        struct SceneEventCommand *command = receiver->pending[gUnk_0200047C->phase][i];
        s32 action;
        SceneMessages_SetPendingTrigger(receiver, command);
        switch (command->kind) {
            case 0:
                GridAttack_StartMessageAction(p, 2, 0, 0);
                break;
            case 1:
                p->savedAngle = (((u8 *)&command->value)[0] + 5) << 5;
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
                *(u16 *)((u8 *)p + 0x54) = *(u16 *)((u8 *)command + 8);
                *(u16 *)((u8 *)p + 0x56) = *(u16 *)((u8 *)command + 10);
                *(u16 *)((u8 *)p + 0x58) = *(u16 *)((u8 *)command + 12);
                *(u16 *)((u8 *)base + 0x8A4) = *(u16 *)((u8 *)p + 0x54);
                *(u16 *)((u8 *)base + 0x8A6) = *(u16 *)((u8 *)p + 0x56) + base->height;
                *(u16 *)((u8 *)base + 0x8A8) = *(u16 *)((u8 *)p + 0x58);
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

INCLUDE_ASM("asm/nonmatching", GridAttack_UpdateAnimationAndTargets);

INCLUDE_ASM("asm/nonmatching", GridAttack_UpdateCollidersAndPalette);

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
