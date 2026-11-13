#include "scene_effect.h"

/* Tentative family: an animated actor with an embedded scaled shadow. */

void ScaledShadow_Enable(void *);

void ShadowActor_UpdateTransition(struct SceneEffectActor *actor)
{
    if (!actor->entered) {
        Anim_SetKeyframes(&actor->animation, actor->animationData, 5, 2, 0);
        actor->entered++;
    } else if (actor->animationEnded) {
        u32 zero;
        Anim_SetKeyframes(&actor->animation, actor->animationData, 0, 2, zero = 0);
        ScaledShadow_Enable(actor->shadow);
        {
            s32 next = actor->state;
            switch (next) {
                case 3:
                    next = 4;
                    break;
                case 6:
                    next = 7;
                    break;
                case 0:
                default:
                    goto idle;
            }
            actor->state = next;
            actor->entered = zero;
            actor->phase = zero;
            return;
        idle: {
            u8 one = 1;
            actor->state = one;
            actor->entered = 0;
            actor->phase = 0;
        }
        }
    }
}

void ScaledShadow_Disable(void *);

void SceneEffect_UpdateFinishAnimation(struct SceneEffectActor *actor)
{
    u8 *entered = &actor->entered;
    u32 phase = *entered;
    if (phase == 0) {
        Anim_SetKeyframes(&actor->animation, actor->animationData, 6, 2, phase);
        ScaledShadow_Disable(actor->shadow);
        (*entered)++;
    } else if (actor->animationEnded != 0) {
        u32 zero = 0;
        actor->update = zero;
        if (actor->destroy != 0) {
            Callback_InvokeActor(actor, actor->destroy);
            actor->destroy = zero;
        }
    }
}

void SceneActor_SetSavedPositionOffset(struct SceneEffectActor *actor)
{
    ShadowActor_UpdateTransition(actor);
    if (actor->animation.flags & 4) {
        actor->animation.position.x = gGameState->positionX + 0x120;
        actor->animation.position.y = gGameState->positionY;
        actor->animation.position.z = gGameState->positionZ - 0x120;
    } else {
        actor->animation.position.x = gGameState->positionX - 0x120;
        actor->animation.position.y = gGameState->positionY;
        actor->animation.position.z = gGameState->positionZ + 0x120;
    }
}

#define gShadowActorActionVectors gUnk_0824F60C

#define gShadowActorMirroredActionVectors gUnk_0824F624

/* Same tentative actor layout as ShadowActor_UpdateTransition. */

extern const struct ContactPosition gShadowActorActionVectors[];

extern const struct ContactPosition gShadowActorMirroredActionVectors[];

static inline void SetRegionFlag(struct ContactCollider *region, u32 bit)
{
    region->flags = bit | (u16)region->flags;
}

static inline void ChangeState(struct SceneEffectActor *actor, u32 state)
{
    actor->state = state;
    actor->entered = 0;
    actor->phase = 0;
}

void ShadowActor_UpdateThreeStageAction(struct SceneEffectActor *actor)
{
    struct ContactCollider *region;
    const struct ContactPosition *vector;
    u8 *counter;
    switch ((s32)actor->entered) {
        case 1:
        case 2:
            region = &actor->collider;
            goto configure;
        case 0:
            Anim_SetKeyframes(&actor->animation, actor->animationData, 1, 2, 0);
            region = &actor->collider;
            {
                s32 mask = -5;
                region->flags &= mask;
            }
        configure:
            if (actor->animation.flags & 4)
                vector = &gShadowActorMirroredActionVectors[actor->entered];
            else
                vector = &gShadowActorActionVectors[actor->entered];
            region->offset = *vector;
            counter = &actor->entered;
            break;
        default: {
            struct ContactCollider *region2 = &actor->collider;
            SetRegionFlag(region2, 4);
            if (!actor->animationEnded)
                return;
            {
                u32 next = actor->phase;
                if (next > 4) {
                    next = 2;
                    ChangeState(actor, next);
                    return;
                }
                actor->entered = 0;
                counter = &actor->phase;
            }
        } break;
    }
    (*counter)++;
}

