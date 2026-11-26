#include "global.h"
#include "monsters/glove/glove.h"
void MotionActor_DispatchRequestedState(void *);
void Glove_UpdatePostState(void *);
u32 Glove_UpdateTask(struct Glove *p)
{
    MotionActor_DispatchRequestedState(p);
    Callback_InvokeActor(p, p->update);
    Glove_UpdatePostState(p);
    return 0;
}
