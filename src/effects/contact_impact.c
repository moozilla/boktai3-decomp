#include "effects/contact_impact.h"
#include "engine/math/fixed8_interpolation.h"

u32 PopInSprite_GetStateResult(struct ContactImpactEffect *p)
{
    return p ? p->stateResult : 0;
}

s32 __divsi3(s32, s32);

void OrbitMotion_BeginDistanceWave(void *, void *, void *, u32, u32);

static inline s32 Half(s32 value)
{
    return value >= 0 ? value >> 1 : -(-value >> 1);
}

static inline s32 Scale(s32 value)
{
    s32 scaled = value * 3;
    return scaled >= 0 ? scaled >> 5 : -(-scaled >> 5);
}

static inline void FromScreen(struct ContactPosition *out, const struct ContactPosition *screen,
                              s32 offset)
{
    s32 x = screen->x + (s16)gCameraOrigin[0] - 120;
    s32 y = screen->y + (s16)gCameraOrigin[1] - 80;
    out->x = Half(__divsi3((x + y) << 5, 3) + offset);
    out->y = offset;
    out->z = Half(__divsi3((y - x) << 5, 3) + offset);
}

static inline void ToScreen(struct ContactPosition *screen, struct ContactPosition *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 sum;
    s32 y;

    screen->x = Scale(x - z);
    sum = Scale(x + z);
    y = Scale(world->y);
    screen->y = sum - y;
    screen->z = sum + y;
    screen->x = screen->x - gCameraOrigin[0] + 120;
    screen->y = screen->y - gCameraOrigin[1] + 80;
    screen->z -= gCameraOrigin[2];
}

// confidence=low: configures a contact effect's movement and independent scale ramp.

static inline void ContactImpactEffect_EnterUpdate(struct ContactImpactEffect *effect,
                                                   void (*update)(struct ContactImpactEffect *))
{
    effect->update = update;
    effect->timer = 0;
    effect->stateResult = 0;
}

void ContactImpactEffect_BeginMotion(void *object, struct ContactPosition *from,
                                     struct ContactPosition *to, u32 duration, u32 trackMode,
                                     u32 rampDuration, s32 rampFrom, u32 rampTo)
{
    struct ContactImpactEffect *effect = object;

    if (!effect)
        return;
    if (from) {
        if (!effect->coordinateMode)
            FromScreen(&effect->world, from, 0);
        else
            effect->world = *from;
    } else {
        FromScreen(&effect->world, &effect->sprite.position, 512);
    }
    if (!effect->coordinateMode)
        FromScreen(&effect->target, to, 0);
    else if (effect->coordinateMode == 1)
        effect->target = *to;
    ToScreen(&effect->sprite.position, &effect->world);
    OrbitMotion_BeginDistanceWave(&effect->motion, &effect->world, &effect->target, duration,
                                  trackMode);
    if (rampFrom < 0)
        effect->rampFrom = (s8)effect->sprite.scaleX;
    else
        effect->rampFrom = rampFrom;
    effect->rampTo = rampTo;
    effect->shakeThreshold = rampDuration;
    effect->sprite.scaleX = effect->rampFrom;
    effect->sprite.scaleY = effect->rampFrom;
    effect->sprite.flags &= ~1;
    ContactImpactEffect_EnterUpdate(effect, ContactImpactEffect_UpdateContact);
}

void OrbitMotion_BeginArc(void *, void *, void *, u32, u32);

void SceneTarget_BeginMove(void *object, struct ContactPosition *from, struct ContactPosition *to,
                           s32 rampFrom, u32 rampTo, u32 arcParameter, u32 trackMode)
{
    struct ContactImpactEffect *effect = object;

    if (!effect || !to)
        return;
    if (from) {
        if (!effect->coordinateMode)
            FromScreen(&effect->world, from, 0);
        else
            effect->world = *from;
    } else if (!effect->coordinateMode) {
        FromScreen(&effect->world, &effect->sprite.position, 0);
    } else {
        FromScreen(&effect->world, &effect->sprite.position, to->y);
    }
    if (!effect->coordinateMode)
        FromScreen(&effect->target, to, 0);
    else
        effect->target = *to;
    if (rampFrom < 0)
        effect->rampFrom = (s8)effect->sprite.scaleX;
    else
        effect->rampFrom = rampFrom;
    effect->rampTo = rampTo;
    ToScreen(&effect->sprite.position, &effect->world);
    effect->sprite.scaleX = effect->rampFrom;
    effect->sprite.scaleY = effect->rampFrom;
    OrbitMotion_BeginArc(&effect->motion, &effect->world, &effect->target, arcParameter, trackMode);
    effect->shakeThreshold = trackMode;
    effect->sprite.flags &= ~1;
    ContactImpactEffect_EnterUpdate(effect, ContactImpactEffect_UpdateContact);
    Callback_InvokeActor(effect, effect->update);
}

