#include "global.h"
#include "engine.h"
#include "ram.h"
struct PackedVec {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
struct CharacterExtra {
    u32 handle;
    u8 pad4[2];
    u16 field6;
    u8 pad8[8];
    u16 field10, field12;
    u32 field14, field18;
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
struct Display {
    u8 pad[0x68];
    u16 *palette;
};
struct SceneNpc {
    u8 pad0;
    u8 type;
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
    u8 pad1B;
    u16 palette1C, palette1E;
    struct CharacterExtra extra;
    u8 pad3C[0x20];
    struct Motion motion;
    u32 animMode;
    u8 padD8[8];
    struct Display display;
    u8 pad14C[0x14];
    u32 field160;
    u8 pad164[0x68];
    u8 colors[0x20];
    u8 pad1EC[7];
    s8 field1F3;
    u8 pad1F4[0x27];
    u8 field21B;
    u8 pad21C[0xB0];
    void *update;
    void *callbacks[6];
};
s32 SceneCharacter_GetModelId(s32);
s32 SceneCharacter_GetPaletteIndex(s32);
s32 SceneCharacter_GetResourceVariant(s32);
struct SceneNpc *Actor_Allocate(s32 name, s32 variant);
struct SceneNpc *Actor_FindById(void *manager, s32 name);
s32 Actor_InitMotion(struct SceneNpc *, s32 name, struct PackedVec *, s32 angle);
s32 Actor_InitDrawable(struct SceneNpc *, s32 kind, s32 a, s32 b, s32 c);
void Actor_SetDefaultPaletteBank(struct SceneNpc *, s32);
void Actor_InitShadow(struct SceneNpc *, u32, s32, s32, s32);
void SceneCharacter_RegisterMessageReceiver(struct SceneNpc *, s32);
void Actor_InitResources(void *, struct SceneNpc *, s32, s32);
void Actor_LinkToManager(void *, struct SceneNpc *);
void Actor_InitBehaviorFromScript(void *, struct SceneNpc *);
void DisplayAnimation_SetFlags(void *);
void Actor_DisableShadow(struct SceneNpc *);
u8 SceneCharacter_GetPathNodeState(struct SceneNpc *);
u32 ParticlePool_Create(void);
void SceneNpc_SetKindMotionVector(struct SceneNpc *, s32);
void SceneCharacter_OnStateChange(void);
void SceneNpc_UpdateCommands(void);
void SceneCharacter_OnCommandUpdate(void);
void Actor_TickCallbacks(void);
void SceneCharacter_UpdateDisplay(void);
void SceneCharacter_OnDestroy(void);
void Actor_OnIdleUpdate(void);
void Actor_OnHiddenIdleUpdate(void);
void Actor_OnConfiguredAnimationUpdate(void);
void StateActor_SimpleStep(void);
void sub_081CC1B4(void);
void StateActor_StartAnimStep(void);
void StateActor_StartKind5Animation9AndAttach(void);
void StateActor_StartAndMaybeFinishAnim(void);
static inline void setHandler(void *fn, void **slot)
{
    *slot = fn;
}
/* Registry routine F9D7: commands the scene character named -n. For kind 29,
   -C sets its tag and sound choice (as in place_character) and resets its
   speed; -i restarts a behavior as in SceneCharacter_PlaceFromScript. Returns
   -1 when there is no such character. */
s32 SceneCharacter_CommandFromScript(void)
{
    s32 tag = 0;
    s32 sound = 0x34;
    void *manager = gSceneActorListManager;
    struct SceneNpc *npc;
    u8 mode;
    void (*updates[2])(void);
    if (!manager || !(npc = Actor_FindById(manager, Script_GetKeywordValue('n', 0))))
        return -1;
    if (Script_SeekToKeyword('C')) {
        tag = Script_GetValue();
        sound = Script_GetValue();
    }
    if (npc->type == 0x1D) {
        struct CharacterExtra *x = &npc->extra;
        x->field12 = 0;
        x->field10 = tag;
        x->field14 = 0x34;
        if (sound == 1)
            sound = 0x174;
        else if (sound == 9)
            sound = 0x16F;
        else
            sound = 0x34;
        x->field18 = sound;
        x->field6 = 0;
        {
            struct Motion *motion = &npc->motion;
            s32 speed = 400;
            motion->field34 = 0;
            motion->field36 = speed;
            motion->field38 = 0;
        }
    }
    switch (Script_GetKeywordValue('i', 0)) {
        case 0: {
            u8 one = 1;
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = one;
            npc->field14 = 0;
            npc->field8 = 1;
            updates[1] = Actor_OnIdleUpdate;
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
            DisplayAnimation_SetFlags(&npc->animMode);
            Actor_DisableShadow(npc);
            break;
        case 2: {
            void (*fn)(void);
            npc->field21B = 1;
            if (npc->field1F3 >= 0) {
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
        }
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
        case 6:
            if (npc->type == 0x1D) {
                struct Motion *motion;
                npc->extra.field6 = 1;
                motion = &npc->motion;
                {
                    s32 speed = 0x210;
                    motion->field34 = 0;
                    motion->field36 = speed;
                    motion->field38 = 0;
                }
                mode = 0x62;
                npc->field2 = 0;
                npc->field3 = 0;
                npc->mode = mode;
                npc->field14 = 0;
                npc->field8 = 1;
                updates[1] = sub_081CC1B4;
                npc->state = 0x5E;
                npc->update = updates[1];
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
                updates[0] = Actor_OnConfiguredAnimationUpdate;
                npc->state = 7;
                npc->update = updates[0];
                npc->field10 = 0;
                npc->field9 = 1;
                npc->field18 = 0;
            }
            break;
        case 5:
            mode = 0x12;
            npc->field2 = 0;
            npc->field3 = 0;
            npc->mode = mode;
            npc->field14 = 0;
            npc->field8 = 1;
            updates[0] = StateActor_SimpleStep;
            npc->state = 0xF;
            npc->update = updates[0];
            npc->field10 = 0;
            npc->field9 = 1;
            npc->field18 = 0;
            break;
        case 7:
            if (npc->type == 0x1D) {
                struct Motion *motion;
                npc->extra.field6 = 1;
                motion = &npc->motion;
                {
                    s32 speed = 0x210;
                    motion->field34 = 0;
                    motion->field36 = speed;
                    motion->field38 = 0;
                }
                mode = 0x61;
                npc->field2 = 0;
                npc->field3 = 0;
                npc->mode = mode;
                npc->field14 = 0;
                npc->field8 = 1;
                updates[1] = StateActor_StartAnimStep;
                npc->state = 0x5D;
                npc->update = updates[1];
                npc->field10 = 0;
                npc->field9 = 1;
                npc->field18 = 0;
            } else {
                mode = 0x12;
                npc->field2 = 0;
                npc->field3 = 0;
                npc->mode = mode;
                npc->field14 = 0;
                npc->field8 = 1;
                updates[0] = StateActor_SimpleStep;
                npc->state = 0xF;
                npc->update = updates[0];
                npc->field10 = 0;
                npc->field9 = 1;
                npc->field18 = 0;
            }
            break;
        case 8:
            if (npc->type == 5) {
                mode = 0x2A;
                npc->field2 = 0;
                npc->field3 = 0;
                npc->mode = mode;
                npc->field14 = 0;
                npc->field8 = 1;
                updates[0] = StateActor_StartKind5Animation9AndAttach;
                npc->state = 0x28;
                npc->update = updates[0];
                npc->field10 = 0;
                npc->field9 = 1;
                npc->field18 = 0;
            } else if (npc->type == 0x1B) {
                mode = 0x4E;
                npc->field2 = 0;
                npc->field3 = 0;
                npc->mode = mode;
                npc->field14 = 0;
                npc->field8 = 1;
                updates[0] = StateActor_StartAndMaybeFinishAnim;
                npc->state = 0x4A;
                npc->update = updates[0];
                npc->field10 = 0;
                npc->field9 = 1;
                npc->field18 = 0;
            }
            break;
    }
    return 0;
}
