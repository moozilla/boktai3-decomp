#include "global.h"
s32 Registry1745_GetSessionState(void *);
void ScrollingDialogue_HideImmediately(void);
void Task_Kill(void *);
void sub_0824923C(void *, u32);
struct S {
    u8 filler[0xFE4];
    u32 f;
};
s32 SaveMenu_Update(struct S *s)
{
    if (Registry1745_GetSessionState(s) == 0) {
        ScrollingDialogue_HideImmediately();
        Task_Kill(s);
        return -1;
    }
    if (s->f != 0)
        sub_0824923C(s, s->f);
    return 1;
}
