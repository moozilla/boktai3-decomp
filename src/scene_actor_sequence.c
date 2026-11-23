#include "global.h"
#include "scene_actor_sequence.h"
#include "fixed8_interpolation.h"
#include "render.h"

void BgGfx_LoadTilemapAtPosition(u32, s32, s32);

void SceneActorSequence_UpdateBackgroundScroll(struct SceneActorSequence *p)
{
    BgGfx_LoadTilemapAtPosition(1, p->layer1X, p->layer1Y);
    BgGfx_LoadTilemapAtPosition(3, p->layer3X, p->layer3Y);
}

void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);

void SceneActorSequence_UpdateFadeIn(struct SceneActorSequence *);

void SceneActorSequence_BeginFadeIn(struct SceneActorSequence *p)
{
    ScreenTransition_SendCommand(0, 6, 4, 4, 4, 0xFFFF, 0);
    p->updateState = SceneActorSequence_UpdateFadeIn;
    p->timer = 0;
}

void SceneActorSequence_BeginDownwardMotion(struct SceneActorSequence *p);

void SceneActorSequence_UpdateFadeIn(struct SceneActorSequence *p)
{
    p->layer3X = Fixed8_Interpolate(320, 192, 160, p->timer);
    p->layer1X = Fixed8_Interpolate(224, 32, 160, p->timer);

    {
        s32 n = p->timer + 1;
        p->timer = n;
        if (n > 159)
            SceneActorSequence_BeginDownwardMotion(p);
    }
}

void SceneTarget_BeginMove(void *, void *, void *, int, int, int, int);

void SceneActorSequence_UpdateFadeInHold(struct SceneActorSequence *);

void SceneActorSequence_BeginDownwardMotion(struct SceneActorSequence *p)
{
    struct SceneActorSequencePoint start, end;
    s16 *q = (s16 *)&start;
    int x = 120, y = 80, zero = 0;
    start.x = x;
    q[1] = y;
    q[2] = zero;
    {
        s16 *r = (s16 *)&end;
        int y2 = 144;
        r[0] = x;
        r[1] = y2;
        r[2] = zero;
    }
    SceneTarget_BeginMove(p->actor, &start, &end, 1, 64, 128, 60);
    p->updateState = SceneActorSequence_UpdateFadeInHold;
    p->timer = zero;
}

s32 RegistryF3E4_GetStateResult(void *);

void SceneActorSequence_BeginCenterMotion(struct SceneActorSequence *p);

void SceneActorSequence_UpdateFadeInHold(struct SceneActorSequence *p)
{
    p->layer3X = Fixed8_Interpolate(192, 168, 64, p->timer);
    p->layer1X = Fixed8_Interpolate(32, 16, 64, p->timer);

    {
        s32 n = p->timer + 1;
        p->timer = n;
    }

    if (RegistryF3E4_GetStateResult(p->actor)) {
        if (p->timer > 63)
            SceneActorSequence_BeginCenterMotion(p);
    }
}

void ContactImpactEffect_BeginTimedPosition(void *, u32, u32);

void SceneActorSequence_UpdateCenterMotion(struct SceneActorSequence *);

void SceneActorSequence_BeginCenterMotion(struct SceneActorSequence *p)
{
    ContactImpactEffect_BeginTimedPosition(p->actor, 0, 0x5a);
    p->updateState = SceneActorSequence_UpdateCenterMotion;
    p->timer = 0;
}

s32 RegistryF3E4_GetStateResult(void *);

void SceneActorSequence_BeginCornerMotion(struct SceneActorSequence *);

void SceneActorSequence_UpdateCenterMotion(struct SceneActorSequence *p)
{
    p->timer = p->timer + 1;
    if (RegistryF3E4_GetStateResult(p->actor) != 0)
        SceneActorSequence_BeginCornerMotion(p);
}

void SceneTarget_BeginMove(void *, void *, void *, int, int, int, int);

