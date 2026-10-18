#define SceneNpc_EnterRequestedState sub_081D2544
#define gSceneNpcStateCallbacks      gUnk_0861265C
#include "global.h"
#include "state_actor.h"
extern void (*const gUnk_0861265C[])(void);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, int, int, void *, int, int, int);
s32 sub_081D2544(struct StateActor *p)
{
    struct StateActorTag *tag = &p->tag;
    s32 result;
    if ((u8)(p->nextState - 1) <= 0x17) {
        void (*callback)(void) = gUnk_0861265C[p->nextState];
        s32 zero = 0;
        s32 one;
        p->prevState = p->nextState;
        p->stateCallback = callback;
        p->stateFrame = zero;
        one = 1;
        p->enter = one;
        p->phase = zero;
        tag->attachment = ActorAttachment_UpdateFromDescriptor(tag->attachment, p->motion, 2, 0x7F,
                                                               0, 0, 0x80, 0);
        tag->field4 = zero;
        tag->field6 = one;
        tag->state.word = zero;
        result = 1;
    } else
        result = 0;
    return result;
}