s32 __divsi3(s32, s32);

static inline s32 ShiftTowardZero(s32 value, s32 shift)
{
    s32 result;
    if (value >= 0)
        result = value >> shift;
    else
        result = -((-value) >> shift);
    return result;
}

static inline void ProjectPosition(struct ContactPosition *out, struct ContactPosition *in)
{
    s32 a, b, c, d, x = in->x, z = in->z;
    out->x = ShiftTowardZero(3 * (x - z), 5);
    a = ShiftTowardZero(3 * (x + z), 5);
    b = ShiftTowardZero(3 * in->y, 5);
    c = a - b;
    d = a + b;
    out->x = out->x - (*(struct ContactPosition *)gCameraOrigin).x + 120;
    out->y = c - (*(struct ContactPosition *)gCameraOrigin).y + 80;
    out->z = d - (*(struct ContactPosition *)gCameraOrigin).z;
}

static inline s32 ProjectedX(struct ContactPosition *position)
{
    return position->x;
}

void SceneEffect_SetScreenEdgePosition(struct SceneEffectActor *actor)
{
    struct SceneEffectPackedPosition position;
    struct ContactPosition screen;
    s32 height;
    struct GameState *savedState, *currentState;
    s32 y, screenX, screenY, coordinate;
    struct ContactPosition *world;
    ShadowActor_UpdateTransition(actor);
    savedState = gGameState;
    position.x = (u16)savedState->positionX;
    position.y = (u16)savedState->positionY;
    position.z = (u16)savedState->positionZ;
    ProjectPosition(&screen, (struct ContactPosition *)&position);
    {
        if (actor->animation.flags & 4)
            screen.x = 0xe0;
        else
            screen.x = 0x10;
    }
    currentState = gGameState;
    world = (struct ContactPosition *)&position;
    y = currentState->positionY;
    screenX = ProjectedX(&screen) + (*(struct ContactPosition *)gCameraOrigin).x - 0x78;
    screenY = screen.y + (*(struct ContactPosition *)gCameraOrigin).y - 0x50;
    coordinate = __divsi3((screenX + screenY) << 5, 3) + y;
    world->x = ShiftTowardZero(coordinate, 1);
    height = y;
    world->y = height;
    coordinate = __divsi3((screenY - screenX) << 5, 3) + height;
    world->z = ShiftTowardZero(coordinate, 1);
    actor->animation.position.x = position.x;
    actor->animation.position.y = position.y;
    actor->animation.position.z = position.z;
}

static inline void UnprojectPosition(struct ContactPosition *out, struct ContactPosition *in, s32 y)
{
    s32 a, b, c;
    a = in->x + (*(struct ContactPosition *)gCameraOrigin).x - 120;
    b = in->y + (*(struct ContactPosition *)gCameraOrigin).y - 80;
    c = __divsi3((a + b) << 5, 3) + y;
    out->x = ShiftTowardZero(c, 1);
    out->y = y;
    c = __divsi3((b - a) << 5, 3) + y;
    out->z = ShiftTowardZero(c, 1);
}

void SceneEffect_UpdateRingSpawn(struct SceneEffectActor *actor)
{
    if (!actor->entered) {
        Anim_SetKeyframes(&actor->animation, actor->animationData, 3, 2, 0);
        if (actor->animation.flags & 4) {
            struct ContactPosition screen, world;
            s32 y;
            ProjectPosition(&screen, &actor->animation.position);
            y = actor->animation.position.y + 213;
            screen.x -= 39;
            screen.y -= 20;
            UnprojectPosition(&world, &screen, y);
            SceneEntryPool_CreateEntry(actor->owner, &world, 1);
        } else {
            struct ContactPosition screen, world;
            s32 y;
            ProjectPosition(&screen, &actor->animation.position);
            y = actor->animation.position.y + 213;
            screen.x += 39;
            screen.y -= 20;
            UnprojectPosition(&world, &screen, y);
            SceneEntryPool_CreateEntry(actor->owner, &world, 0);
        }
        actor->entered++;
    } else if (actor->animationEnded) {
        ChangeState(actor, 5);
    }
}