// confidence=low: begins a stationary world-position effect with a timed callback.

void ContactImpactEffect_BeginTimedPosition(void *object, struct ContactPosition *from,
                                            u32 duration)
{
    struct ContactImpactEffect *effect = object;

    if (!effect)
        return;
    if (from) {
        if (!effect->coordinateMode)
            FromScreen(&effect->world, from, 0);
        else
            effect->world = *from;
    } else {
        FromScreen(&effect->world, &effect->sprite.position, 0);
    }
    effect->shakeThreshold = duration;
    effect->sprite.flags &= ~1;
    ContactImpactEffect_EnterUpdate(effect, ContactImpactEffect_UpdateShakingPosition);
}

void PopInSprite_ClearState(struct ContactImpactEffect *p)
{
    if (p) {
        ContactImpactEffect_EnterUpdate(p, NULL);
    }
}

// confidence=low: initializes linear world movement and a scale ramp without a motion track.

void ContactImpactEffect_BeginInterpolatedPosition(void *object, struct ContactPosition *from,
                                                   struct ContactPosition *to, s32 rampFrom,
                                                   u32 rampTo, u32 duration)
{
    struct ContactImpactEffect *effect = object;

    if (!effect)
        return;
    if (!effect->coordinateMode) {
        FromScreen(&effect->world, from, 0);
        FromScreen(&effect->target, to, 0);
        ToScreen(&effect->sprite.position, &effect->world);
    } else if (effect->coordinateMode == 1) {
        effect->world = *from;
        effect->target = *to;
        ToScreen(&effect->sprite.position, &effect->world);
    }
    effect->shakeThreshold = duration;
    effect->moveTimer = 0;
    if (rampFrom < 0)
        effect->rampFrom = (s8)effect->sprite.scaleX;
    else
        effect->rampFrom = rampFrom;
    effect->rampTo = rampTo;
    effect->sprite.scaleX = effect->rampFrom;
    effect->sprite.scaleY = effect->rampFrom;
    effect->sprite.flags &= ~1;
    ContactImpactEffect_EnterUpdate(effect, ContactImpactEffect_UpdateInterpolatedPosition);
}

// confidence=low: alternates the displayed frame between zero and one on staggered timers.

static inline void SetFrame(struct OrbitEffectSprite *sprite, s32 frame)
{
    sprite->frame = frame;
}

void ContactImpactEffect_UpdateBlinkFrame(struct ContactImpactEffect *effect)
{
    if (effect->sprite.flags & 1)
        return;
    switch (effect->blinkState) {
        case 0:
            if (++effect->blinkTimer > 1) {
                SetFrame(&effect->sprite, effect->blinkState);
                effect->blinkState = 1;
                effect->blinkTimer = 0;
            }
            break;
        case 1:
            if (++effect->blinkTimer > 0) {
                SetFrame(&effect->sprite, effect->blinkState);
                effect->blinkState = 0;
                effect->blinkTimer = 0;
            }
            break;
    }
}

void PopInSprite_StartAppearing(struct ContactImpactEffect *p)
{
    p->sprite.flags &= ~1;
    ContactImpactEffect_EnterUpdate(p, SpriteController_AnimateGrowing);
}

void PopInSprite_StartDisappearing(struct ContactImpactEffect *p)
{
    ContactImpactEffect_EnterUpdate(p, SpriteController_AnimateShrinking);
}

void PopInSprite_SetHidden(struct ContactImpactEffect *p)
{
    p->sprite.flags |= 1;
    ContactImpactEffect_EnterUpdate(p, SpriteController_UpdateStopped);
}

