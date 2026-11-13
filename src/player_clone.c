#include "engine.h"
#include "player_clone.h"
#include "ram.h"

u32 PlayerClone_GetActiveCount(void)
{
    struct PlayerCloneManager *pool = (void *)gDirectionalSpritePool;
    return pool->count;
}

/* PlayerClone_IsSlotActive: Tests the requested slot bit in the pool occupancy mask */

u32 PlayerClone_IsSlotActive(struct PlayerCloneManager *pool, s32 bit)
{
    return pool->activeSlots & (1 << bit);
}

void PlayerClone_FoldFacing(struct PlayerCloneSlot *sprite)
{
    u8 facing = sprite->facing;
    if (facing > 4) {
        sprite->foldedFacing = 8 - facing;
        sprite->mirrored = 1;
    } else {
        sprite->foldedFacing = facing;
        sprite->mirrored = 0;
    }
}

#define Anim_SetFrame Anim_SetFrameDurationScale

void Anim_SetFrameDurationScale(void *, u32);

s32 PlayerClone_SetAnimation(struct PlayerCloneSlot *s, s32 index, u32 frame, u32 mode)
{
    u32 same = 0;
    if (s->animationIndex == index)
        same = 1;
    s->animationIndex = index;
    Anim_SetKeyframes(&s->animation, &s->resource, (u16)((u16)(*(&index)) + s->foldedFacing),
                      (u16)mode, same);
    if (s->mirrored)
        s->animation.flags |= 4;
    else
        s->animation.flags &= ~4;
    Anim_SetFrameDurationScale(&s->animation, (u16)frame);
    return Anim_Step(&s->animation, &s->resource);
}

void PlayerClone_OnBodyColliderContact(struct ContactCollider *contact,
                                       struct ContactCollider *other, struct PlayerCloneSlot *s)
{
    u16 zero = other->responseValue;
    if (zero == 0) {
        if (contact->reactionFlags & 0x40) {
            Sound_PlaySong(0x211);
            s->hitPending = 1;
            s->hitStrength = zero;
            s->hitEffect = zero;
            s->hitDuration = zero;
        } else {
            s->hitPending = 1;
            s->hitStrength = contact->unknown3C >> 2;
            s->hitEffect = contact->responseParameter;
            s->hitDuration = contact->responseValue;
            Sound_PlaySong(0x4f4);
        }
        s->paletteTimer = 16;
        s->collider.responseValue = 16;
    }
}

void PlayerClone_OnPassiveColliderContact(void)
{
}

extern u16 gUnk_086141BC[];

extern const s16 gUnk_086149C4[];

s32 Div(s32, s32);

s32 Mod(s32, s32);

void ActorDebris_Show(u32);

void PlayerEffect_SetPaletteSource(u32, u32, u32);

void SlotMotion_Initialize(u8 *);

void Ratatosk_ConfigureParticle(u8 *, u32, u32, u32, u32, u32);

int SceneParticles_SpawnAtPosition(u8 *, void *);

u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);

void SpriteParticle_CreateDefaultVariant4384(void *, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                             u32, u32);

static inline s32 Clone_ScaleCoordinate(s32 x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}

static inline void Clone_CreateAngleVector(struct ContactPosition *v, s32 angle, s32 speed)
{
    v->x = Clone_ScaleCoordinate(gUnk_086149C4[(angle + 0x40) & 0xff] * speed);
    v->y = 0;
    v->z = Clone_ScaleCoordinate(gUnk_086149C4[angle & 0xff] * speed);
}

static inline void Clone_AddVectors(struct ContactPosition *d, struct ContactPosition *a,
                                    struct ContactPosition *b)
{
    d->x = b->x + a->x;
    d->y = b->y + a->y;
    d->z = b->z + a->z;
}

static inline void Clone_SubtractVectors(struct ContactPosition *d, struct ContactPosition *a,
                                         struct ContactPosition *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}

static inline s32 Clone_NextRandomByte(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return *(u8 *)(gRandomIndex * 2 + RANDOM_TABLE_ADDR);
}