// Tentative: enters from the screen edge before resuming its world-space animation.

void SceneEffect_UpdateArrival(struct SceneEffectActor *actor)
{
    struct SceneEffectPackedPosition input;
    struct ContactPosition screen;
    struct ContactPosition world;
    struct GameState *player;
    s32 nextState;
    if (actor->entered == 0) {
        Anim_SetKeyframes(&actor->animation, actor->animationData, 5, 1, 0);
        actor->animation.flags |= 0x10;
        player = gGameState;
        input.x = player->positionX;
        input.y = player->positionY;
        input.z = player->positionZ;
        ProjectPosition(&screen, (struct ContactPosition *)&input);
        if (actor->animation.flags & 4)
            screen.x = 264;
        else
            screen.x = -24;
        actor->animation.position.x = screen.x;
        actor->animation.position.y = screen.y;
        actor->entered++;
    } else {
        if (actor->animation.flags & 4) {
            actor->animation.position.x -= 7;
            if (actor->animation.position.x > 184)
                return;
            actor->animation.position.x = 184;
        } else {
            actor->animation.position.x += 7;
            if (actor->animation.position.x < 56)
                return;
            actor->animation.position.x = 56;
        }
        actor->animation.flags &= ~0x10;
        UnprojectPosition(&world, &actor->animation.position, gGameState->positionY);
        actor->animation.position.x = world.x;
        actor->animation.position.y = world.y;
        actor->animation.position.z = world.z;
        Anim_SetKeyframes(&actor->animation, actor->animationData, 0, 2, 0);
        ScaledShadow_Enable(actor->shadow);
        nextState = 7;
        actor->state = nextState;
        actor->entered = 0;
        actor->phase = 0;
    }
}

extern const struct ContactPosition gUnk_0824F65C, gUnk_0824F63C, gUnk_0824F664, gUnk_0824F644,
    gUnk_0824F66C, gUnk_0824F64C, gUnk_0824F674, gUnk_0824F654;

static inline u8 *SceneEffect_GetRecoilTimer(struct SceneEffectActor *actor)
{
    return &actor->phase;
}

