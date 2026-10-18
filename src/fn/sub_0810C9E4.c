#include "global.h"

struct P {
    u8 f[0xf40];
    void *cb;
};
s32 Registry1745_GetSessionState(void);
void ScrollingDialogue_HideImmediately(void);
void Task_Kill(void *);
void Callback_InvokeActor(void *, void *);

s32 RegistryF246_Update(struct P *p)
{
    if (Registry1745_GetSessionState() == 0) {
        ScrollingDialogue_HideImmediately();
        Task_Kill(p);
        return -1;
    }
    if (p->cb != 0)
        Callback_InvokeActor(p, p->cb);
    return 1;
}