static inline u16 Clone_NextRandomAngle(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3ff;
    return gUnk_086141BC[gEffectRandomTableIndex];
}

static inline void Clone_EnableHiddenFlag(struct SceneInputWidget *animation)
{
    animation->flags |= 1;
}

static inline void Clone_DisableHiddenFlag(struct SceneInputWidget *animation)
{
    animation->flags &= ~1;
}

static inline void Clone_EnableRiseFlag(struct SceneInputWidget *animation)
{
    animation->flags |= 0x400;
}

static inline void Clone_DisableRiseFlag(struct SceneInputWidget *animation)
{
    animation->flags &= ~0x400;
}

static inline void Clone_SetFlags(u32 mask, u32 *flags)
{
    *flags |= mask;
}

static inline struct SceneInputWidget *Clone_GetAnimation(struct PlayerCloneSlot *clone)
{
    return &clone->animation;
}

static inline void Clone_SetRiseHeight(struct PlayerCloneSlot *clone, s16 height)
{
    clone->animation.position.y = height;
}

static inline void Clone_ClearVector(const void *value, void *destination, u32 control)
{
    CpuSet(value, destination, control);
}

static inline u32 Clone_UpdateBlink(void *animation, struct PlayerCloneSlot *s)
{
    u16 *timer = &s->blinkTimer;
    if (Sprite_BlinkUntilExpired(animation, (s16)*timer, 0x2d, 0x5a))
        return 1;
    (*timer)++;
    return 0;
}

#define CLONE_DISABLE_COLLIDER(collider)                                                           \
    do {                                                                                           \
        (collider)->flags |= 4;                                                                    \
    } while (0)

static inline void Clone_MaskColliderFlags(u32 mask, struct ContactCollider *collider)
{
    collider->flags &= mask;
}