void SceneEffect_UpdateRecoil(struct SceneEffectActor *actor)
{
    s32 state = actor->entered;
    u16 nextState;
    switch (state) {
        case 0:
            Anim_SetKeyframes(&actor->animation, actor->animationData, 4, 2, 0);
            actor->entered++;
            break;

        case 1:
            if (actor->phase <= 7)
                actor->phase++;
            else {
                {
                    u8 nextPhase = 2;
                    actor->entered = nextPhase;
                }
                actor->phase = 0;
                if (actor->animation.flags & 4) {
                    actor->velocity.x = -48;
                    actor->velocity.y = 0;
                    actor->velocity.z = 48;
                } else {
                    actor->velocity.x = 48;
                    actor->velocity.y = 0;
                    actor->velocity.z = -48;
                }
            }
            break;

        case 2:
            actor->animation.position.x += actor->velocity.x;
            actor->animation.position.z += actor->velocity.z;
            if (actor->animation.flags & 4) {
                actor->velocity.x += 2;
                actor->velocity.z -= 2;
                if (actor->velocity.x > 0) {
                    actor->velocity.x = 0;
                    actor->velocity.z = 0;
                }
            } else {
                actor->velocity.x -= 2;
                actor->velocity.z += 2;
                if (actor->velocity.x < 0) {
                    actor->velocity.x = 0;
                    actor->velocity.z = 0;
                }
            }
            if (actor->animationEnded) {
                actor->entered++;
                actor->phase = 0;
            }
            break;

        default: {
            u8 *timer = SceneEffect_GetRecoilTimer(actor);
            if (*timer <= 3) {
                (*timer)++;
            } else {
                nextState = 8;
                actor->state = nextState;
                actor->entered = 0;
                actor->phase = 0;
            }
            break;
        }
    }

    {
        switch (actor->animation.frame) {
            case 5:
            case 6: {
                struct ContactCollider *collider = &actor->collider;
                u32 mask = 4;
                collider->flags = ~mask & collider->flags;
                if (actor->animation.flags & mask)
                    collider->offset = gUnk_0824F65C;
                else
                    collider->offset = gUnk_0824F63C;
                break;
            }
            case 7: {
                if (actor->animation.flags & 4) {
                    struct ContactCollider *collider = &actor->collider;
                    collider->offset = gUnk_0824F664;
                } else {
                    struct ContactCollider *collider = &actor->collider;
                    collider->offset = gUnk_0824F644;
                }
                break;
            }
            case 15: {
                if (actor->animation.flags & 4) {
                    struct ContactCollider *collider = &actor->collider;
                    collider->offset = gUnk_0824F66C;
                } else {
                    struct ContactCollider *collider = &actor->collider;
                    collider->offset = gUnk_0824F64C;
                }
                break;
            }
            case 16: {
                if (actor->animation.flags & 4) {
                    struct ContactCollider *collider = &actor->collider;
                    collider->offset = gUnk_0824F674;
                } else {
                    struct ContactCollider *collider = &actor->collider;
                    collider->offset = gUnk_0824F654;
                }
                break;
            }
            default: {
                struct ContactCollider *collider = &actor->collider;
                collider->flags |= 4;
                break;
            }
        }
    }
}

extern const u8 gUnk_0824F67C[];

void SceneEffect_UpdateActor(void *arg)
{
    struct SceneEffectActor *actor = arg;
    const u32 *callbacks = (const u32 *)gUnk_0824F67C;
    Callback_InvokeActor(actor, (void *)callbacks[actor->state]);
    Collider_Refresh(&actor->collider);
    Collider_SetPosition(&actor->collider, &actor->animation.position, 0);
}

void ScaledShadow_Destroy(void *);

void SceneEffect_DestroyActor(void *arg)
{
    struct SceneEffectActor *actor = arg;
    ScaledShadow_Destroy(actor->shadow);
    Collider_Remove(&actor->collider);
    if (actor->animation.registered) {
        Anim_Remove(&actor->animation);
    }
}

extern const struct ContactPosition gUnk_0824F6A0, gUnk_0824F6B0, gUnk_0824F6A8;

s32 ResourceHeader_Relocate(void *, void *);

void Widget_InitLinked(void *, void *, int, int, int, int, int, const void *);

void ScaledShadow_InitializeDefault(void *, void *);

void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

