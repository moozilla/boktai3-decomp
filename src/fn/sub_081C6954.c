#include "global.h"
#include "engine.h"
struct PackedVec {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
struct SceneNpc {
    u8 unknown0[2];
    u8 field2, field3, state, nextState, prevState, done, enterCommand, enter;
    u8 unknownA[6];
    u32 stateFrame, timer;
    u8 phase, facing4, facing8;
    u8 unknown1B[0x65 - 0x1B];
    u8 motionAngle;
    u8 unknown66[0x160 - 0x66];
    u32 shadow;
    u8 unknown164[4];
    u8 receiver[0x10];
    u8 unknown178[0x1EC - 0x178];
    u8 behavior, enabled;
    u16 behaviorValue;
    u8 unknown1F0[0x21C - 0x1F0];
    u32 parameter21C, parameter220;
    u8 unknown224[0x2CC - 0x224];
    void (*update)(void);
    void (*callbacks[6])(void);
};

static inline void setHandler(void (*fn)(void), void (**slot)(void))
{
    *slot = fn;
}

struct SceneNpc *Actor_Allocate(void *, s32 variant);
s32 Actor_InitMotion(struct SceneNpc *, void *, struct PackedVec *, s32 angle);
s32 Actor_InitDrawable(struct SceneNpc *, s32 kind, s32 a, s32 b, s32 c);
void Actor_InitShadow(struct SceneNpc *, u32, s32, s32, s32);
void SceneMessages_RegisterReceiver(void *, void *, s32);
void Actor_InitResources(void *, struct SceneNpc *, s32, s32);
void Actor_LinkToManager(void *, struct SceneNpc *);
void StateActor_EnterPendingState(void);
void StateActor_OnUpdateDisplay(void);
void StateActor_OnCommand(void);
void StateActor_OnUpdateState(void);
void StateActor_OnRender(void);
void StateActor_OnUnregisterReceiver(void);
void Actor_OnHiddenIdleUpdate(void);
void sub_0802F298(void);

// confidence=low: creates a state actor from script keywords (k variant, p position, d angle, b, i behavior).
s32 StateActor_CreateFromScriptEntry(void *manager, void *name)
{
    s32 model;
    s32 fail;
    s32 variant;
    s32 angle;
    struct SceneNpc *npc;
    struct PackedVec position;
    variant = Script_GetKeywordValue('k', 0);
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
    model = 0xE02E;
    fail = -1;
    npc = Actor_Allocate(name, variant);
    if (!npc)
        return fail;
    if (Actor_InitMotion(npc, name, &position, angle) < 0)
        return fail;
    if (Actor_InitDrawable(npc, 2, model, 0, 0) < 0)
        return fail;
    Actor_InitShadow(npc, npc->shadow, 1, 0, 0x40);
    SceneMessages_RegisterReceiver(npc->receiver, name, 5);
    npc->motionAngle = angle;
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
    setHandler(StateActor_EnterPendingState, &npc->callbacks[5]);
    setHandler(StateActor_OnUpdateDisplay, &npc->callbacks[0]);
    setHandler(StateActor_OnCommand, &npc->callbacks[1]);
    setHandler(StateActor_OnUpdateState, &npc->callbacks[2]);
    setHandler(StateActor_OnRender, &npc->callbacks[3]);
    setHandler(StateActor_OnUnregisterReceiver, &npc->callbacks[4]);
    if (Script_SeekToKeyword('b')) {
        npc->behavior = Script_GetValue();
        npc->enabled = Script_GetValue();
        npc->behaviorValue = Script_GetValue();
    } else {
        npc->behavior = 0;
        npc->enabled = 1;
        npc->behaviorValue = 0;
    }

    Actor_InitResources(manager, npc, model, 0);
    Actor_LinkToManager(manager, npc);
    switch (Script_GetKeywordValue('i', 0)) {
        case 0: {
            u8 one;
            u32 ten;
            void (*fn)(void);
            npc->parameter21C = 0x400;
            npc->parameter220 = 0x1000;
            ten = 0xA;
            npc->field2 = 0;
            npc->field3 = 0;
            npc->state = ten;
            npc->timer = 0;
            one = 1;
            npc->enterCommand = one;
            fn = sub_0802F298;
            npc->prevState = ten;
            npc->update = fn;
            npc->stateFrame = 0;
            npc->enter = one;
            npc->phase = 0;
        } break;
        case 1: {
            void (*fn)(void);
            npc->field2 = 0;
            npc->field3 = 0;
            npc->state = 0;
            npc->timer = 0;
            npc->enterCommand = 1;
            fn = Actor_OnHiddenIdleUpdate;
            npc->prevState = 1;
            npc->update = fn;
            npc->stateFrame = 0;
            npc->enter = 1;
            npc->phase = 0;
        } break;
    }
}
