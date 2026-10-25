#include "global.h"
#include "engine.h"
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
    u8 pad1B[5];
    struct Rig rig;
    u8 pad30[0x2C];
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
extern void *gUnk_02000480;
extern struct SceneNpc *gUnk_02000274;
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

/* Registry routine BB34: creates a scene NPC (0x384-byte object from
   Actor_Allocate) named -n, variant -k (default 1), at -p x y z facing -d, with
   behavior -i (0-4: update callback, mode and state), records it in
   gUnk_02000274 and registers it with the scene actor manager; -K also runs
   SceneNpc_EnterProjectedIdle on it. The facing computation needs register pins and an empty
   asm barrier (agbcc otherwise turns the arithmetic shift into a logical one). */
void SceneNpc_CreateFromScript(void)
{
    void (*update)(void);
    u8 mode;
    void (*updateB)(void);
    void *manager = gUnk_02000480;
    s32 name;
    s32 variant;
    s32 angle;
    s32 extra;
    struct SceneNpc *npc;
    void (*updateC)(void);
    struct PackedVec position;
    u32 zero;
    s32 model;
    struct Rig *rig;
    struct Motion *motion;
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
    gUnk_02000274 = npc;
    if (Actor_InitMotion(npc, name, &position, angle) < 0) {
        return;
    }
    model = 0x97D3;
    if (Actor_InitDrawable(npc, 1, model, 0xC6A0, zero = 0) < 0) {
        return;
    }
    motion = &npc->motion;
    {
        s32 speed = 400;
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
    setHandler(SceneNpc_OnStateChange, &npc->callbacks[5]);
    setHandler(sub_081D0864, &npc->callbacks[0]);
    setHandler(SceneNpc_OnModeUpdate, &npc->callbacks[1]);
    setHandler(SceneNpc_OnStateUpdate, &npc->callbacks[2]);
    setHandler(SceneNpc_OnStateEffectUpdate, &npc->callbacks[3]);
    setHandler(SceneNpc_OnDestroy, &npc->callbacks[4]);
    Actor_InitResources(manager, npc, model, 0);
    Actor_LinkToManager(manager, npc);
    Actor_InitBehaviorFromScript(manager, npc);
    rig = &npc->rig;
    npc->fieldC = zero;
    rig->fieldC = 0;
    switch (Script_GetKeywordValue('i', 0)) {
        case 0: {
            u8 one = 1;
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = one;
            npc->field14 = 0;
            npc->field8 = 1;
            update = Actor_OnIdleUpdate;
            npc->state = 2;
            npc->update = update;
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            break;
        }

        case 1:
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = 0;
            npc->field14 = 0;
            npc->field8 = 1;
            update = Actor_OnHiddenIdleUpdate;
            npc->state = 1;
            npc->update = update;
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            DisplayAnimation_SetFlags(npc->blockD4);
            Actor_DisableShadow(npc);
            break;

        case 2:
            npc->field21B = 1;
            mode = 0x12;
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = mode;
            npc->field14 = 0;
            npc->field8 = 1;
            update = SceneNpc_FollowSceneTarget;
            npc->state = 0x10;
            npc->update = update;
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            break;

        case 3:
            mode = 0x19;
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = mode;
            npc->field14 = 0;
            npc->field8 = 1;
            updateC = SceneNpc_PlayAnimation12;
            npc->state = 0x16;
            npc->update = updateC;
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
            updateB = Actor_OnConfiguredAnimationUpdate;
            npc->state = 7;
            npc->update = updateB;
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            break;
    }

    Actor_SetMirroredFourWayAnimation(npc, 0, 0, 0, 0, 0);
    setBox(npc, 16, 20);
    npc->field200 = 0x100;
    npc->field204 = 0x190;
    rig->handle = ParticlePool_Create();
    if (extra) {
        rig->attached = 1;
        SceneNpc_EnterProjectedIdle(manager, npc);
    }
}
