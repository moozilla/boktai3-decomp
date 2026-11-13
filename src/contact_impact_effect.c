#include "contact_impact_effect.h"

u32 RegistryF3E4_GetStateResult(struct ContactImpactEffect *p)
{
    u32 r;
    if (p)
        r = p->stateResult;
    else
        r = 0;
    return r;
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
    sum += y;
    screen->z = sum;
    screen->x = screen->x - gCameraOrigin[0] + 120;
    screen->y = screen->y - gCameraOrigin[1] + 80;
    sum -= gCameraOrigin[2];
    screen->z = sum;
}

// confidence=low: configures a contact effect's movement and independent scale ramp.

static inline void SetCallback(void (*callback)(struct ContactImpactEffect *),
                               void (**slot)(struct ContactImpactEffect *))
{
    *slot = callback;
}

void ContactImpactEffect_BeginMotion(struct ContactImpactEffect *input,
                                     struct ContactPosition *source,
                                     struct ContactPosition *destination, u32 time, u32 trackMode,
                                     u32 rampDuration, s32 rampFrom, u32 rampTo)
{
    struct ContactPosition *from = source;
    struct ContactPosition *to = destination;
    u32 duration = time;
    struct ContactImpactEffect *effect = input;
    if (!effect)
        return;
    if (from) {
        if (!effect->coordinateMode)
            FromScreen(&effect->world, from, 0);
        else
            *&effect->world = *from;
    } else {
        FromScreen(&effect->world, &effect->sprite.position, 512);
    }
    if (!effect->coordinateMode) {
        FromScreen(&effect->target, to, 0);
    } else if (effect->coordinateMode == 1) {
        *&effect->target = *to;
    }
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
    SetCallback(ContactImpactEffect_UpdateContact, &effect->update);
    effect->timer = 0;
    effect->stateResult = 0;
}

void OrbitMotion_BeginArc(void *, void *, void *, u32, u32);

static inline void FromScreenCapture(struct ContactPosition *out,
                                     const struct ContactPosition *screen, s32 offset,
                                     struct ContactPosition **capture)
{
    s32 x = screen->x + (s16)gCameraOrigin[0] - 120;
    s32 y = screen->y + (s16)gCameraOrigin[1] - 80;
    s32 first = __divsi3((x + y) << 5, 3) + offset;
    *capture = out;
    out->x = Half(first);
    out->y = offset;
    out->z = Half(__divsi3((y - x) << 5, 3) + offset);
}

static inline void ToScreenArc(struct ContactPosition *screen, struct ContactPosition *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 sum;
    s32 y;
    s32 sy, depth;
    screen->x = Scale(x - z);
    sum = Scale(x + z);
    y = Scale(world->y);
    sy = sum - y;

    screen->y = sy;
    depth = sum + y;
    screen->z = depth;
    screen->x = screen->x - gCameraOrigin[0] + 120;
    screen->y = screen->y - gCameraOrigin[1] + 80;
    screen->z -= gCameraOrigin[2];
}

void SceneTarget_BeginMove(struct ContactImpactEffect *input, struct ContactPosition *source,
                           struct ContactPosition *destination, s32 initialScale, u32 rampTo,
                           u32 arcParameter, u32 trackMode)
{
    struct ContactPosition *from = source;
    struct ContactPosition *to = destination;
    s32 rampFrom = initialScale;
    struct ContactImpactEffect *effect = input;
    struct ContactPosition *origin;
    struct ContactPosition *target;
    if (!effect)
        return;
    if (!to)
        return;
    if (from) {
        if (!effect->coordinateMode) {
            struct ContactPosition *out = &effect->world;
            FromScreenCapture(out, from, 0, &origin);
        } else {
            struct ContactPosition *out = &effect->world;
            *out = *from;
            origin = out;
        }
    } else if (!effect->coordinateMode) {
        struct ContactPosition *out = &effect->world;
        struct ContactPosition *in = &effect->sprite.position;
        FromScreenCapture(out, in, 0, &origin);
    } else {
        struct ContactPosition *out = &effect->world;
        struct ContactPosition *in = &effect->sprite.position;
        s32 height = to->y;
        FromScreenCapture(out, in, height, &origin);
    }
    if (!effect->coordinateMode) {
        struct ContactPosition *out = &effect->target;
        FromScreenCapture(out, to, 0, &target);
    } else {
        struct ContactPosition *out = &effect->target;
        *out = *to;
        target = out;
    }
    if (rampFrom < 0)
        effect->rampFrom = (s8)effect->sprite.scaleX;
    else
        effect->rampFrom = rampFrom;
    effect->rampTo = rampTo;
    ToScreenArc(&effect->sprite.position, origin);
    effect->sprite.scaleX = effect->rampFrom;
    effect->sprite.scaleY = effect->rampFrom;
    OrbitMotion_BeginArc(&effect->motion, origin, target, arcParameter, trackMode);
    effect->shakeThreshold = trackMode;
    effect->sprite.flags &= ~1;
    SetCallback(ContactImpactEffect_UpdateContact, &effect->update);
    effect->timer = 0;
    effect->stateResult = 0;
    Callback_InvokeActor(effect, effect->update);
}

