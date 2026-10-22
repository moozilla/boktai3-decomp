#include "global.h"
s32 Callback_InvokeActor(void *arg, void *function);
struct T {
    u8 pad[0xc];
    void *cb;
    u16 pad10;
    u16 flags;
};
s32 sub_0821A0CC(struct T *p)
{
    s32 result;
    if (p->cb != NULL)
        result = Callback_InvokeActor(p, p->cb);
    else
        result = 0;
    if (result >= 0) {
        p->cb = NULL;
        p->flags |= 1;
    }
    return result;
}