void SceneActorSequence_UpdateFirstWaypointMotion(struct SceneActorSequence *);

void SceneActorSequence_BeginCornerMotion(struct SceneActorSequence *p)
{
    struct SceneActorSequencePoint end;
    s16 *q = (s16 *)&end;
    int xy = 48, zero = 0;
    end.x = xy;
    q[1] = xy;
    q[2] = zero;
    SceneTarget_BeginMove(p->actor, 0, (s16 *)&end, -1, xy, p->secondWaypointSpeed, 45);
    p->updateState = SceneActorSequence_UpdateFirstWaypointMotion;
    p->timer = zero;
}

void SceneActorSequence_BeginSecondWaypointMotion(struct SceneActorSequence *);

void SceneActorSequence_UpdateFirstWaypointMotion(struct SceneActorSequence *p)
{
    if (RegistryF3E4_GetStateResult(p->actor) != 0)
        SceneActorSequence_BeginSecondWaypointMotion(p);
}

void SceneActorSequence_UpdateSecondWaypointMotion(struct SceneActorSequence *);

void SceneActorSequence_BeginSecondWaypointMotion(struct SceneActorSequence *p)
{
    struct SceneActorSequencePoint v;
    s16 *q = (s16 *)&v;
    int a = 0x70, b = 0x48, z = 0;
    v.x = a;
    q[1] = b;
    q[2] = z;
    SceneTarget_BeginMove(p->actor, 0, (s16 *)&v, -1, 0x10, p->firstWaypointSpeed, 0x32);
    p->updateState = SceneActorSequence_UpdateSecondWaypointMotion;
    p->timer = 0;
}

void SceneActorSequence_BeginFinalWaypointMotion(struct SceneActorSequence *);

void SceneActorSequence_UpdateSecondWaypointMotion(struct SceneActorSequence *p)
{
    if (RegistryF3E4_GetStateResult(p->actor) != 0)
        SceneActorSequence_BeginFinalWaypointMotion(p);
}

void SceneActorSequence_UpdateFinalWaypointMotion(struct SceneActorSequence *);

void SceneActorSequence_BeginFinalWaypointMotion(struct SceneActorSequence *p)
{
    struct SceneActorSequencePoint v;
    s16 *q = (s16 *)&v;
    int a = 0x36, b = 0x52, z = 0;
    v.x = a;
    q[1] = b;
    q[2] = z;
    SceneTarget_BeginMove(p->actor, 0, (s16 *)&v, -1, 0x1, p->finalWaypointSpeed, 0x3C);
    p->updateState = SceneActorSequence_UpdateFinalWaypointMotion;
    p->timer = 0;
}

void SceneActorSequence_BeginFadeOut(struct SceneActorSequence *);

void SceneActorSequence_UpdateFinalWaypointMotion(struct SceneActorSequence *p)
{
    if (RegistryF3E4_GetStateResult(p->actor) != 0)
        SceneActorSequence_BeginFadeOut(p);
}

void RegistryF3E4_ClearState(void *);

void SceneActorSequence_UpdateCompletionDelay(struct SceneActorSequence *);

void SceneActorSequence_BeginFadeOut(struct SceneActorSequence *p)
{
    RegistryF3E4_ClearState(p->actor);
    ScreenTransition_SendCommand(1, 6, 4, 4, 4, 0xFFFF, 0);
    p->updateState = SceneActorSequence_UpdateCompletionDelay;
    p->timer = 0;
}

void Script_Call(void *, u32);

void Task_Kill(void *);

void SceneActorSequence_UpdateCompletionDelay(struct SceneActorSequence *p)
{
    p->timer = p->timer + 1;
    if (p->timer > 0x3f && p->completionScript != 0) {
        Script_Call(p->completionScript, 0);
        Task_Kill(p->actor);
        Task_Kill(p);
    }
}

static inline s32 SceneActorSequence_ReadAnimationPhase(const s16 *state)
{
    return *state;
}

