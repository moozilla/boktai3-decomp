#define gSceneNpcStateCallbacks gUnk_0861265C
#include "global.h"
#include "npc/story_character/state_actor.h"
extern void (*const gUnk_0861265C[])(void);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, int, int, void *, int, int, int);
s32 SceneNpc_EnterRequestedState(struct StateActor *p)
{
    struct StateActorTag *tag = &p->tag;
    s32 result;
    if ((u8)(p->nextState - 1) <= 0x17) {
        void (*callback)(void) = gUnk_0861265C[p->nextState];
        p->prevState = p->nextState;
        p->stateCallback = callback;
        p->stateFrame = 0;
        p->enter = 1;
        p->phase = 0;
        tag->attachment = ActorAttachment_UpdateFromDescriptor(tag->attachment, p->motion, 2, 0x7F,
                                                               0, 0, 0x80, 0);
        tag->field4 = 0;
        tag->field6 = 1;
        tag->state.word = 0;
        result = 1;
    } else
        result = 0;
    return result;
}
