#include "global.h"
void sub_0811A030(void *);
void sub_0811A158(void *);
void Callback_InvokeActor(void *, u32);
u32 Glove_UpdateTask(u8 *p)
{
    sub_0811A030(p);
    Callback_InvokeActor(p, *(u32 *)(p + 0x86C));
    sub_0811A158(p);
    return 0;
}
