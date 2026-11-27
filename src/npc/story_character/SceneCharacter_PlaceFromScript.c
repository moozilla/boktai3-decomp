#include "npc/story_character/scene_character.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/task_manager.h"
struct PackedVec {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
s32 SceneCharacter_GetModelId(s32);
s32 SceneCharacter_GetPaletteIndex(s32);
s32 SceneCharacter_GetResourceVariant(s32);
struct SceneNpc *Actor_Allocate(s32 name, s32 variant);
s32 Actor_InitMotion(struct SceneNpc *, s32 name, struct PackedVec *, s32 angle);
s32 Actor_InitDrawable(struct SceneNpc *, s32 kind, s32 a, s32 b, s32 c);
void Actor_SetDefaultPaletteBank(struct SceneNpc *, s32);
void Actor_InitShadow(struct SceneNpc *, u32, s32, s32, s32);
void SceneCharacter_RegisterMessageReceiver(struct SceneNpc *, s32);
void Actor_InitResources(void *, struct SceneNpc *, s32, s32);
void Actor_LinkToManager(void *, struct SceneNpc *);
void Actor_InitBehaviorFromScript(void *, struct SceneNpc *);
void DisplayAnimation_SetFlags(void *, u32);
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
void StoryCharacter_SimpleStep(void);
void StoryCharacter_UpdateTargetTurn(void);
void StoryCharacter_StartAnimStep(void);
void StoryCharacter_StartKind5Animation9AndAttach(void);
void StoryCharacter_StartAndMaybeFinishAnim(void);
/* Registry routine 8EA3 (GCL place_character): creates a scene character
   named -n of kind -k (default 9) at -p x y z facing -d. -C gives two
   values for kind 29 (a tag and a sound choice). The model, palette
   and setup depend on the kind; -i picks the starting behavior (0-8). */
void SceneCharacter_PlaceFromScript(void)
{
    s16 k2 = 0;
    s32 name;
    s32 kind;
    s32 extra;
    s32 model;
    void *manager;
    s32 colorA;
    s32 sound;
    s32 k, palette, key, flags, behavior;
    s32 angle;

    u8 mode;
    void (*updates[2])(void); /* two update temporaries; an array keeps agbcc's allocation */
    struct SceneNpc *npc;
    struct PackedVec position;
    colorA = 0;
    sound = 0x34;
    manager = gSceneActorListManager;
    if (!manager) {
        return;
    }
    name = Script_GetKeywordValue('n', 0);
    kind = Script_GetKeywordValue('k', 9);
    k = kind - 4;
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
    if (Script_SeekToKeyword('C')) {
        colorA = Script_GetValue();
        sound = Script_GetValue();
    }
    model = SceneCharacter_GetModelId(k);
    palette = SceneCharacter_GetPaletteIndex(k);
    extra = SceneCharacter_GetResourceVariant(k);
    if (kind == 0x2B) {
        flags = 1;
        key = 0x3D95;
    } else {
        flags = 2;
        key = 0;
    }
    npc = Actor_Allocate(name, kind);
    if (!npc) {
        return;
    }
    if (Actor_InitMotion(npc, name, &position, angle) < 0) {
        return;
    }
    if (Actor_InitDrawable(npc, flags, model, key, k2) < 0) {
        return;
    }
    if (palette >= 0) {
        if (k == 0x19) {
            CpuSet(gPaletteBankData + palette * 32, npc->workPalette, 0x04000008);
            npc->defaultPaletteBank = palette;
            npc->paletteBank = palette;
            {
                struct DisplayAnimationKeyframes *keyframes = &npc->display.resource.keyframes;
                keyframes->sprite.palette = npc->workPalette;
            }
            npc->display.resource.keyframes.sprite.paletteBank = palette;
        } else {
            Actor_SetDefaultPaletteBank(npc, palette);
        }
    }
    if (k == 0x19) {
        struct SceneCharacterEffects *x = &npc->effects;
        x->particleHandle = ParticlePool_Create();
        x->fadeCounter = 0;
        x->fadeMode = colorA;
        x->bankB = 0x34;
        if (sound == 1) {
            sound = 0x174;
        } else if (sound == 9) {
            sound = 0x16F;
        } else {
            sound = 0x34;
        }
        x->bankA = sound;
    } else if (k == 5) {
        struct SceneCharacterEffects *x = &npc->effects;
        x->tag = 1;
        x->fadeCounter = 0;
        x->fadeMode = 0;
    }
    Actor_InitShadow(npc, (u32)npc->display.position, 1, 0, 0x40);
    SceneCharacter_RegisterMessageReceiver(npc, name);
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
    Task_SetManager(SceneCharacter_OnStateChange, &npc->callbacks[5]);
    Task_SetManager(SceneNpc_UpdateCommands, &npc->callbacks[0]);
    Task_SetManager(SceneCharacter_OnCommandUpdate, &npc->callbacks[1]);
    Task_SetManager(Actor_TickCallbacks, &npc->callbacks[2]);
    Task_SetManager(SceneCharacter_UpdateDisplay, &npc->callbacks[3]);
    Task_SetManager(SceneCharacter_OnDestroy, &npc->callbacks[4]);
    Actor_InitResources(manager, npc, model, extra);
    Actor_LinkToManager(manager, npc);
    Actor_InitBehaviorFromScript(manager, npc);
    behavior = Script_GetKeywordValue('i', 0);

    switch (behavior) {
        case 0: {
            u8 one = 1;
            npc->stage = 0;
            npc->stageEntered = 0;
            npc->state = one;
            npc->timer = 0;
            npc->commandEntered = 1;
            updates[1] = Actor_OnIdleUpdate;
            npc->prevState = 2;
            npc->stateCallback = updates[1];
            npc->stateFrame = 0;
            npc->animationEntered = 1;
        }
            npc->animationPhase = 0;
            break;
        case 1:
            npc->stage = 0;
            npc->stageEntered = 0;
            npc->state = 0;
            npc->timer = 0;
            npc->commandEntered = 1;
            updates[1] = Actor_OnHiddenIdleUpdate;
            npc->prevState = 1;
            npc->stateCallback = updates[1];
            npc->stateFrame = 0;
            npc->animationEntered = 1;
            npc->animationPhase = 0;
            DisplayAnimation_SetFlags(&npc->display.kind, 1);
            Actor_DisableShadow(npc);
            break;
        case 2: {
            void (*fn)(void);
            npc->controlled = 1;
            if (npc->routeId >= 0) {
                mode = SceneCharacter_GetPathNodeState(npc);
                npc->stage = 0;
                npc->stageEntered = 0;
                npc->state = mode;
                npc->timer = 0;
                npc->commandEntered = 1;
                fn = Actor_OnIdleUpdate;
                npc->prevState = 2;
                npc->stateCallback = fn;
                npc->stateFrame = 0;
                npc->animationEntered = 1;
                npc->animationPhase = 0;
            } else {
                mode = 6;
                npc->stage = 0;
                npc->stageEntered = 0;
                npc->state = mode;
                npc->timer = 0;
                npc->commandEntered = 1;
                fn = Actor_OnConfiguredAnimationUpdate;
                npc->prevState = 7;
                npc->stateCallback = fn;
                npc->stateFrame = 0;
                npc->animationEntered = 1;
                npc->animationPhase = 0;
            }
            break;
        }
        case 4:
            mode = 6;
            npc->stage = 0;
            npc->stageEntered = 0;
            npc->state = mode;
            npc->timer = 0;
            npc->commandEntered = 1;
            updates[0] = Actor_OnConfiguredAnimationUpdate;
            npc->prevState = 7;
            npc->stateCallback = updates[0];
            npc->stateFrame = 0;
            npc->animationEntered = 1;
            npc->animationPhase = 0;
            break;
        case 6:
            if (npc->kind == 0x1D) {
                struct SceneActorBody *body;
                npc->effects.tag = 1;
                body = &npc->body;
                {
                    s32 speed = 0x210;
                    body->positionOffset.x = 0;
                    body->positionOffset.y = speed;
                    body->positionOffset.z = 0;
                }
                mode = 0x62;
                npc->stage = 0;
                npc->stageEntered = 0;
                npc->state = mode;
                npc->timer = 0;
                npc->commandEntered = 1;
                updates[1] = StoryCharacter_UpdateTargetTurn;
                npc->prevState = 0x5E;
                npc->stateCallback = updates[1];
                npc->stateFrame = 0;
                npc->animationEntered = 1;
                npc->animationPhase = 0;
            } else {
                mode = 6;
                npc->stage = 0;
                npc->stageEntered = 0;
                npc->state = mode;
                npc->timer = 0;
                npc->commandEntered = 1;
                updates[0] = Actor_OnConfiguredAnimationUpdate;
                npc->prevState = 7;
                npc->stateCallback = updates[0];
                npc->stateFrame = 0;
                npc->animationEntered = 1;
                npc->animationPhase = 0;
            }
            break;
        case 5:
            mode = 0x12;
            npc->stage = 0;
            npc->stageEntered = 0;
            npc->state = mode;
            npc->timer = 0;
            npc->commandEntered = 1;
            updates[0] = StoryCharacter_SimpleStep;
            npc->prevState = 0xF;
            npc->stateCallback = updates[0];
            npc->stateFrame = 0;
            npc->animationEntered = 1;
            npc->animationPhase = 0;
            break;
        case 7:
            if (npc->kind == 0x1D) {
                struct SceneActorBody *body;
                npc->effects.tag = 1;
                body = &npc->body;
                {
                    s32 speed = 0x210;
                    body->positionOffset.x = 0;
                    body->positionOffset.y = speed;
                    body->positionOffset.z = 0;
                }
                mode = 0x61;
                npc->stage = 0;
                npc->stageEntered = 0;
                npc->state = mode;
                npc->timer = 0;
                npc->commandEntered = 1;
                updates[1] = StoryCharacter_StartAnimStep;
                npc->prevState = 0x5D;
                npc->stateCallback = updates[1];
                npc->stateFrame = 0;
                npc->animationEntered = 1;
                npc->animationPhase = 0;
            } else {
                mode = 0x12;
                npc->stage = 0;
                npc->stageEntered = 0;
                npc->state = mode;
                npc->timer = 0;
                npc->commandEntered = 1;
                updates[0] = StoryCharacter_SimpleStep;
                npc->prevState = 0xF;
                npc->stateCallback = updates[0];
                npc->stateFrame = 0;
                npc->animationEntered = 1;
                npc->animationPhase = 0;
            }
            break;
        case 8:
            if (npc->kind == 5) {
                mode = 0x2A;
                npc->stage = 0;
                npc->stageEntered = 0;
                npc->state = mode;
                npc->timer = 0;
                npc->commandEntered = 1;
                updates[0] = StoryCharacter_StartKind5Animation9AndAttach;
                npc->prevState = 0x28;
                npc->stateCallback = updates[0];
                npc->stateFrame = 0;
                npc->animationEntered = 1;
                npc->animationPhase = 0;
            } else if (npc->kind == 0x1B) {
                mode = 0x4E;
                npc->stage = 0;
                npc->stageEntered = 0;
                npc->state = mode;
                npc->timer = 0;
                npc->commandEntered = 1;
                updates[0] = StoryCharacter_StartAndMaybeFinishAnim;
                npc->prevState = 0x4A;
                npc->stateCallback = updates[0];
                npc->stateFrame = 0;
                npc->animationEntered = 1;
                npc->animationPhase = 0;
            }
            break;
    }

    if (npc->display.kind == 1) {
        Actor_PlayAnimationEntry(npc, 0, 0, 0, 0);
    }
    SceneNpc_SetKindMotionVector(npc, kind);
}
