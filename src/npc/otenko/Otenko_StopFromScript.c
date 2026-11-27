#include "npc/otenko/otenko_actor.h"
#include "engine/engine.h"
#include "engine/ram.h"
struct OtenkoActor *Actor_FindById(void *manager, s32 name);
void DisplayAnimation_SetFlags(void *, s32);
void Actor_DisableShadow(struct OtenkoActor *);
void Otenko_UpdateStopped(void);
/* Registry routine 6CBE: puts scene NPC -n (default: the last created) into
   mode 0x11, state 15 (update Otenko_UpdateStopped) and resets its animation
   (tentative: stop and stand). */
void Otenko_StopFromScript(void)
{
    struct OtenkoActor *npc;
    u8 mode;
    void (*update)(void);
    if (!gSceneNpc)
        return;
    if (Script_SeekToKeyword('n')) {
        void *manager = gSceneActorListManager;
        if (!manager)
            return;
        npc = Actor_FindById(manager, Script_GetKeywordValue('n', 0));
        if (!npc)
            return;
    } else
        npc = (struct OtenkoActor *)gSceneNpc;
    mode = 0x11;
    npc->stage = 0;
    npc->stageEntered = 0;
    npc->state = mode;
    npc->timer = 0;
    npc->commandEntered = 1;
    update = Otenko_UpdateStopped;
    npc->prevState = 15;
    npc->stateCallback = update;
    npc->stateFrame = 0;
    npc->animationEntered = 1;
    npc->animationPhase = 0;
    DisplayAnimation_SetFlags(&npc->display, 1);
    Actor_DisableShadow(npc);
}