void SceneEffect_InitActor(struct SceneEffectActor *actor, s32 mode, s32 flip)
{
    void *resource = ScriptProc_Start(0xCB05, 0x658);
    u8 *animationData = actor->animationData;
    u8 *animation;
    struct ContactCollider *specialCollider;
    struct ContactCollider *normalCollider;
    u16 stateValue;
    s16 state;
    void (*update)(void *);
    void (*destroy)(void *);
    ResourceHeader_Relocate(animationData, resource);
    animation = &actor->animation;
    Widget_InitLinked(animation, animationData, 0, 0, 2, 0, 0x3c, &gUnk_0824F6A0);
    Anim_SetKeyframes(animation, animationData, 5, 1, 0);
    if (flip)
        actor->animation.flags |= 4;
    else
        actor->animation.flags &= ~4;
    ScaledShadow_InitializeDefault(actor->shadow, &actor->animation.position);
    ScaledShadow_Disable(actor->shadow);
    switch (mode) {
        case 0:
            actor->state = mode;
            actor->entered = mode;
            actor->phase = mode;
            normalCollider = &actor->collider;
            Collider_Init(normalCollider, 0, 0x2105, 0, 0x10, &gUnk_0824F6A0, &gUnk_0824F6A0);
            Collider_SetAttackProperties(normalCollider, actor->owner->actorPower,
                                         actor->owner->actorElement, 0x10, mode, mode);
            {
                const struct ContactPosition *src = &gUnk_0824F6A8;
                u32 hi = ((const u32 *)src)[1];
                ((u32 *)&normalCollider->bounds)[0] = ((const u32 *)src)[0];
                ((u32 *)&normalCollider->bounds)[1] = hi;
            }
            break;
        case 1:
            state = 3;
            stateValue = state;
            actor->state = stateValue;
            actor->entered = 0;
            actor->phase = 0;
            Collider_Init(&actor->collider, 0, 0x2105, 0, 0x10, &gUnk_0824F6A0, &gUnk_0824F6A0);
            break;
        case 2:
        case 3:
            stateValue = 6;
            actor->state = stateValue;
            actor->entered = 0;
            actor->phase = 0;
            specialCollider = &actor->collider;
            Collider_Init(specialCollider, 0, 0x2105, 0, 0x10, &gUnk_0824F6A0, &gUnk_0824F6A0);
            Collider_SetAttackProperties(specialCollider, actor->owner->specialPower,
                                         actor->owner->specialElement, 1, 0x40000, 0);
            specialCollider->bounds = gUnk_0824F6B0;
            break;
    }
    if (flip) {
        u8 facing = 0x60;
        actor->collider.direction = facing;
    } else {
        u8 facing = 0xe0;
        actor->collider.direction = facing;
    }
    update = SceneEffect_UpdateActor;
    destroy = SceneEffect_DestroyActor;
    actor->update = update;
    actor->destroy = destroy;
}

struct SceneEffectQueue;

/* Tentative: claims the pool's single actor slot and initializes its mode. */

struct SceneEffectActor *SceneEffect_AllocateActor(struct SceneEffectQueue *pool, s32 mode,
                                                   s32 flag)
{
    struct SceneEffectActor *actor = NULL;
    struct SceneEffectActor *result;
    s32 i;

    for (i = 0; i < 1; i++) {
        if (!((pool->actorsUsed >> i) & 1)) {
            actor = &pool->actors[i];
            pool->actorsUsed |= 1 << i;
            break;
        }
    }
    if (actor) {
        actor->owner = pool;
        SceneEffect_InitActor(actor, mode, flag);
        result = actor;
    } else {
        result = NULL;
    }
    return result;
}

u32 SceneEffect_LoadRingAnimation(void)
{
    return ScriptProc_Start(0x922E, 0x6946);
}

void Anim_RestartSequence(void *, u32, u32, u32, u32);

static inline s32 RingProjectionScale(s32 v)
{
    s32 r;
    if (v >= 0)
        r = v >> 5;
    else
        r = -((-v) >> 5);
    return r;
}