/* Installed at controller +0xF4 after setting drawable flag bit zero. */

void SpriteController_UpdateStopped(struct ContactImpactEffect *controller)
{
}

void SpriteController_AnimateGrowing(struct ContactImpactEffect *p)
{
    s32 duration = 8;
    s32 scale = Fixed8_Interpolate(38, 64, duration, p->timer);

    p->sprite.scaleX = scale;
    p->sprite.scaleY = scale;
    if (p->timer >= duration) {
        p->sprite.flags &= ~1;
        ContactImpactEffect_EnterUpdate(p, PopInSprite_UpdateVisibleIdle);
    } else {
        p->timer++;
    }
}

void SpriteController_AnimateShrinking(struct ContactImpactEffect *p)
{
    s32 duration = 8;
    s32 scale = Fixed8_Interpolate(64, 38, duration, p->timer);

    p->sprite.scaleX = scale;
    p->sprite.scaleY = scale;
    if (p->timer >= duration)
        PopInSprite_SetHidden(p);
    else
        p->timer++;
}

void PopInSprite_UpdateVisibleIdle(struct ContactImpactEffect *p)
{
    p->timer++;
}

s32 sub_08249350(s32, s32);

static inline void ApplyOffset(s16 *field, s32 random, s32 range)
{
    s32 offset = sub_08249350(random >> 4, range);
    *field += offset;
}

// confidence=low: projects the fixed world position and adds a growing random shake.

void ContactImpactEffect_UpdateShakingPosition(struct ContactImpactEffect *effect)
{
    const u16 *table;
    s32 range;
    u16 index;

    ToScreen(&effect->sprite.position, &effect->world);
    if (effect->timer >= effect->shakeThreshold)
        effect->stateResult = 1;
    range = effect->timer >> 2;
    table = gRandomTable;
    index = gRandomIndex + 1;
    index &= 0x3FF;
    gRandomIndex = index;
    ApplyOffset(&effect->sprite.position.x, table[index], range);
    index = (index + 1) & 0x3FF;
    gRandomIndex = index;
    ApplyOffset(&effect->sprite.position.y, table[index], range);
    effect->timer++;
}

void MotionTrack_Update(u8 *);

void ContactImpactEffect_UpdateInterpolatedPosition(struct ContactImpactEffect *effect)
{
    struct ContactPosition position;
    s32 scale =
        Fixed8_Interpolate(effect->rampFrom, effect->rampTo, effect->shakeThreshold, effect->timer);

    effect->sprite.scaleX = scale;
    effect->sprite.scaleY = scale;
    position.x = Fixed8_Interpolate(effect->world.x, effect->target.x, effect->shakeThreshold,
                                    effect->timer);
    position.y = Fixed8_Interpolate(effect->world.y, effect->target.y, effect->shakeThreshold,
                                    effect->timer);
    position.z = Fixed8_Interpolate(effect->world.z, effect->target.z, effect->shakeThreshold,
                                    effect->timer);
    ToScreen(&effect->sprite.position, &position);
    if (++effect->timer > effect->shakeThreshold)
        effect->stateResult = 1;
}

void ContactImpactEffect_UpdateContact(struct ContactImpactEffect *effect)
{
    s32 scale =
        Fixed8_Interpolate(effect->rampFrom, effect->rampTo, effect->shakeThreshold, effect->timer);

    effect->sprite.scaleX = scale;
    effect->sprite.scaleY = scale;
    ToScreen(&effect->sprite.position, &effect->world);
    MotionTrack_Update((u8 *)&effect->motion);
    if (++effect->timer > effect->shakeThreshold)
        effect->stateResult = 1;
}

/* ContactImpactEffect_OnUnusedHook: Empty function within the F3E4 actor implementation with no observed callers */

void ContactImpactEffect_OnUnusedHook(void)
{
}

