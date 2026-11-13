#include "shipwheel_presentation.h"
#include "player_cue_sequence.h"

// Drawable layout shared with ActorDisplay_UpdateForegroundSprite.

void ScenePanelSubject_SlideOpen(struct ShipWheel *owner)
{
    owner->sprite.position.x -= 0xD2;
    owner->sprite.position.z += 0xD2;
    Anim_SetSequence(&owner->sequence, owner->sequenceResource, 1, 0, 0);
    PlayerCue_StepSequenceEnded(&owner->sprite, &owner->sequence);
    Sound_PlaySong(0x50E);
}

void ShipWheel_ProcessMessages(struct ShipWheel *obj)
{
    struct SceneEventReceiver *receiver = &obj->receiver;
    s32 i = 0;
    if (i < receiver->count[gSceneEventDispatchState->phase]) {
        do {
            struct SceneEventDispatchState *globals = gSceneEventDispatchState;
            struct SceneEventCommand *command = receiver->pending[globals->phase][i];
            SceneMessages_SetPendingTrigger(receiver, command);
            if (command->kind == 7)
                ScenePanelSubject_SlideOpen(obj);
            i++;
        } while (i < receiver->count[gSceneEventDispatchState->phase]);
    }
}

void ShipWheel_Update(struct ShipWheel *wheel)
{
    ShipWheel_ProcessMessages(wheel);
    if (wheel->palette.update != 0)
        Callback_InvokeActor(&wheel->palette.update, wheel->palette.update);
}

void ShipWheel_OnDestroy(struct ShipWheel *wheel)
{
    Sprite_Remove(&wheel->sprite);
    gShipWheel = 0;
}

s32 ShipWheel_Initialize(struct ShipWheel *wheel, void *owner)
{
    struct ContactSwarmSprite *sprite = &wheel->sprite;
    struct ContactSwarmGraphics *resource = &wheel->graphics;
    AnimData_Load(resource, 0x2173);
    TimePalette_InitTransition(&wheel->palette, gPaletteBankData + 0x2D20,
                               gPaletteBankData + 0x2D40, gPaletteBankData + 0x2D60);
    wheel->graphics.paletteData = wheel->palette.colors;
    wheel->graphics.paletteIndex = 0x169;
    Sprite_Init(sprite, resource, 0);
    wheel->sequenceResource = ScriptProc_Start(0x922E, 0x2990);
    if (!wheel->sequenceResource)
        return -1;
    Anim_SetSequence(&wheel->sequence, wheel->sequenceResource, 0, 0, 0);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)sprite, &wheel->sequence);
    if (Script_SeekToKeyword('p')) {
        sprite->position.x = Script_GetValue();
        sprite->position.y = Script_GetValue();
        sprite->position.z = Script_GetValue();
        wheel->owner = owner;
        SceneMessages_RegisterReceiver(&wheel->receiver, owner, 10);
        return 0;
    }
    return -1;
}

void ShipWheel_RequestStop(void)
{
    struct ShipWheel *wheel = gShipWheel;
    if (wheel != 0)
        wheel->sprite.flags |= 1;
}

void *ShipWheel_Create(void *a)
{
    void *wheel;
    if (gShipWheel != 0)
        return gShipWheel;
    wheel = Task_Create(0xb, 0xe4);
    if (wheel != 0) {
        Task_SetCallbacks(wheel, ShipWheel_Update, ShipWheel_OnDestroy);
        if (ShipWheel_Initialize(wheel, a) < 0) {
            Task_Kill(wheel);
            return 0;
        }
    }
    gShipWheel = wheel;
    return wheel;
}

void SceneTransition_Finish(struct SceneFade *fade)
{
    if (fade->exitScript)
        Script_Call(fade->exitScript, 0);
    Task_Kill(fade->target);
    Task_Kill(fade);
}

void SceneTransition_StartFadeOut(struct SceneFade *fade)
{
    ScreenTransition_SendCommand(1, 6, 4, 4, 4, 0xFFFF, 0);
    fade->update = RegistryC281_UpdateClosingFade;
    fade->timer = 0;
}

void SceneTransition_StartCompletionDelay(struct SceneFade *fade)
{
    if (fade->wait < 0) {
        SceneTransition_Finish(fade);
    } else {
        fade->update = RegistryC281_UpdateClosingDelay;
        fade->timer = 0;
    }
}

// Provisional object-family interpretation.

