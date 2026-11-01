#include "global.h"
#include "engine.h"
#define B(o) (*(u8 *)((u8 *)npc + (o)))
#define H(o) (*(u16 *)((u8 *)npc + (o)))
#define W(o) (*(u32 *)((u8 *)npc + (o)))
struct PackedVec {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
struct SceneNpc {
    u8 pad0[2];
    u8 field2;
    u8 field3;
    u8 mode;
    u8 pad5;
    u8 state;
    u8 pad7;
    u8 field8;
    u8 field9;
    u8 padA[6];
    u32 field10;
    u32 field14;
    u8 field18;
    u8 facing4;
    u8 facing8;
    u8 field1B;
    u8 pad1C[0x2CC - 0x1C];
    void *update;
    void *callbacks[6];
};

static inline void setHandler(void *fn, void **slot)
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

// MATCHING_CONCESSION: register-pinning
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
    Actor_InitShadow(npc, W(0x160), 1, 0, 0x40);
    SceneMessages_RegisterReceiver((u8 *)npc + 0x168, name, 5);
    B(0x65) = angle;
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
    {
        register u32 o6 asm("r6");
        u32 o2;
        {
            register void *h asm("r1");
            h = StateActor_EnterPendingState;
            o6 = 0x2E4;
            *(void **)((u8 *)npc + o6) = h;
            h = StateActor_OnUpdateDisplay;
            o2 = 0x2D0;
            *(void **)((u8 *)npc + o2) = h;
            h = StateActor_OnCommand;
            o6 -= 0x10;
            *(void **)((u8 *)npc + o6) = h;
            h = StateActor_OnUpdateState;
            o2 += 8;
            *(void **)((u8 *)npc + o2) = h;
            h = StateActor_OnRender;
            o6 += 8;
            *(void **)((u8 *)npc + o6) = h;
            h = StateActor_OnUnregisterReceiver;
            o2 += 8;
            *(void **)((u8 *)npc + o2) = h;
            asm("" ::"r"(o2));
        }
        if (Script_SeekToKeyword('b')) {
            *(u8 *)((u8 *)npc + (o6 -= 0xF0)) = Script_GetValue();
            B(0x1ED) = Script_GetValue();
            *(u16 *)((u8 *)npc + (o6 += 2)) = Script_GetValue();
            asm("" ::"r"(o6));
        } else {
            u32 offset = 0x1EC;
            *(u8 *)((u8 *)npc + offset) = 0;
            asm("" ::"r"(offset));
            o6 = 0x1ED;
            *(u8 *)((u8 *)npc + o6) = 1;
            offset = 0x1EE;
            *(u16 *)((u8 *)npc + offset) = 0;
            asm("" ::"r"(offset));
        }
    }
    Actor_InitResources(manager, npc, model, 0);
    Actor_LinkToManager(manager, npc);
    switch (Script_GetKeywordValue('i', 0)) {
        case 0: {
            register u32 o6 asm("r6") = 0x21C;
            u8 one;
            u32 ten;
            void (*fn)(void);
            *(u32 *)((u8 *)npc + o6) = 0x400;
            W(0x220) = 0x1000;
            ten = 0xA;
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = ten;
            npc->field14 = 0;
            one = 1;
            npc->field8 = one;
            fn = sub_0802F298;
            npc->state = ten;
            o6 += 0xB0;
            *(void **)((u8 *)npc + o6) = fn;
            asm("" ::"r"(o6));
            npc->field10 = 0;
            npc->field9 = one;
            npc->field18 = 0;
        } break;
        case 1: {
            void (*fn)(void);
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = 0;
            npc->field14 = 0;
            npc->field8 = 1;
            fn = Actor_OnHiddenIdleUpdate;
            npc->state = 1;
            npc->update = fn;
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
        } break;
    }
}
