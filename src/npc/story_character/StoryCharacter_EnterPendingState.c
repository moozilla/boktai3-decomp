#include "npc/story_character/scene_character.h"

extern void (*gUnk_0861218C[])(void);

s32 StoryCharacter_EnterPendingState(struct SceneNpc *npc)
{
    s32 result;
    if (npc->nextState >= 1 && npc->nextState <= 14) {
        void (*callback)(void) = gUnk_0861218C[npc->nextState];
        npc->prevState = npc->nextState;
        npc->stateCallback = callback;
        npc->stateFrame = 0;
        npc->animationEntered = 1;
        npc->animationPhase = 0;
        result = 1;
    } else {
        result = 0;
    }
    return result;
}
