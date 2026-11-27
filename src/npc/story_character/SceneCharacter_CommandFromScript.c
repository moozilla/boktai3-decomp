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
struct SceneNpc *Actor_FindById(void *manager, s32 name);
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
    if (npc->kind == 0x1D) {
        struct SceneCharacterEffects *x = &npc->effects;
        x->fadeCounter = 0;
        x->fadeMode = tag;
        x->bankB = 0x34;
        if (sound == 1)
            sound = 0x174;
        else if (sound == 9)
            sound = 0x16F;
        else
            sound = 0x34;
        x->bankA = sound;
        x->tag = 0;
        {
            struct SceneActorBody *body = &npc->body;
            s32 speed = 400;
            body->positionOffset.x = 0;
            body->positionOffset.y = speed;
            body->positionOffset.z = 0;
        }
    }
    switch (Script_GetKeywordValue('i', 0)) {
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
    return 0;
}