s32 PlayerClone_UpdateBehavior(struct PlayerCloneSlot *clone, s32 index)
{
    struct PlayerCloneConfiguration *params = &clone->configuration;
    struct {
        u32 fillWord;
        u8 particleMotion[0x18];
    } work;
    struct ContactPosition offset;
    struct ContactPosition delta;
    struct ContactPosition particlePosition;
    switch (clone->behaviorPhase) {
        case 0: {
            struct ContactCollider *body = &clone->collider;
            s32 direction;
            struct ContactPosition *initialPosition;
            CLONE_DISABLE_COLLIDER(body);
            ScaledShadow_Enable(clone->shadow);
            Clone_EnableHiddenFlag(&clone->animation);
            direction = params->angle;
            Clone_CreateAngleVector(&offset, direction + 0x80, Div(clone->timer << 9, 30));
            {
                struct ContactPosition *destination = &clone->animation.position;
                initialPosition = &clone->targetPosition;
                Clone_AddVectors(destination, &offset, initialPosition);
            }
            if (clone->timer > 30) {
                Clone_DisableHiddenFlag(&clone->animation);
                Clone_MaskColliderFlags(~4, body);
                clone->behaviorPhase = 1;
                clone->flags &= ~1;
                {
                    void *destination = &clone->velocity;
                    work.fillWord = 0;
                    Clone_ClearVector((void *)(&work.fillWord), destination, 0x05000002);
                }
            }
            break;
        }

        case 1: {
            struct SceneInputWidget *display;
            if (clone->timer & 1)
                SpriteParticle_CreateDefaultVariant4384(&clone->position, 0x28, 0x20, 0xc, 4, 0x18,
                                                        Clone_NextRandomByte(), 0x20, 0x20, 3, 2,
                                                        0x200);
            display = Clone_GetAnimation(clone);
            Clone_EnableRiseFlag(display);
            {
                s16 height = clone->previousPosition.y - ((60 - clone->timer) * 2);
                Clone_SetRiseHeight(clone, height);
            }
            if (clone->timer > 60) {
                clone->behaviorPhase = 2;
                Clone_DisableRiseFlag(display);
                ScaledShadow_Disable(clone->shadow);
            }
            break;
        }
        case 2:
            if (clone->timer > 0x120)
                clone->behaviorPhase = 3;
            break;

        case 3:
            if (Mod(clone->timer, 120) == 0) {
                params->angle = Clone_NextRandomAngle();
                Clone_SetFlags(1, &clone->flags);
                clone->animationIndex = 5;
                Anim_LoadKeyframe(&clone->animation, &clone->resource, 5, 1);
            }
            if (clone->timer > ((index * 30) + 0x294)) {
                u32 z;
                clone->previousPosition = clone->animation.position;
                {
                    u16 x = (u16)clone->animation.position.x;
                    z = 0;
                    clone->position.x = x;
                }
                clone->position.y = ((u16)clone->animation.position.y) + 0x80;
                clone->position.z = (u16)clone->animation.position.z;
                clone->behaviorPhase = 5;
                params->speed = z;
                Clone_SetFlags(1, &clone->flags);
                {
                    struct ContactCollider *body = &clone->collider;
                    CLONE_DISABLE_COLLIDER(body);
                }
                clone->blinkTimer = z;
                PLAYER_CLONE_START_IDLE(clone->animationIndex, z, &clone->animation,
                                        &clone->resource);
            } else {
                struct ContactPosition *playerPosition = Player_GetEffectivePosition(0);
                struct ContactPosition *pos = &clone->animation.position;
                s32 dx = playerPosition->x - pos->x;
                s32 dz = playerPosition->z - pos->z;
                if (((dx * dx) + (dz * dz)) <= 0xffff) {
                    struct SceneInputWidget *animation;
                    struct SceneInputResource *resource;
                    struct PlayerCloneManager *pool;
                    s16 *timer = &clone->attackTimer;
                    s32 z = 0;
                    *timer = z;
                    params->speed = z;
                    Clone_SetFlags(1, &clone->flags);
                    {
                        struct ContactPosition *target = Player_GetEffectivePosition(0);
                        params->angle = Math_Atan2(target->x - pos->x, target->z - pos->z);
                    }
                    clone->animationIndex = 0x54;
                    animation = &clone->animation;
                    resource = &clone->resource;
                    Anim_LoadKeyframe(animation, resource, 0x54, 2);
                    Anim_SetKeyframes(animation, resource,
                                      (u16)(clone->animationIndex + clone->foldedFacing), 2, 4);
                    clone->animation.attributeE = z;
                    pool = (void *)gDirectionalSpritePool;
                    if (!pool) {
                        /* Animation setup cannot finish without the drawable pool. */
                        return;
                    }
                    Anim_SelectPaletteBank(animation, 0x1f4);
                    clone->animation.paletteBank = 0xe000;
                    clone->animation.palette = pool->palette2;
                    ActorDebris_Show(clone->debris);
                    PlayerEffect_SetPaletteSource(clone->debris, 0xe001, 0);
                    clone->behaviorPhase = 4;
                } else {
                    s32 r;
                    if (Clone_NextRandomByte() < 4) {
                        if (clone->particleCount == 0)
                            clone->particleCount = 10;
                    }
                    if (clone->particleCount == 0)
                        break;
                    r = Mod(clone->timer, 0x20);
                    Clone_SubtractVectors(&delta, Ratatosk_GetPosition(),
                                          &clone->animation.position);
                    if (r == 0) {
                        work.fillWord = r;
                        Clone_ClearVector((void *)(&work.fillWord), &delta, 0x05000002);
                    } else {
                        delta.x = Div(delta.x * r, 0x20);
                        delta.y = 0;
                        delta.z = Div(delta.z * r, 0x20);
                    }
                    Clone_AddVectors(&particlePosition, &delta, &clone->animation.position);
                    particlePosition.y += 0x60;
                    SlotMotion_Initialize(work.particleMotion);
                    Ratatosk_ConfigureParticle(work.particleMotion, 10, 0, 0, 0, 0);
                    SceneParticles_SpawnAtPosition(work.particleMotion, &particlePosition);
                    clone->particleCount--;
                }
            }
            break;

        case 4: {
            s16 *attackTimer = &clone->attackTimer;
            if ((*attackTimer) > 5) {
                struct ContactCollider *area = &clone->area;
                Clone_MaskColliderFlags(~4, area);
            }
            if ((*attackTimer) > 30) {
                clone->behaviorPhase = 3;
                params->angle = Clone_NextRandomAngle();
                params->speed = clone->speed;
                Clone_SetFlags(1, &clone->flags);
                clone->animationIndex = 5;
                Anim_LoadKeyframe(&clone->animation, &clone->resource, 5, 1);
            }
            (*attackTimer)++;
            break;
        }

        case 5: {
            s16 *height;
            if (clone->timer & 1)
                SpriteParticle_CreateDefaultVariant4384(&clone->position, 0x28, 0x20, 0xc, 4, 0x18,
                                                        Clone_NextRandomByte(), 0x20, 0x20, 3, 2,
                                                        0x200);
            {
                s32 delay = index * 30;
                s32 elapsed = clone->timer - 0x2b2;
                s32 remaining = delay - elapsed;
                s16 y = clone->previousPosition.y - ((30 - remaining) * 2);
                height = &clone->animation.position.y;
                *height = y;
            }
            return Clone_UpdateBlink(&clone->animation.flags, clone);
        }
    }

    return 0;
}

