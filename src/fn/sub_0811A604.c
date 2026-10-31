#include "global.h"
void MotionActor_DispatchRequestedState(void *);
void Glove_UpdatePostState(void *);
void Callback_InvokeActor(void *, u32);
u32 Glove_UpdateTask(u8 *p)
{
    MotionActor_DispatchRequestedState(p);
    Callback_InvokeActor(p, *(u32 *)(p + 0x86C));
    Glove_UpdatePostState(p);
    return 0;
}