// confidence=low: begins a stationary world-position effect with a timed callback.

void ContactImpactEffect_BeginTimedPosition(struct ContactImpactEffect *input,
                                            struct ContactPosition *source, u32 time)
{
    struct ContactPosition *from = source;
    u32 duration = time;
    struct ContactImpactEffect *effect = input;
    if (effect) {
        if (from) {
            if (!effect->coordinateMode)
                FromScreen((struct ContactPosition *)&effect->world.x, from, 0);
            else
                *(struct ContactPosition *)&effect->world.x = *from;
        } else
            FromScreen((struct ContactPosition *)&effect->world.x,
                       (struct ContactPosition *)(u16 *)&effect->sprite.position.x, 0);
        effect->shakeThreshold = duration;
        effect->sprite.flags &= ~1;
        SetCallback(ContactImpactEffect_UpdateShakingPosition, &effect->update);
        effect->timer = 0;
        effect->stateResult = 0;
    }
}

void RegistryF3E4_ClearState(struct ContactImpactEffect *p)
{
    if (p) {
        u32 z = 0;
        p->update = z;
        p->timer = z;
        p->stateResult = z;
    }
}

static inline void Project(u16 *screen, struct ContactPosition *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 sum;
    s32 y;
    s32 sy, depth;
    screen[0] = Scale(x - z);
    sum = Scale(x + z);
    y = Scale(world->y);
    sy = sum - y;
    depth = sum + y;
    screen[0] = screen[0] - gCameraOrigin[0] + 120;
    screen[1] = sy - gCameraOrigin[1] + 80;
    screen[2] = depth - gCameraOrigin[2];
}

// confidence=low: initializes linear world movement and a scale ramp without a motion track.

void ContactImpactEffect_BeginInterpolatedPosition(struct ContactImpactEffect *input,
                                                   struct ContactPosition *source,
                                                   struct ContactPosition *destination,
                                                   s32 initialScale, u32 finalScale, u32 duration)
{
    struct ContactPosition *from = source;
    s32 rampFrom = initialScale;
    struct ContactImpactEffect *effect = input;
    if (effect) {
        if (!effect->coordinateMode) {
            struct ContactPosition *out = (struct ContactPosition *)(&effect->world.x);
            struct ContactPosition *origin;
            origin = out;
            FromScreen(out, from, 0);
            FromScreen((struct ContactPosition *)(&effect->target.x), destination, 0);
            Project((u16 *)&effect->sprite.position.x, (struct ContactPosition *)&effect->world.x);
        } else if (effect->coordinateMode == 1) {
            struct ContactPosition *origin = (struct ContactPosition *)(&effect->world.x);
            *origin = *from;
            *((struct ContactPosition *)(&effect->target.x)) = *destination;
            Project((u16 *)&effect->sprite.position.x, (struct ContactPosition *)&effect->world.x);
        }
        effect->shakeThreshold = duration;
        effect->moveTimer = 0;
        if (rampFrom < 0)
            effect->rampFrom = (s8)effect->sprite.scaleX;
        else
            effect->rampFrom = rampFrom;
        effect->rampTo = finalScale;
        effect->sprite.scaleX = effect->rampFrom;
        effect->sprite.scaleY = effect->rampFrom;
        effect->sprite.flags &= ~1;
        SetCallback(ContactImpactEffect_UpdateInterpolatedPosition, &effect->update);
        effect->timer = 0;
        effect->stateResult = 0;
    }
}

// confidence=low: alternates the displayed frame between zero and one on staggered timers.

static inline void SetFrame(struct OrbitEffectSprite *sprite, s32 frame)
{
    sprite->frame = frame;
}