void ContactImpactEffect_ProcessMessages(struct ContactImpactEffect *obj)
{
    struct SceneEventReceiver *receiver = &obj->receiver;
    s32 i = 0;
    if (i < receiver->count[gSceneEventDispatchState->phase]) {
        do {
            struct SceneEventDispatchState *globals = gSceneEventDispatchState;
            struct SceneEventCommand *command = receiver->pending[globals->phase][i];
            SceneMessages_SetPendingTrigger(receiver, command);
            switch (command->kind) {
                case 9:
                    PopInSprite_StartAppearing(obj);
                    Trigger_ConsumePending(receiver, 1);
                    break;
                case 10:
                    PopInSprite_StartDisappearing(obj);
                    Trigger_ConsumePending(receiver, 1);
                    break;
            }
            i++;
        } while (i < receiver->count[gSceneEventDispatchState->phase]);
    }
}

void PopInSprite_OnTaskUpdate(void *arg)
{
    struct ContactImpactEffect *p = arg;
    ContactImpactEffect_ProcessMessages(p);
    if (p->update) {
        ContactImpactEffect_UpdateBlinkFrame(p);
        Callback_InvokeActor(p, p->update);
    }
}

void PopInSprite_OnTaskDestroy(void *arg)
{
    struct ContactImpactEffect *p = arg;
    Sprite_Remove(&p->sprite);
    gRegistryF3E4Actor = 0;
}

struct ContactImpactEffect *PopInSprite_CreateHidden(s32 priority, s32 coordinateMode)
{
    struct ContactImpactEffect *effect = Task_Create(0xA, 0xFC);
    struct OrbitMotion *motion;

    if (effect)
        Task_SetCallbacks(effect, PopInSprite_OnTaskUpdate, PopInSprite_OnTaskDestroy);
    effect->coordinateMode = coordinateMode;
    AnimData_Load(effect->animation, 0x3641);
    Sprite_Init(&effect->sprite, effect->animation, 0x12);
    effect->sprite.priority = priority;
    ZeroFill8(&effect->sprite.position);
    effect->blinkTimer = 0;
    effect->blinkState = 0;
    motion = &effect->motion;
    motion->position = 0;
    motion->state = 0;
    motion->mode = 0;
    motion->timer = 0;
    PopInSprite_SetHidden(effect);
    return effect;
}

void PopInSprite_Show(void)
{
    PopInSprite_StartAppearing(gRegistryF3E4Actor);
}

void PopInSprite_Hide(void)
{
    PopInSprite_StartDisappearing(gRegistryF3E4Actor);
}

void SceneMessages_RegisterReceiver(void *, s32, s32);

/* Registry routine F3E4: creates its task (gRegistryF3E4Actor, sprite 0x3641) for
   the given name: -p x y z (default 0), -c palette bank (+0x4B), registers it
   for scene messages, and -i nonzero starts it via PopInSprite_SetHidden, else idles
   with update PopInSprite_UpdateVisibleIdle (tentative). */

struct ContactImpactEffect *PopInSprite_Create(u32 name)
{
    struct ContactImpactEffect **registry = &gRegistryF3E4Actor;
    struct ContactImpactEffect *effect = Task_Create(8, 0xFC);
    struct OrbitMotion *motion;

    if (effect)
        Task_SetCallbacks(effect, PopInSprite_OnTaskUpdate, PopInSprite_OnTaskDestroy);
    *registry = effect;
    effect->name = name;
    AnimData_Load(effect->animation, 0x3641);
    Sprite_Init(&effect->sprite, effect->animation, 2);
    effect->sprite.priority = 2;
    if (Script_SeekToKeyword('p')) {
        effect->sprite.position.x = Script_GetValue();
        effect->sprite.position.y = Script_GetValue();
        effect->sprite.position.z = Script_GetValue();
    } else {
        ZeroFill8(&effect->sprite.position);
    }
    if (Script_SeekToKeyword('c'))
        AnimData_SelectPaletteBank(effect->animation, Script_GetValue() + 0x4B);
    effect->coordinateMode = 2;
    effect->blinkTimer = 0;
    effect->blinkState = 0;
    motion = &effect->motion;
    motion->position = 0;
    motion->state = 0;
    motion->mode = 0;
    motion->timer = 0;
    SceneMessages_RegisterReceiver(&effect->receiver, effect->name, 10);
    if (Script_SeekToKeyword('i') && Script_GetValue())
        PopInSprite_SetHidden(effect);
    else
        ContactImpactEffect_EnterUpdate(effect, PopInSprite_UpdateVisibleIdle);
    return effect;
}
