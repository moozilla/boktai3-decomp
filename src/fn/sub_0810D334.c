#include "global.h"

struct P {
    u8 f[0x13e8];
    void *cb;
};
s32 Registry1745_GetSessionState(void);
void ScrollingDialogue_HideImmediately(void);
void Task_Kill(void *);
void sub_0824923C(void *, void *);

s32 Registry30AC_Update(struct P *p)
{
    if (Registry1745_GetSessionState() == 0) {
        ScrollingDialogue_HideImmediately();
        Task_Kill(p);
        return -1;
    }
    if (p->cb != 0)
        sub_0824923C(p, p->cb);
    return 1;
}