void SceneFade_BeginSoundFade(struct SceneFade *fade)
{
    u16 a[3];
    u16 *q = a;
    s32 x = 0x868, y = 0x276;
    s32 zero = 0;
    a[0] = x;
    q[1] = y;
    q[2] = x;
    Camera_SetOffset(0x3c, a);
    ScreenTransition_SendCommand(0, 4, 0x1f, 0x1f, 0x1f, 0xffff, zero);
    Sound_PlaySong(0x47e);
    fade->update = RegistryC281_UpdateOpeningDelay;
    fade->timer = zero;
}

// Provisional object-family interpretation.

// Tentative: configures the target movement.

void SceneFade_BeginTargetMove(struct SceneFade *fade)
{
    s16 a[3];
    u16 b[3];
    s16 *q = a;
    s32 x = 0x500, y = 0x200, z = 0x868;
    s32 zero = 0;
    a[0] = x;
    q[1] = y;
    q[2] = z;
    SceneTarget_BeginMove(fade->target, (struct ContactPosition *)&fade->from, a, 0x30, 0x20,
                          fade->firstDuration, 0x40);
    {
        u16 *r = b;
        s32 x = 0x768, y = 0x276;
        r[0] = x;
        r[1] = y;
        r[2] = z;
    }
    Camera_SetOffset(0xb4, b);
    fade->update = RegistryC281_UpdateFirstMovement;
    fade->timer = zero;
}

void SceneTransition_StartChildTransition(struct SceneFade *fade)
{
    ContactImpactEffect_BeginTimedPosition(fade->target, 0, 0x5a);
    fade->update = RegistryC281_UpdateRepositioning;
    fade->timer = 0;
}

// Provisional object-family interpretation.

// Tentative: configures the target movement.

void SceneFade_BeginSecondMove(struct SceneFade *fade)
{
    s16 a[3];
    s16 *q = a;
    s32 x = 0x668, y = 0x200, z = 0x768;
    s32 zero = 0;
    a[0] = x;
    q[1] = y;
    q[2] = z;
    SceneTarget_BeginMove(fade->target, 0, q, 0x20, 0xc, fade->secondDuration, 0x3c);
    fade->update = RegistryC281_UpdateSecondMovement;
    fade->timer = zero;
}

// Provisional object-family interpretation.

// Tentative: configures the target movement.

void SceneFade_BeginFinalMove(struct SceneFade *fade)
{
    s16 a[3];
    s16 *q = a;
    s32 x = 0x580, y = 0x200, z = 0x868;
    s32 zero = 0;
    a[0] = x;
    q[1] = y;
    q[2] = z;
    SceneTarget_BeginMove(fade->target, 0, q, 0xc, 1, fade->finalDuration, 0x3c);
    fade->update = RegistryC281_UpdateFinalMovement;
    fade->timer = zero;
}

void RegistryC281_UpdateOpeningDelay(struct SceneFade *fade)
{
    fade->timer++;
    if (fade->timer > 0x3c)
        SceneFade_BeginTargetMove(fade);
}

void RegistryC281_UpdateFirstMovement(struct SceneFade *fade)
{
    if (RegistryF3E4_GetStateResult(fade->target))
        SceneTransition_StartChildTransition(fade);
}

void RegistryC281_UpdateRepositioning(struct SceneFade *fade)
{
    if (RegistryF3E4_GetStateResult(fade->target))
        SceneFade_BeginSecondMove(fade);
}

void RegistryC281_UpdateSecondMovement(struct SceneFade *fade)
{
    if (RegistryF3E4_GetStateResult(fade->target))
        SceneFade_BeginFinalMove(fade);
}

void RegistryC281_UpdateFinalMovement(struct SceneFade *fade)
{
    if (RegistryF3E4_GetStateResult(fade->target)) {
        RegistryF3E4_ClearState(fade->target);
        SceneTransition_StartCompletionDelay(fade);
    }
}

void RegistryC281_UpdateClosingDelay(struct SceneFade *fade)
{
    fade->timer++;
    if (fade->timer > fade->wait)
        SceneTransition_StartFadeOut(fade);
}

void RegistryC281_UpdateClosingFade(struct SceneFade *fade)
{
    fade->timer++;
    if (fade->timer > 0x40)
        SceneTransition_Finish(fade);
}

void RegistryC281_Update(struct SceneFade *fade)
{
    if (fade->update != 0)
        Callback_InvokeActor(fade, fade->update);
}