void ContactImpactEffect_UpdateBlinkFrame(struct ContactImpactEffect *effect)
{
    u32 flags = effect->sprite.flags;
    u32 one = 1;
    flags &= one;
    if (!flags) {
        switch (effect->blinkState) {
            case 0:
                if (++effect->blinkTimer > 1) {
                    {
                        s32 frame = effect->blinkState;
                        SetFrame(&effect->sprite, frame);
                    }
                    effect->blinkState = one;
                    effect->blinkTimer = flags;
                }
                break;
            case 1:
                if (++effect->blinkTimer > 0) {
                    {
                        s32 frame = effect->blinkState;
                        SetFrame(&effect->sprite, frame);
                    }
                    effect->blinkState = flags;
                    effect->blinkTimer = flags;
                }
                break;
        }
    }
}

void RegistryF3E4_StartAppearing(struct ContactImpactEffect *p)
{
    void (*f)(struct ContactImpactEffect *);
    p->sprite.flags &= ~1;
    f = SpriteController_AnimateGrowing;
    p->update = f;
    p->timer = 0;
    p->stateResult = 0;
}

void RegistryF3E4_StartDisappearing(struct ContactImpactEffect *p)
{
    void (*f)(struct ContactImpactEffect *) = SpriteController_AnimateShrinking;
    p->update = f;
    p->timer = 0;
    p->stateResult = 0;
}

void RegistryF3E4_SetHidden(struct ContactImpactEffect *p)
{
    void (*f)(struct ContactImpactEffect *);
    p->sprite.flags |= 1;
    f = SpriteController_UpdateStopped;
    p->update = f;
    p->timer = 0;
    p->stateResult = 0;
}

/* Installed at controller +0xF4 after setting drawable flag bit zero. */

void SpriteController_UpdateStopped(struct ContactImpactEffect *controller)
{
}

void SpriteController_AnimateGrowing(struct ContactImpactEffect *p)
{
    s32 counter = p->timer;
    s32 value;
    s32 limit2;

    if (counter > 0)
        goto checkUpper;
    value = 0x26;
    goto store;

checkUpper: {
    s32 limit = 8;
    if (counter < limit)
        goto interp;
}
    value = 0x40;
    goto store;

interp: {
    s32 k = 0xd0 << 5;
    s32 t = counter * k;
    s32 d = 8;
    s32 result;
    t = t / d;
    if (t >= 0)
        result = t >> 8;
    else
        result = -(-t >> 8);
    value = result;
    value += 0x26;
}

store:
    p->sprite.scaleX = value;
    p->sprite.scaleY = value;

    limit2 = 8;
    if ((s32)p->timer >= limit2) {
        p->sprite.flags &= -2;
        SetCallback(RegistryF3E4_UpdateVisibleIdle, &p->update);
        p->timer = 0;
        p->stateResult = 0;
    } else {
        p->timer++;
    }
}

void SpriteController_AnimateShrinking(struct ContactImpactEffect *p)
{
    s32 scale;
    s32 duration = 8;
    s32 timer = p->timer;
    if (timer <= 0)
        scale = 0x40;
    else if (timer >= duration)
        scale = 0x26;
    else {
        s32 delta = -0x1a00;
        delta *= timer;
        delta = __divsi3(delta, duration);
        if (delta >= 0)
            delta >>= 8;
        else
            delta = -((-delta) >> 8);
        scale = delta + 0x40;
    }
    p->sprite.scaleX = scale;
    p->sprite.scaleY = scale;
    if (p->timer >= duration)
        RegistryF3E4_SetHidden(p);
    else
        p->timer++;
}

void RegistryF3E4_UpdateVisibleIdle(struct ContactImpactEffect *p)
{
    p->timer++;
}

s32 sub_08249350(s32, s32);

static inline void ApplyOffset(u16 *field, s32 random, s32 range)
{
    s32 offset = sub_08249350(random >> 4, range);
    *field += offset;
}

// confidence=low: projects the fixed world position and adds a growing random shake.

void ContactImpactEffect_UpdateShakingPosition(struct ContactImpactEffect *effect)
{
    s32 *timer;
    s32 range;
    const u16 *table;
    u32 *seed;
    u16 index;
    u32 mask;
    s32 random;
    Project((u16 *)&effect->sprite.position.x, (struct ContactPosition *)&effect->world.x);
    timer = &effect->timer;
    if (*timer >= effect->shakeThreshold)
        effect->stateResult = 1;
    range = *timer >> 2;
    table = gRandomTable;
    seed = &gRandomIndex;
    index = *seed + 1;
    mask = 0x3FF;
    index &= mask;
    *seed = index;
    random = table[index];
    ApplyOffset((u16 *)&effect->sprite.position.x, random, range);
    index = (index + 1) & mask;
    *seed = index;
    random = table[index];
    ApplyOffset((u16 *)&effect->sprite.position.y, random, range);
    ++*timer;
}