static inline u32 Clone_DisableColliderFlags(u32 value)
{
    u32 mask = 4;
    mask |= value;
    return mask;
}

void PlayerClone_InitSlot(void *unused, struct PlayerCloneSlot *s, u32 arg)
{
    struct PlayerCloneConfiguration *context = &s->configuration;
    u32 zero, fill;
    void *res;
    struct ContactCollider *p;
    u32 value;
    struct PlayerCloneColliderVector v, w;

    PlayerClone_InitConfig(context);
    zero = 0;
    s->timer = zero;
    {
        u8 *dest = &s->velocity;
        u32 *source;
        fill = zero;
        source = &fill;
        Clone_ClearVector(source, dest, 0x05000002);
    }
    res = ScriptProc_Start(0xcb05, 0xde23);
    {
        struct SceneInputResource *obj = &s->resource;
        ResourceHeader_Relocate(obj, res);
        {
            struct SceneInputWidget *p = &s->animation;
            Widget_InitLinked(p, obj, 0, 0, 2, 1, zero, zero);
            s->animation.flags |= 1;
            value = ActorDebris_Create(p, &s->foldedFacing, &s->mirrored);
            s->debris = value;
            PlayerEffect_SetPaletteSource(value, 0xe001, 0);
        }
    }
    {
        u8 *obj = s->shadow;
        ScaledShadow_InitializeDefault(obj, &s->animation.position);
        ScaledShadow_Disable(obj);
    }
    s->index = arg;
    p = &s->collider;
    v.x = 40;
    v.y = 128;
    v.z = 40;
    w.x = zero;
    w.y = 128;
    w.z = zero;
    Collider_Init(p, zero, 0x4001, zero, 16, &v, &w);
    Collider_SetCallback(p, PlayerClone_OnBodyColliderContact, s);
    Collider_SetBodyProperties(p, (s16)context->unknown16, 0x402, 1);
    Collider_Refresh(p);
    p->flags = Clone_DisableColliderFlags(p->flags);
    p = &s->contact;
    v.x = 64;
    v.y = 128;
    v.z = 64;
    w.x = zero;
    w.y = 128;
    w.z = zero;
    Collider_Init(p, zero, 0x2001, zero, 16, &v, &w);
    Collider_SetCallback(p, PlayerClone_OnPassiveColliderContact, s);
    Collider_SetAttackProperties(p, (s16)context->operation.attack, (s16)context->operation.element,
                                 0, zero, context->primaryPower);
    p = &s->area;
    v.x = 200;
    v.y = 128;
    v.z = 200;
    w.x = zero;
    w.y = 128;
    w.z = zero;
    Collider_Init(p, zero, 0x2001, zero, 16, &v, &w);
    Collider_SetCallback(p, PlayerClone_OnPassiveColliderContact, s);
    Collider_SetAttackProperties(p, context->secondaryCollider[0], context->secondaryCollider[1], 0,
                                 zero, context->secondaryCollider[2]);
    p = &s->playerAttack;
    v.x = 32;
    v.y = 32;
    v.z = 32;
    w.x = zero;
    w.y = 128;
    w.z = zero;
    Collider_Init(p, zero, 0x2001, zero, 16, &v, &w);
    Collider_SetCallback(p, PlayerClone_OnPassiveColliderContact, s);
}