void RegistryC281_Destroy(void *fade)
{
}

static inline void SetVec(struct SceneFadePosition *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

s32 SceneFade_Initialize(struct SceneFade *fade)
{
    struct SceneFadePosition from, to;
    if (Script_SeekToKeyword('s')) {
        from.x = Script_GetValue();
        from.y = Script_GetValue();
        from.z = Script_GetValue();
    } else {
        SetVec(&from, 0x880, 0x200, 0x868);
    }
    if (Script_SeekToKeyword('g')) {
        to.x = Script_GetValue();
        to.y = Script_GetValue();
        to.z = Script_GetValue();
    } else {
        SetVec(&to, 0x580, 0x200, 0x780);
    }
    {
        s32 v = Script_SeekToKeyword('w');
        if (v)
            v = Script_GetValue();
        fade->wait = v;
    }
    {
        s32 v = Script_SeekToKeyword('e');
        if (v)
            v = Script_GetValue();
        fade->exitScript = v;
    }
    fade->target = RegistryF3E4_CreateHidden(3, 1);
    fade->from = from;
    fade->to = to;
    fade->firstDuration = 0x80;
    fade->secondDuration = 0x80;
    fade->finalDuration = 0x80;
    SceneFade_BeginSoundFade(fade);
}

void *RegistryC281_Create(void)
{
    void *fade = Task_Create(0xb, 0x40);
    if (fade != 0) {
        Task_SetCallbacks(fade, RegistryC281_Update, RegistryC281_Destroy);
        if (SceneFade_Initialize(fade) < 0) {
            Task_Kill(fade);
            return 0;
        }
    }
    return fade;
}

s32 Registry1909_UpdateVisibilityFlicker(struct ProjectedDisplayEntry *actor, s32 elapsed,
                                         s32 intervals)
{
    s32 interval;
    u32 flags;
    if (elapsed > ((intervals + 1) * intervals) >> 1)
        return 1;
    interval = 0;
    flags = actor->flags;
    if (interval < intervals) {
        elapsed -= intervals;
        if (elapsed > 0) {
            do {
                interval++;
                if (interval >= intervals)
                    break;
                elapsed -= intervals - interval;
            } while (elapsed > 0);
        }
    }
    if ((interval & 1) == 0)
        flags |= 1;
    else
        flags &= ~1;
    actor->flags = flags;
    return 0;
}

void Registry1909_EnterHiddenState(struct FallingSceneSprite *actor)
{
    void (*f)(struct FallingSceneSprite *) = Registry1909_UpdateHiddenState;
    actor->update = f;
    actor->timer = 0;
}

void Registry1909_StartVisibilityFlicker(struct FallingSceneSprite *actor)
{
    void (*f)(struct FallingSceneSprite *) = Registry1909_UpdateFlickerState;
    actor->update = f;
    actor->timer = 0;
}

void Registry1909_StartLandingAnimation(struct FallingSceneSprite *actor)
{
    void (*f)(struct FallingSceneSprite *);
    Anim_SetKeyframes((u8 *)&actor->animation, (u8 *)&actor->resource, 0x3b, 2, 0);
    f = Registry1909_UpdateLandingAnimation;
    actor->update = f;
    actor->timer = 0;
}

void Registry1909_StartFalling(struct FallingSceneSprite *actor)
{
    void (*f)(struct FallingSceneSprite *);
    u32 z;
    actor->animation.flags &= ~1;
    actor->velocityY = (z = 0);
    Anim_SetKeyframes((u8 *)&actor->animation, (u8 *)&actor->resource, 0x3a, 1, z);
    f = Registry1909_UpdateFalling;
    actor->update = f;
    actor->timer = z;
}

static inline void SetFallingSpriteUpdate(void (*update)(struct FallingSceneSprite *),
                                          void (**slot)(struct FallingSceneSprite *))
{
    *slot = update;
}

void Registry1909_UpdateHiddenState(struct FallingSceneSprite *actor)
{
    actor->animation.flags |= 1;
    SetFallingSpriteUpdate(Registry1909_UpdateHiddenState, &actor->update);
    actor->timer = 0;
}

void Registry1909_UpdateLandingAnimation(struct FallingSceneSprite *actor)
{
    Anim_Step((u8 *)&actor->animation, (u8 *)&actor->resource);
    actor->timer++;
    if (actor->timer > 0x3b)
        Registry1909_StartVisibilityFlicker(actor);
}

void Registry1909_UpdateFlickerState(struct FallingSceneSprite *actor)
{
    u8 *a = (u8 *)&actor->animation;
    s32 *q = &actor->timer;
    if (Registry1909_UpdateVisibilityFlicker(a, *q, 8)) {
        Registry1909_EnterHiddenState(actor);
    } else {
        *q = *q + 1;
    }
}

void Registry1909_UpdateFalling(struct FallingSceneSprite *actor)
{
    u8 *q = (u8 *)&actor->animation.position;
    s32 x = actor->animation.position.x >> 8;
    s32 z = ((struct ContactPosition *)q)->z >> 8;
    s32 cellIndex;
    u8 *cell;
    u32 kind;
    s32 height;

    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cellIndex = 0;
    else
        cellIndex = gTerrainMap->rows[z] + x;

    cell = TerrainModifiers_FindCellOverride(cellIndex, 1);
    if (cell)
        cell += 4;
    else
        cell = gTerrainMap->cells + 12 + cellIndex * 4;

    kind = *cell >> 4;
    height = (*cell & 0xf) << 8;
    switch (kind) {
        case 1:
            height -= q[4];
            break;
        case 2:
            height -= q[0];
            break;
    }

    actor->velocityY -= 2;
    actor->animation.position.y += actor->velocityY;
    if (actor->animation.position.y < height) {
        actor->animation.position.y = height;
        if (actor->velocityY > -2) {
            Registry1909_StartLandingAnimation(actor);
            return;
        } else {
            actor->velocityY = (-actor->velocityY * 3) / 4;
        }
    }
    actor->timer++;
}

void FallingSceneSprite_ProcessMessages(struct FallingSceneSprite *actor)
{
    struct SceneEventReceiver *receiver = &actor->receiver;
    s32 i = 0;
    if (i < actor->receiver.count[gSceneEventDispatchState->phase]) {
        do {
            struct SceneEventDispatchState *globals = gSceneEventDispatchState;
            struct SceneEventCommand *command = receiver->pending[globals->phase][i];
            SceneMessages_SetPendingTrigger(receiver, command);
            if (command->kind == 8) {
                Registry1909_StartFalling(actor);
                Trigger_ConsumePending(&actor->receiver, 1);
            }
            i++;
        } while (i < receiver->count[gSceneEventDispatchState->phase]);
    }
}

void Registry1909_Update(struct FallingSceneSprite *actor)
{
    FallingSceneSprite_ProcessMessages(actor);
    if (actor->update != 0)
        Callback_InvokeActor(actor, actor->update);
}

void Registry1909_Destroy(struct FallingSceneSprite *actor)
{
    Anim_Remove(&actor->animation);
}

static inline void ClearVector(void *dest, u32 *zero)
{
    *zero = 0;
    CpuSet(zero, dest, 0x05000002);
}

s32 FallingSceneSprite_Initialize(struct FallingSceneSprite *actor, u32 id)
{
    struct ProjectedDisplayResource *resource = ScriptProc_Start(0xCB05, 0x35E3);
    struct ProjectedDisplayResource *header, *savedHeader;
    u8 zero[4];
    struct SceneFadePosition position;
    struct SceneFadePosition *pos;
    if (!resource)
        return -1;
    actor->resource = *resource;
    header = &actor->resource;
    ResourceHeader_Relocate(header, resource);
    {
        u32 found = Script_SeekToKeyword('p');
        savedHeader = header;
        if (found) {
            position.x = Script_GetValue();
            position.y = Script_GetValue();
            position.z = Script_GetValue();
            pos = &position;
        } else {
            ClearVector(&position, (u32 *)zero);
            pos = &position;
        }
    }
    Widget_InitLinked(&actor->animation, savedHeader, 0, 0, 2, 0, 60, pos);
    Anim_Step(&actor->animation, savedHeader);
    actor->id = id;
    SceneMessages_RegisterReceiver(&actor->receiver, id, 10);
    Registry1909_EnterHiddenState(actor);
}

void *SceneMessageSprite_Create(void *a)
{
    void *actor = Task_Create(8, 0xdc);
    if (actor != 0) {
        Task_SetCallbacks(actor, Registry1909_Update, Registry1909_Destroy);
        if (FallingSceneSprite_Initialize(actor, a) < 0) {
            Task_Kill(actor);
            return 0;
        }
    }
    return actor;
}