void MotionTrack_Update(u8 *);

static inline s32 EffectProjectScale(s32 x)
{
    s32 r;
    s32 y = x * 3;
    if (y >= 0)
        r = y >> 5;
    else
        r = -((-y) >> 5);
    return r;
}

static inline s32 EffectScale8(s32 x)
{
    s32 r;
    if (x >= 0)
        r = x >> 8;
    else
        r = -((-x) >> 8);
    return r;
}

static inline s32 InterpolatePosition(s32 start, s32 end, s32 duration, s32 time)
{
    if (time <= 0)
        return start;
    if (time >= duration)
        return end;
    return start + EffectScale8(((end - start) << 8) * time / duration);
}

void ContactImpactEffect_UpdateInterpolatedPosition(struct ContactImpactEffect *effect)
{
    {
        u16 *base = &effect->rampFrom;
        s32 rampTo = effect->rampTo;
        s32 threshold = effect->shakeThreshold;
        s32 rampFrom = *(u16 *)base;
        s32 *timerPtr = &effect->timer;
        s32 timer = *timerPtr;
        s32 result;
        if (timer <= 0)
            result = rampFrom;
        else if (timer >= threshold)
            result = rampTo;
        else {
            s32 delta = rampTo - rampFrom;
            s32 scaled = __divsi3(timer * (delta << 8), threshold);
            result = rampFrom + EffectScale8(scaled);
        }
        effect->sprite.scaleX = result;
        effect->sprite.scaleY = result;
    }
    {
        struct ContactImpactPackedPosition pos;
        u16 *q;
        struct ContactPosition *p;
        s32 x;
        s32 z;
        s32 y;
        s32 sum;
        pos.x = InterpolatePosition(effect->world.x, effect->target.x, effect->shakeThreshold,
                                    effect->timer);
        pos.y = InterpolatePosition(effect->world.y, effect->target.y, effect->shakeThreshold,
                                    effect->timer);
        pos.z = InterpolatePosition(effect->world.z, effect->target.z, effect->shakeThreshold,
                                    effect->timer);
        q = (u16 *)&effect->sprite.position.x;
        p = (struct ContactPosition *)&pos;
        x = ((s16 *)&pos)[0];
        z = p->z;
        q[0] = EffectProjectScale(x - z);
        sum = EffectProjectScale(x + z);
        {
            struct ContactPosition *source = p;
            y = EffectProjectScale(source->y);
        }
        {
            s32 sy = sum - y;
            s32 depth = sum + y;
            q[0] = q[0] - gCameraOrigin[0] + 120;
            q[1] = sy - gCameraOrigin[1] + 80;
            q[2] = depth - gCameraOrigin[2];
        }
    }
    {
        s32 *timer = &effect->timer;
        s32 t = ++*timer;
        {
            u16 *limitp = &effect->shakeThreshold;
            s32 limit;
            limit = *limitp;
            if (t > limit)
                effect->stateResult = 1;
        }
    }
}

void ContactImpactEffect_UpdateContact(struct ContactImpactEffect *effect)
{
    {
        u16 *base = &effect->rampFrom;
        s32 rampTo = effect->rampTo;
        s32 threshold = effect->shakeThreshold;
        s32 rampFrom = *(u16 *)base;
        s32 *timerPtr = &effect->timer;
        s32 timer = *timerPtr;
        s32 result;

        if (timer <= 0)
            result = rampFrom;
        else if (timer >= threshold)
            result = rampTo;
        else {
            s32 delta = rampTo - rampFrom;
            s32 scaled = __divsi3(timer * (delta << 8), threshold);
            result = rampFrom + EffectScale8(scaled);
        }
        effect->sprite.scaleX = result;
        effect->sprite.scaleY = result;
    }

    {
        u16 *q = (u16 *)&effect->sprite.position.x;
        s16 *p = &effect->world.x;
        s32 y;
        s32 x, zz;
        s32 z;
        x = p[0];
        zz = p[2];
        q[0] = EffectProjectScale(x - zz);
        z = EffectProjectScale(x + zz);
        y = EffectProjectScale(p[1]);
        q[1] = z - y;
        z += y;
        q[0] = q[0] - gCameraOrigin[0] + 0x78;
        q[1] = q[1] - gCameraOrigin[1] + 0x50;
        z -= gCameraOrigin[2];
        q[2] = z;
    }

    MotionTrack_Update((u8 *)&effect->motion);

    {
        s32 *timer = &effect->timer;
        s32 t = ++*timer;

        if (t > effect->shakeThreshold) {
            effect->stateResult = 1;
        }
    }
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
                    RegistryF3E4_StartAppearing(obj);
                    Trigger_ConsumePending(receiver, 1);
                    break;
                case 10:
                    RegistryF3E4_StartDisappearing(obj);
                    Trigger_ConsumePending(receiver, 1);
                    break;
            }
            i++;
        } while (i < receiver->count[gSceneEventDispatchState->phase]);
    }
}

