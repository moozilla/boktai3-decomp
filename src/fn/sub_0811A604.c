#include "global.h"
void MotionActor_DispatchRequestedState(void *);
void sub_0811A158(void *);
void Callback_InvokeActor(void *, u32);
u32 Glove_UpdateTask(u8 *p)
{
    MotionActor_DispatchRequestedState(p);
    Callback_InvokeActor(p, *(u32 *)(p + 0x86C));
    sub_0811A158(p);
    return 0;
}