void ActorDebris_Hide(u32);

void PlayerClone_ReleaseSlot(struct PlayerCloneManager *a, struct PlayerCloneSlot *b, u32 n)
{
    struct ContactCollider *s;
    struct SceneInputWidget *w;
    u32 m;
    a->count--;
    ScaledShadow_Disable(b->shadow);
    ActorDebris_Hide(b->debris);
    s = &b->collider;
    s->flags = 4 | s->flags;
    w = &b->animation;
    w->flags = w->flags | 1;
    a->activeSlots &= ~(1 << n);
}

struct PlayerCloneSlot;

extern u32 gUnk_02000560;

static inline CloneUpdateCallback CloneCallback_Get(struct CloneCallback operation)
{
    return operation.callback;
}

static inline u16 CloneMemberOffset_Value(u16 value)
{
    struct {
        u16 value : 16;
    } field;
    field.value = value;
    return field.value;
}

static inline void Vec_FromAngle(struct ContactPosition *v, s32 angle, s32 speed)
{
    v->x = Clone_ScaleCoordinate(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = Clone_ScaleCoordinate(gUnk_086149C4[angle & 255] * speed);
}

static inline void Vec_Sum(struct ContactPosition *d, struct ContactPosition *a,
                           struct ContactPosition *b)
{
    const s16 *x = &b->x;
    d->x = *x + a->x;
    d->y = b->y + a->y;
    d->z = b->z + a->z;
}

static inline u32 IsPaused(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}

static inline u32 TestGlobal(u32 mask)
{
    return gUnk_02000560 & mask;
}

static inline void ClearFlags(u32 *flags, u32 mask)
{
    *flags &= ~mask;
}

static inline s8 CloneParams_GetDirection(struct PlayerCloneConfiguration *params)
{
    return params->angle;
}

static inline void SetFacing(struct PlayerCloneSlot *slot, u16 angle)
{
    s32 facing = (angle + 0x70) >> 5;
    u32 mask = 7;
    mask &= facing;
    slot->facing = mask;
}

static inline void UpdateVelocity(u32 moving, struct PlayerCloneSlot *slot,
                                  struct PlayerCloneConfiguration *params)
{
    u32 *flags = &slot->flags;
    if (moving) {
        struct ContactPosition *velocity = &slot->velocity;
        Vec_FromAngle(velocity, params->angle, params->speed);
        ClearFlags(flags, 1);
    }
}

static inline u8 Clone_ColliderHasFlags(struct CloneColliderHeader header, s16 mask)
{
    mask &= header.flags;
    return mask != 0;
}

static inline void CloneAnim_SetPaletteFlags(struct SceneInputWidget *anim, u16 flags)
{
    anim->paletteBank = flags;
}

static inline void CloneAnim_SetPalette(struct SceneInputWidget *anim, void *palette)
{
    anim->palette = palette;
}

/* Advance each active clone, its animation and colliders, and its lifetime. */

s32 PlayerClone_UpdatePool(struct PlayerCloneManager *pool)
{
    struct PlayerCloneSlot *slot = pool->slots;
    s32 i = 0;
    struct CloneProjectionVectors projection;
    struct ContactPosition *positionRef = &projection.position;
    u32 stride = sizeof(*slot);
    u32 stateOffset = CloneMemberOffset_Value((u32) & ((struct PlayerCloneSlot *)0)->flags);
    do {
        struct SceneInputWidget *walkAnim = &slot->animation;
        struct ContactCollider *area;
        u8 *facing;
        struct CloneAreaWork areaWork;
        u32 active;
        active = pool->activeSlots & (1 << i);
        area = &slot->area;
        if (active) {
            struct SceneInputWidget *anim = walkAnim;
            struct PlayerCloneConfiguration *params = &slot->configuration;
            struct ContactCollider *collider;
            struct SceneInputWidget *metadata;
            u32 paused;
            s32 callbackResult;

            paused = IsPaused(0x400);
            metadata = walkAnim;
            if (paused)
                goto next;
            if (TestGlobal(4))
                goto next;
            slot->area.flags |= 4;
            callbackResult =
                ((s32(*)(struct PlayerCloneSlot *, s32))CloneCallback_Get(params->operation))(slot,
                                                                                              i);
            areaWork.collider = area;
            if (callbackResult == 1) {
                PlayerClone_ReleaseSlot(pool, slot, i);
                goto next;
            }
            facing = &slot->facing;
            SetFacing(slot, (u16)CloneParams_GetDirection(params));
            PlayerClone_FoldFacing(slot);
            {
                u32 moving = slot->flags & 1;
                areaWork.facing = facing;

                UpdateVelocity(moving, slot, params);
                Vec_Sum(&anim->position, &slot->velocity, &anim->position);
            }
            if (anim->position.x > 0xcda)
                anim->position.x = 0xcda;
            else if (anim->position.x < 0x424)
                anim->position.x = 0x424;
            if (anim->position.z > 0xbd6)
                anim->position.z = 0xbd6;
            else if (anim->position.z < 0x528)
                anim->position.z = 0x528;
            if (slot->behaviorPhase == 4)
                PlayerClone_SetAnimation(slot, slot->animationIndex, 0x40, 2);
            else
                PlayerClone_SetAnimation(slot, slot->animationIndex, 0x40, 1);
            {
                struct ContactPosition *pos;
                collider = &slot->contact;
                pos = &slot->animation.position;
                Collider_SetPosition(collider, pos, 0);
                if (!(slot->collider.flags & 4))
                    Collider_Refresh(collider);
                collider = &slot->collider;
                Collider_SetPosition(collider, pos, 0);
            }
            if (!Clone_ColliderHasFlags(*(struct CloneColliderHeader *)&slot->area, 4)) {
                struct ContactPosition *deltaRef;
                collider = areaWork.collider;
                projection.position = slot->animation.position;
                deltaRef = &projection.delta;
                Vec_FromAngle(deltaRef, *areaWork.facing, 0x80);
                Vec_Sum(positionRef, positionRef, &projection.delta);
                Collider_SetPosition(collider, positionRef, 0);
                Collider_Refresh(collider);
            }
            if (slot->hitPending) {
                slot->hitPending = 0;
                collider = &slot->playerAttack;
                Collider_SetAttackProperties(collider, slot->hitStrength, slot->hitEffect, 0, 2,
                                             slot->hitDuration);
                Collider_SetPosition(collider, Player_GetEffectivePosition(0), 0);
                Collider_Refresh(collider);
            }
            if (slot->paletteTimer > 0) {
                slot->paletteTimer--;
                Anim_SelectPaletteBank(metadata, 0x192);
                CloneAnim_SetPaletteFlags(walkAnim, 0xe013);
                CloneAnim_SetPalette(walkAnim, pool->palette1);
            } else {
                Anim_SelectPaletteBank(metadata, 0x1f4);
                CloneAnim_SetPaletteFlags(walkAnim, 0xe000);
                CloneAnim_SetPalette(walkAnim, pool->palette2);
            }
            if (slot->timer >= (u32)(i * 30 + 0x2b2))
                PlayerClone_ReleaseSlot(pool, slot, i);
            if (slot->collider.responseValue)
                slot->collider.responseValue--;
            slot->timer++;
        }
    next:
        i++;
        slot++;
    } while (i <= 2);
    return 0;
}

void ScaledShadow_Destroy(void *);

u32 PlayerClone_DestroyPool(struct PlayerCloneManager *a)
{
    struct PlayerCloneSlot *e = a->slots;
    s32 i;
    for (i = 2; i >= 0; i--, e++) {
        u32 *q;
        Collider_Remove(&e->collider);
        ScaledShadow_Destroy(e->shadow);
        q = &e->debris;
        if (*q != 0) {
            ActorDebris_Hide(*q);
            Task_Kill((void *)*q);
        }
        Anim_Remove(&e->animation);
    }
    return gDirectionalSpritePool = 0;
}

#define DirectionalSprite_InitEntry PlayerClone_InitSlot

s32 PlayerClone_InitializePool(struct PlayerCloneManager *pool)
{
    struct PlayerCloneSlot *entry;
    s32 i;
    s16 *value;
    gDirectionalSpritePool = pool;
    pool->activeSlots = 0;
    entry = pool->slots;
    i = 0;
    do {
        PlayerClone_InitSlot(pool, entry, i);
        i++;
        entry++;
    } while (i <= 2);
    {
        u8 **palettes = &gPaletteBankData;
        u16 blank = 0x7FFF;
        i = 15;
        value = &pool->palette1[15];
        do {
            *value = blank;
            value--;
            i--;
        } while (i >= 0);
        CpuSet(*palettes + 0x3E80, pool->palette2, 0x04000008);
    }
    return 0;
}

void *PlayerClone_CreatePool(void)
{
    void *p = Task_Create(8, 0xa2c);
    if (p) {
        Task_SetCallbacks(p, PlayerClone_UpdatePool, PlayerClone_DestroyPool);
        if (PlayerClone_InitializePool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
        return p;
    }
}

struct PlayerCloneSlot *PlayerClone_FindFreeSlot(struct PlayerCloneManager *a, s32 *out)
{
    struct PlayerCloneSlot *e = a->slots;
    s32 i = 0;
    u32 m = 1;
    u32 w = a->activeSlots;
    do {
        if ((m << i & w) != 0) {
            i++;
            e++;
            continue;
        }
        *out = i;
        return e;
    } while (i <= 2);
    return 0;
}

static inline void PlayerClone_ClearVelocity(const u32 *zero, struct ContactPosition *velocity)
{
    CpuSet(zero, velocity, 0x05000002);
}

#define PlayerClone_Spawn sub_08201160

s32 PlayerClone_Spawn(struct PlayerCloneConfiguration *resource, struct ContactPosition *pos)
{
    struct {
        u32 fill;
        s32 index;
    } scratch;
    struct PlayerCloneManager *pool;
    struct PlayerCloneSlot *slot;
    pool = gDirectionalSpritePool;
    if (!pool) {
        PlayerClone_CreatePool();
    } else {
        slot = PlayerClone_FindFreeSlot(pool, &scratch.index);
        if (slot) {
            struct PlayerCloneConfiguration *handler = &slot->configuration;
            struct SceneInputWidget *display;
            u32 zero;
            struct ContactPosition *tmp;
            Memory_Copy(handler, resource, sizeof(*resource));
            zero = 0;
            slot->timer = zero;
            slot->behaviorPhase = zero;
            slot->paletteTimer = zero;
            slot->animationIndex = zero;
            display = &slot->animation;
            Anim_LoadKeyframe(display, &slot->resource, 0, 1);
            slot->speed = resource->speed;
            slot->animation.attribute1A = 2;
            slot->animation.position = *pos;
            slot->previousPosition = *pos;
            slot->position.x = pos->x;
            slot->position.y = pos->y + 0x80;
            slot->position.z = pos->z;
            tmp = Player_GetEffectivePosition(0);
            slot->targetPosition = *tmp;
            {
                struct ContactPosition *target = &slot->velocity;
                scratch.fill = zero;
                PlayerClone_ClearVelocity(&scratch.fill, target);
            }
            display->flags |= 1;
            PlayerClone_SetBehaviorCallback(handler, PlayerClone_UpdateBehavior);
            pool->activeSlots |= 1 << scratch.index;
            pool->count++;
            return scratch.index;
        }
        return -1;
    }
    return -1;
}

void PlayerClone_SetBehaviorCallback(struct PlayerCloneConfiguration *configuration,
                                     CloneUpdateCallback callback)
{
    configuration->operation.callback = callback;
}

void PlayerClone_InitConfig(struct PlayerCloneConfiguration *p)
{
    Memory_Clear(p, 0x20);
    PlayerClone_SetBehaviorCallback(p, PlayerClone_UpdateBehavior);
    p->flags = 0;
    p->lifetime = 0x3c;
    p->speed = 0x10;
    p->angle = 0;
    {
        u32 c = 0x1e;
        p->operation.attack = c;
        p->operation.element = c;
        p->primaryPower = c;
        p->secondaryCollider[0] = c;
        p->secondaryCollider[1] = 0xf;
        p->secondaryCollider[2] = c;
    }
}

void PlayerClone_SetLifetimeAndBehavior(struct PlayerCloneConfiguration *p, u16 a, u32 b, u8 c)
{
    p->lifetime = a;
    p->speed = b;
    p->angle = c;
}

void PlayerClone_SetColliderExtents(struct PlayerCloneConfiguration *p, u32 a, u32 b, u32 c, u32 d,
                                    u32 e, u32 f)
{
    p->operation.attack = a;
    p->operation.element = b;
    p->primaryPower = c;
    p->secondaryCollider[0] = d;
    p->secondaryCollider[1] = e;
    p->secondaryCollider[2] = f;
}

void PlayerClone_BeginActiveReactions(void)
{
    struct PlayerCloneManager *manager = gDirectionalSpritePool;
    u32 slotStride = sizeof(struct PlayerCloneSlot);
    if (manager) {
        struct PlayerCloneSlot *s = manager->slots;
        s32 i = 0;
        u32 resetValue = 0;
        u32 *flags = &s->flags;
        s32 *speed = &s->configuration.speed;
        u32 timer = 660;
        do {
            if ((manager->activeSlots & (1 << i)) != 0) {
                s->previousPosition =
                    ((struct PlayerCloneSlot *)((u8 *)speed - (u32) &
                                                ((struct PlayerCloneSlot *)0)->configuration.speed))
                        ->animation.position;
                s->position.x =
                    ((struct PlayerCloneSlot *)((u8 *)speed - (u32) &
                                                ((struct PlayerCloneSlot *)0)->configuration.speed))
                        ->animation.position.x;
                s->position.y =
                    ((struct PlayerCloneSlot *)((u8 *)speed - (u32) &
                                                ((struct PlayerCloneSlot *)0)->configuration.speed))
                        ->animation.position.y +
                    128;
                s->position.z =
                    ((struct PlayerCloneSlot *)((u8 *)speed - (u32) &
                                                ((struct PlayerCloneSlot *)0)->configuration.speed))
                        ->animation.position.z;
                ((struct PlayerCloneSlot *)((u8 *)flags - (u32) &
                                            ((struct PlayerCloneSlot *)0)->flags))
                    ->behaviorPhase = 5;
                *speed = resetValue;
                {
                    u16 one = 1;
                    *flags |= one;
                }
                {
                    struct ContactCollider *collider = &s->collider;
                    collider->flags |= 4;
                }
                PLAYER_CLONE_START_IDLE(
                    ((struct PlayerCloneSlot *)((u8 *)flags - (u32) &
                                                ((struct PlayerCloneSlot *)0)->flags))
                        ->animationIndex,
                    resetValue, &s->animation, &s->resource);
                s->timer = timer;
            }
            timer += 30;
            i++;
            flags = (void *)(((u8 *)flags) + slotStride);
            speed = (void *)(((u8 *)speed) + slotStride);
            s++;
        } while (i <= 2);
    }
}