void RegistryF3E4_OnTaskUpdate(void *arg)
{
    struct ContactImpactEffect *p = arg;
    ContactImpactEffect_ProcessMessages(p);
    if (p->update) {
        ContactImpactEffect_UpdateBlinkFrame(p);
        Callback_InvokeActor(p, p->update);
    }
}

void RegistryF3E4_OnTaskDestroy(void *arg)
{
    struct ContactImpactEffect *p = arg;
    Sprite_Remove(&p->sprite);
    gRegistryF3E4Actor = 0;
}

void CpuSet(const void *src, void *dest, u32 control);

static inline void fill32(void *dst, u32 *tmp, u32 control)
{
    *tmp = 0;
    CpuSet(tmp, dst, control);
}

struct ContactImpactEffect *RegistryF3E4_CreateHidden(s32 field3B, s32 mode)
{
    struct ContactImpactEffect *p = Task_Create(0xA, 0xFC);
    u32 zero;
    if (p)
        Task_SetCallbacks(p, RegistryF3E4_OnTaskUpdate, RegistryF3E4_OnTaskDestroy);
    p->coordinateMode = mode;
    AnimData_Load(p->animation, 0x3641);
    Sprite_Init(&p->sprite, p->animation, 0x12);
    p->sprite.unk_04[3] = field3B;
    fill32(&p->sprite.position.x, &zero, 0x05000002);
    p->blinkTimer = 0;
    p->blinkState = 0;
    {
        struct OrbitMotion *in = &p->motion;
        in->position = 0;
        in->state = 0;
        in->unk28 = 0;
        in->timer = 0;
    }
    RegistryF3E4_SetHidden(p);
    return p;
}

void RegistryF3E4_Show(void)
{
    RegistryF3E4_StartAppearing(gRegistryF3E4Actor);
}

void RegistryF3E4_Hide(void)
{
    RegistryF3E4_StartDisappearing(gRegistryF3E4Actor);
}

void SceneMessages_RegisterReceiver(void *, s32, s32);

/* Registry routine F3E4: creates its task (gRegistryF3E4Actor, sprite 0x3641) for
   the given name: -p x y z (default 0), -c palette bank (+0x4B), registers it
   for scene messages, and -i nonzero starts it via RegistryF3E4_SetHidden, else idles
   with update RegistryF3E4_UpdateVisibleIdle (tentative). */

struct ContactImpactEffect *RegistryF3E4_Create(u32 name)
{
    struct ContactImpactEffect **current = &gRegistryF3E4Actor;
    struct ContactImpactEffect *p = Task_Create(8, 0xFC);
    u32 zero;
    if (p)
        Task_SetCallbacks(p, RegistryF3E4_OnTaskUpdate, RegistryF3E4_OnTaskDestroy);
    *current = p;
    p->name = name;
    AnimData_Load(p->animation, 0x3641);
    Sprite_Init(&p->sprite, p->animation, 2);
    p->sprite.unk_04[3] = 2;
    if (Script_SeekToKeyword('p')) {
        p->sprite.position.x = Script_GetValue();
        p->sprite.position.y = Script_GetValue();
        p->sprite.position.z = Script_GetValue();
    } else
        fill32(&p->sprite.position.x, &zero, 0x05000002);
    if (Script_SeekToKeyword('c'))
        AnimData_SelectPaletteBank(p->animation, Script_GetValue() + 0x4B);
    p->coordinateMode = 2;
    p->blinkTimer = 0;
    p->blinkState = 0;
    {
        struct OrbitMotion *in = &p->motion;
        in->position = 0;
        in->state = 0;
        in->unk28 = 0;
        in->timer = 0;
    }
    SceneMessages_RegisterReceiver(&p->receiver, p->name, 10);
    if (Script_SeekToKeyword('i') && Script_GetValue())
        RegistryF3E4_SetHidden(p);
    else {
        SetCallback(RegistryF3E4_UpdateVisibleIdle, &p->update);
        p->timer = 0;
        p->stateResult = 0;
    }
    return p;
}
