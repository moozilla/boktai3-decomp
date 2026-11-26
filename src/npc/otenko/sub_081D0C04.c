#include "npc/otenko/otenko_actor.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/task_manager.h"
struct PackedVec {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
struct OtenkoActor *Actor_Allocate(s32 name, s32 variant);
s32 Actor_InitMotion(struct OtenkoActor *, s32 name, struct PackedVec *, s32 angle);
s32 Actor_InitDrawable(struct OtenkoActor *, s32 kind, s32 a, s32 b, s32 c);
void Actor_InitShadow(struct OtenkoActor *, u32, s32, s32, s32);
void SceneMessages_RegisterReceiver(void *, s32, s32);
void Actor_InitResources(void *, struct OtenkoActor *, s32, s32);
void Actor_LinkToManager(void *, struct OtenkoActor *);
void Actor_InitBehaviorFromScript(void *, struct OtenkoActor *);
void DisplayAnimation_SetFlags(void *, u32);
void Actor_DisableShadow(struct OtenkoActor *);
s32 Actor_SetMirroredFourWayAnimation(struct OtenkoActor *, s32, s32, s32, s32, s32);
u32 ParticlePool_Create(void);
void Otenko_EnterProjectedIdle(void *, struct OtenkoActor *);
void Otenko_OnStateChange(void);
void sub_081D0864(void);
void Otenko_OnModeUpdate(void);
void Otenko_OnStateUpdate(void);
void Otenko_OnStateEffectUpdate(void);
void Otenko_OnDestroy(void);
void Actor_OnIdleUpdate(void);
void Actor_OnHiddenIdleUpdate(void);
void Otenko_FollowSceneTarget(void);
void Otenko_PlayAnimation12(void);
void Actor_OnConfiguredAnimationUpdate(void);

static inline void setBox(struct OtenkoActor *npc, s32 width, s32 height)
{
    npc->extentX = width;
    npc->extentZ = height;
    npc->rangeX = width;
    npc->rangeZ = height;
}

/* Registry routine BB34: creates a scene NPC (0x384-byte object from
   Actor_Allocate) named -n, variant -k (default 1), at -p x y z facing -d, with
   behavior -i (0-4: update callback, mode and state), records it in
   gSceneNpc and registers it with the scene actor manager; -K also runs
   Otenko_EnterProjectedIdle on it. */
void Otenko_CreateFromScript(void)
{
    void (*update)(void);
    u8 mode;
    void (*updateB)(void);
    void *manager = gSceneActorListManager;
    s32 name;
    s32 variant;
    s32 angle;
    s32 extra;
    struct OtenkoActor *npc;
    void (*updateC)(void);
    struct PackedVec position;
    u32 zero;
    struct OtenkoEffectState *effect;
    struct SceneActorBody *body;
    if (!manager) {
        return;
    }
    name = Script_GetKeywordValue('n', 0);
    variant = Script_GetKeywordValue('k', 1);
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    angle = Script_GetKeywordValue('d', 0);
    extra = Script_GetKeywordValue('K', 0);
    npc = Actor_Allocate(name, variant);
    if (!npc) {
        return;
    }
    gSceneNpc = (void *)npc;
    if (Actor_InitMotion(npc, name, &position, angle) < 0) {
        return;
    }
    if (Actor_InitDrawable(npc, 1, 0x97D3, 0xC6A0, zero = 0) < 0) {
        return;
    }
    body = &npc->body;
    {
        s32 speed = 400;
        body->positionOffset.x = zero;
        body->positionOffset.y = speed;
        body->positionOffset.z = zero;
    }
    Actor_InitShadow(npc, (u32)npc->display.position, 1, 0, 0x40);
    SceneMessages_RegisterReceiver(&npc->receiver, name, 5);
    npc->body.direction = angle;
    {
        u16 a = angle;
        {
            s32 t;
            u32 m;
            t = a + 0x70;
            t >>= 5;
            m = 7;
            m &= t;
            npc->facing8 = m;
        }
        {
            s32 t = (a + 0x60) >> 6;
            u32 m;
            m = 3;
            m &= t;
            npc->facing4 = m;
        }
    }
    Task_SetManager(Otenko_OnStateChange, (void **)&npc->animationCallback);
    Task_SetManager(sub_081D0864, &npc->callbacks[0]);
    Task_SetManager(Otenko_OnModeUpdate, &npc->callbacks[1]);
    Task_SetManager(Otenko_OnStateUpdate, &npc->callbacks[2]);
    Task_SetManager(Otenko_OnStateEffectUpdate, &npc->callbacks[3]);
    Task_SetManager(Otenko_OnDestroy, &npc->callbacks[4]);
    Actor_InitResources(manager, npc, 0x97D3, 0);
    Actor_LinkToManager(manager, npc);
    Actor_InitBehaviorFromScript(manager, npc);
    effect = &npc->effect;
    npc->commandFlags = zero;
    effect->projected = 0;
    switch (Script_GetKeywordValue('i', 0)) {
        case 0: {
            u8 one = 1;
            npc->stage = 0;
            npc->stageEntered = 0;
            npc->state = one;
            npc->timer = 0;
            npc->commandEntered = 1;
            update = Actor_OnIdleUpdate;
            npc->prevState = 2;
            npc->stateCallback = update;
            npc->stateFrame = 0;
            npc->animationEntered = 1;
            npc->animationPhase = 0;
            break;
        }

        case 1:
            npc->stage = 0;
            npc->stageEntered = 0;
            npc->state = 0;
            npc->timer = 0;
            npc->commandEntered = 1;
            update = Actor_OnHiddenIdleUpdate;
            npc->prevState = 1;
            npc->stateCallback = update;
            npc->stateFrame = 0;
            npc->animationEntered = 1;
            npc->animationPhase = 0;
            DisplayAnimation_SetFlags(&npc->display, 1);
            Actor_DisableShadow(npc);
            break;

        case 2:
            npc->controlled = 1;
            mode = 0x12;
            npc->stage = 0;
            npc->stageEntered = 0;
            npc->state = mode;
            npc->timer = 0;
            npc->commandEntered = 1;
            update = Otenko_FollowSceneTarget;
            npc->prevState = 0x10;
            npc->stateCallback = update;
            npc->stateFrame = 0;
            npc->animationEntered = 1;
            npc->animationPhase = 0;
            break;

        case 3:
            mode = 0x19;
            npc->stage = 0;
            npc->stageEntered = 0;
            npc->state = mode;
            npc->timer = 0;
            npc->commandEntered = 1;
            updateC = Otenko_PlayAnimation12;
            npc->prevState = 0x16;
            npc->stateCallback = updateC;
            npc->stateFrame = 0;
            npc->animationEntered = 1;
            npc->animationPhase = 0;
            break;

        case 4:
            mode = 6;
            npc->stage = 0;
            npc->stageEntered = 0;
            npc->state = mode;
            npc->timer = 0;
            npc->commandEntered = 1;
            updateB = Actor_OnConfiguredAnimationUpdate;
            npc->prevState = 7;
            npc->stateCallback = updateB;
            npc->stateFrame = 0;
            npc->animationEntered = 1;
            npc->animationPhase = 0;
            break;
    }

    Actor_SetMirroredFourWayAnimation(npc, 0, 0, 0, 0, 0);
    setBox(npc, 16, 20);
    npc->rangeXSquared = 0x100;
    npc->rangeZSquared = 0x190;
    effect->particleHandle = ParticlePool_Create();
    if (extra) {
        effect->attached = 1;
        Otenko_EnterProjectedIdle(manager, npc);
    }
}
