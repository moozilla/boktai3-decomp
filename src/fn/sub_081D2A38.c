#include "global.h"
#include "engine.h"
#include "ram.h"
struct PackedVec {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
struct Rig {
    u8 pad0[4];
    u32 handle;
    u8 pad8[4];
    u8 fieldC;
    u8 attached;
    u8 padE[2];
};
struct Motion {
    u8 pad0[9];
    u8 angle;
    u8 padA[0x2A];
    u16 field34;
    u16 field36;
    u16 field38;
    u8 pad3A[0x3E];
};
struct Tagged {
    u16 tag;
    u8 pad2[0x14];
    u16 field16;
    u32 field18, field1C;
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
    u8 padA[2];
    u16 fieldC;
    u8 padE[2];
    u32 field10;
    u32 field14;
    u8 field18;
    u8 facing4;
    u8 facing8;
    u8 field1B;
    u8 pad1C[4];
    struct Tagged tagged;
    u8 pad40[0x1C];
    struct Motion motion;
    u8 blockD4[0x8C];
    u32 field160;
    u8 pad164[4];
    u8 block168[0x8C];
    u16 field1F4;
    u16 field1F6;
    u32 field1F8;
    u32 field1FC;
    u32 field200;
    u32 field204;
    u8 pad208[0x13];
    u8 field21B;
    u8 pad21C[0xB0];
    void *update;
    void *callbacks[6];
};
struct SceneNpc *Actor_Allocate(s32 name, s32 variant);
s32 Actor_InitMotion(struct SceneNpc *, s32 name, struct PackedVec *, s32 angle);
s32 Actor_InitDrawable(struct SceneNpc *, s32 kind, s32 a, s32 b, s32 c);
void Actor_InitShadow(struct SceneNpc *, u32, s32, s32, s32);
void SceneMessages_RegisterReceiver(void *, s32, s32);
void Actor_InitResources(void *, struct SceneNpc *, s32, s32);
void Actor_LinkToManager(void *, struct SceneNpc *);
void Actor_InitBehaviorFromScript(void *, struct SceneNpc *);
void DisplayAnimation_SetFlags(void *);
void Actor_DisableShadow(struct SceneNpc *);
s32 Actor_SetMirroredFourWayAnimation(struct SceneNpc *, s32, s32, s32, s32, s32);
u32 ParticlePool_Create(void);
void SceneNpc_EnterProjectedIdle(void *, struct SceneNpc *);
void SceneNpc_OnStateChange(void);
void sub_081D0864(void);
void SceneNpc_OnModeUpdate(void);
void SceneNpc_OnStateUpdate(void);
void SceneNpc_OnStateEffectUpdate(void);
void SceneNpc_OnDestroy(void);
void Actor_OnIdleUpdate(void);
void Actor_OnHiddenIdleUpdate(void);
void SceneNpc_FollowSceneTarget(void);
void SceneNpc_PlayAnimation12(void);
void Actor_OnConfiguredAnimationUpdate(void);
void SceneNpc_EnterRequestedState(void);
void TaggedSceneNpc_OnFrameUpdate(void);
void TaggedSceneNpc_OnBehaviorUpdate(void);
void SceneNpc_TickStateAndPalette(void);
void TaggedSceneNpc_OnVisibilityUpdate(void);
void TaggedSceneNpc_DispatchReceiverCallback(void);
void sub_081D1A44(void);
void TaggedSceneNpc_UpdateState17AnimationA(void);
u8 SceneCharacter_GetPathNodeState(struct SceneNpc *);
static inline void setHandler(void *fn, void **slot)
{
    *slot = fn;
}

static inline void setBox(struct SceneNpc *npc, s32 width, s32 height)
{
    npc->field1F4 = width;
    npc->field1F6 = height;
    npc->field1F8 = width;
    npc->field1FC = height;
}

/* Creates a scene NPC (model E00E) named -n, variant -k (default 2), at -p
   facing -d, keeping tag (0 or 1, from its two registry callers) at +0x20;
   behavior -i (0, 1, 2, 4, 8) as in SceneNpc_CreateFromScript. */
void TaggedSceneNpc_Create(u32 tag)
{
    // The model id as one local for both calls; with the literal in each call
    // agbcc kept it in sb (found by tools/translate.py).
    s32 model = 0xE00E;
    void (*updates[2])(void);
    u8 mode;
    void *manager = gSceneActorListManager;
    s32 name;
    s32 variant;
    s32 angle;
    struct SceneNpc *npc;
    struct PackedVec position;
    u32 zero;
    struct Motion *motion;
    if (!manager)
        return;
    name = Script_GetKeywordValue('n', 0);
    variant = Script_GetKeywordValue('k', 2);
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
    zero = 0;
    npc = Actor_Allocate(name, variant);
    if (!npc)
        return;
    if (Actor_InitMotion(npc, name, &position, angle) < 0)
        return;
    if (Actor_InitDrawable(npc, 2, model, 0, zero) < 0)
        return;
    {
        struct Tagged *t = &npc->tagged;
        npc->tagged.tag = tag;
        t->field16 = zero;
        t->field18 = zero;
        t->field1C = zero;
    }
    motion = &npc->motion;
    {
        s32 speed = 200;
        motion->field34 = zero;
        motion->field36 = speed;
        motion->field38 = zero;
    }
    Actor_InitShadow(npc, npc->field160, 1, 0, 0x40);
    SceneMessages_RegisterReceiver(npc->block168, name, 5);
    npc->motion.angle = angle;
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
    setHandler(SceneNpc_EnterRequestedState, &npc->callbacks[5]);
    setHandler(TaggedSceneNpc_OnFrameUpdate, &npc->callbacks[0]);
    setHandler(TaggedSceneNpc_OnBehaviorUpdate, &npc->callbacks[1]);
    setHandler(SceneNpc_TickStateAndPalette, &npc->callbacks[2]);
    setHandler(TaggedSceneNpc_OnVisibilityUpdate, &npc->callbacks[3]);
    setHandler(TaggedSceneNpc_DispatchReceiverCallback, &npc->callbacks[4]);
    npc->field1B = angle;
    Actor_InitResources(manager, npc, model, 0);
    Actor_LinkToManager(manager, npc);
    Actor_InitBehaviorFromScript(manager, npc);
    switch (Script_GetKeywordValue('i', 0)) {
        case 0: {
            u8 one = 1;
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = one;
            npc->field14 = 0;
            npc->field8 = 1;
            updates[1] = sub_081D1A44;
            npc->state = 2;
            npc->update = updates[1];
            npc->field10 = 0;
            npc->field9 = 1;
        }
            npc->field18 = 0;
            break;
        case 1:
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = 0;
            npc->field14 = 0;
            npc->field8 = 1;
            updates[1] = Actor_OnHiddenIdleUpdate;
            npc->state = 1;
            npc->update = updates[1];
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            DisplayAnimation_SetFlags(npc->blockD4);
            Actor_DisableShadow(npc);
            break;
        case 2: {
            void (*fn)(void);
            npc->field21B = 1;
            if ((s8)npc->block168[0x8B] >= 0) {
                mode = SceneCharacter_GetPathNodeState(npc);
                npc->field2 = 0;
                npc->field3 = 0;
                npc->mode = mode;
                npc->field14 = 0;
                npc->field8 = 1;
                fn = Actor_OnIdleUpdate;
                npc->state = 2;
                npc->update = fn;
                npc->field10 = 0;
                npc->field9 = 1;
                npc->field18 = 0;
            } else {
                mode = 6;
                npc->field2 = 0;
                npc->field3 = 0;
                npc->mode = mode;
                npc->field14 = 0;
                npc->field8 = 1;
                fn = Actor_OnConfiguredAnimationUpdate;
                npc->state = 7;
                npc->update = fn;
                npc->field10 = 0;
                npc->field9 = 1;
                npc->field18 = 0;
            }
            break;
        }
        case 8:
            mode = 0x15;
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = mode;
            npc->field14 = 0;
            npc->field8 = 1;
            updates[0] = TaggedSceneNpc_UpdateState17AnimationA;
            npc->state = 0x12;
            npc->update = updates[0];
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            break;
        case 4:
            mode = 6;
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = mode;
            npc->field14 = 0;
            npc->field8 = 1;
            updates[0] = Actor_OnConfiguredAnimationUpdate;
            npc->state = 7;
            npc->update = updates[0];
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            break;
    }
}