void SceneActorSequence_UpdateActorAnimation(struct SceneActorSequenceActor *p)
{
    if (p->flags & 1)
        return;
    switch (p->animationState) {
        case 0:
            if (++p->animationTimer > 1) {
                p->animationFrame = SceneActorSequence_ReadAnimationPhase(&p->animationState);
                p->animationState = 1;
                p->animationTimer = 0;
            }
            break;
        case 1:
            if (++p->animationTimer > 0) {
                p->animationFrame = SceneActorSequence_ReadAnimationPhase(&p->animationState);
                p->animationState = 0;
                p->animationTimer = 0;
            }
            break;
    }
}

void SceneActorSequence_UpdateBackgroundScroll(struct SceneActorSequence *);

void Callback_InvokeActor(void *, u32);

void SceneActorSequence_Update(struct SceneActorSequence *p)
{
    SceneActorSequence_UpdateBackgroundScroll(p);
    if (p->updateState != 0)
        Callback_InvokeActor(p, p->updateState);
}

void SceneActorSequence_OnDestroy(void)
{
}

#define gDisplayControlShadow gDispCntShadow

void Bg_SetupLayers(int, int, void *, int, int, int, void *);

void *ScriptProc_Start(u16, u16);

void Bg_SetPriority(u32, s32);

u8 *sub_08216E54(u16);

void SceneActorSequence_InitDisplayResources(struct SceneActorSequence *p)
{
    u8 *resource;
    u8 *palette;
    s32 layers[2];
    {
        u16 *control = &gDispCntShadow;
        u32 mask = 0xfffff0ff;
        u16 value = *control;
        mask &= value;
        {
            u16 enabled = 0xa00;
            *control = mask | enabled;
        }
    }
    resource = sub_08216E54(0x98e2);
    p->background = resource;
    layers[0] = 1;
    layers[1] = 0;
    Bg_SetupLayers(4, 0, resource, 0, 0, 2, layers);
    Bg_SetPriority(1, 2);
    Bg_SetPriority(3, 3);
    palette = (u8 *)ScriptProc_Start(0x92b3, 0x3cca) + 0x14;
    p->palette = palette;
    CpuSet(palette, gBgPaletteBuffer, 0x4000080);
}

void SceneActorSequence_InitDisplayResources(struct SceneActorSequence *);

u32 RegistryF3E4_CreateHidden(s32, s32);

void SceneActorSequence_BeginFadeIn(struct SceneActorSequence *);

s32 SceneActorSequence_InitFromScript(struct SceneActorSequence *p, void *scriptContext)
{
    u32 e;
    SceneActorSequence_InitDisplayResources(p);
    p->actor = RegistryF3E4_CreateHidden(2, 0);
    ZeroFill8(&p->layer3X);
    ZeroFill8(&p->layer1X);
    p->layer1X = 0xE0;
    p->layer3X = 0x140;
    if ((e = Script_SeekToKeyword('e')) != 0)
        e = Script_GetValue();
    p->completionScript = e;
    p->secondWaypointSpeed = 0x70;
    p->firstWaypointSpeed = 0x68;
    p->finalWaypointSpeed = 0x64;
    SceneActorSequence_BeginFadeIn(p);
#ifdef UBFIX
    // falls off the end with r0 left over from the SceneActorSequence_BeginFadeIn call; the creator only tests
    // < 0, and this initializer has no failure path here
    return 0;
#endif
}

void *Task_Create(u32, u32);

void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));

s32 SceneActorSequence_InitFromScript(struct SceneActorSequence *, void *);

void SceneActorSequence_Update(struct SceneActorSequence *);

void SceneActorSequence_OnDestroy(void);

void *SceneActorSequence_CreateFromScript(void *a)
{
    void *p = Task_Create(8, 0x60);
    if (p != 0) {
        Task_SetCallbacks(p, SceneActorSequence_Update, SceneActorSequence_OnDestroy);
        if (SceneActorSequence_InitFromScript(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