void SceneEffectRing_Update(void *param)
{
    struct SceneEffectRing *ring = param;
    s16 screen[3];
    s16 *out = screen;
    s16 *position = &ring->sprite.position.x;
    s32 x = position[0], z = position[2], a, c, y;
    u8 *mode;
    void *destroy;
    struct ContactPosition *worldPosition;
    struct ContactCollider *collider;
    s16 *positionX, *positionZ;
    u32 zero;
    out[0] = RingProjectionScale(3 * (x - z));
    c = RingProjectionScale(3 * (x + z));
    y = RingProjectionScale(3 * position[1]);
    a = c - y;
    zero = 0;
    c = c + y;
    out[0] = out[0] - gCameraOrigin[0] + 120;
    out[1] = a - gCameraOrigin[1] + 80;
    c = c - gCameraOrigin[2];
    out[2] = c;
    if ((u16)(screen[0] + 0x20) > 0x130) {
        ring->update = zero;
        destroy = ring->destroy;
        if (destroy != 0) {
            Callback_InvokeActor(ring, destroy);
            ring->destroy = zero;
        }
        return;
    }
    mode = &ring->mode;
    switch (*mode) {
        default:
            worldPosition = &ring->sprite.position;
            break;
        case 0:
            if (ring->animationEnded != 0) {
                Anim_RestartSequence(&ring->sequence, SceneEffect_LoadRingAnimation(), 1, 0, zero);
                (*mode)++;
            }
        case 1:
            positionX = &ring->sprite.position.x;
            positionX[0] += 0x50;
            positionZ = &ring->sprite.position.z;
            positionZ[0] -= 0x50;
            worldPosition = (struct ContactPosition *)positionX;
            break;
        case 0x10:
            if (ring->animationEnded != 0) {
                Anim_RestartSequence(&ring->sequence, SceneEffect_LoadRingAnimation(), 1, 0, zero);
                (*mode)++;
            }
        case 0x11:
            positionX = &ring->sprite.position.x;
            positionX[0] -= 0x50;
            positionZ = &ring->sprite.position.z;
            positionZ[0] += 0x50;
            worldPosition = (struct ContactPosition *)positionX;
            break;
    }
    collider = &ring->collider;
    Collider_Refresh(collider);
    Collider_SetPosition(collider, worldPosition, 0);
}

void SceneEffect_DestroyRing(void *arg)
{
    struct SceneEffectRing *ring = arg;
    Collider_Remove(&ring->collider);
    Sprite_Remove(&ring->sprite);
}

extern const struct ContactPosition gUnk_0824F6B8, gUnk_0824F6C0;

void SceneEffect_InitializeRing(struct SceneEffectQueue *owner, struct SceneEffectRing *ring,
                                struct ContactPosition *pos, u32 mode)
{
    u32 animation = SceneEffect_LoadRingAnimation();
    void *data = ring->animation;
    struct ContactCollider *collider;
    void (*update)(void *);
    void (*destroy)(void *);
    AnimData_Load(data, 0x856e);
    AnimData_SelectPaletteBank(data, 0x101);
    Sprite_Init(&ring->sprite, data, 0);
    Anim_RestartSequence(&ring->sequence, animation, 0, 0, 0);
    ring->sprite.position = *pos;
    ring->animationEnded = 0;
    collider = &ring->collider;
    Collider_Init(collider, 0, 0x2101, 0, 0x10, &gUnk_0824F6B8, &gUnk_0824F6B8);
    Collider_SetAttackProperties(collider, owner->ringPower, owner->ringElement, 0x10, 0x80000, 0);
    collider->offset = gUnk_0824F6B8;
    collider->bounds = gUnk_0824F6C0;
    if (mode) {
        ring->mode = 0x10;
        {
            u8 facing = 0x60;
            collider->direction = facing;
        }
    } else {
        ring->mode = 0;
        {
            u8 facing = 0xe0;
            collider->direction = facing;
        }
    }
    update = SceneEffectRing_Update;
    destroy = SceneEffect_DestroyRing;
    ring->update = update;
    ring->destroy = destroy;
}

void SceneEntryPool_CreateEntry(struct SceneEffectQueue *pool, struct ContactPosition *position,
                                u32 mode)
{
    struct SceneEffectRing *entry = 0;
    s32 index;
    for (index = 0; index < 4; index++) {
        if (!((pool->ringsUsed >> index) & 1)) {
            entry = &pool->rings[index];
            pool->ringsUsed |= 1 << index;
            break;
        }
    }
    if (entry)
        SceneEffect_InitializeRing(pool, entry, position, mode);
}
