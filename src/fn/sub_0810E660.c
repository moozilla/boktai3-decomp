#define SceneTask_UpdateConditionalCallback sub_0810E660
#include "global.h"
s32 Registry1745_GetSessionState(void);
void ScrollingDialogue_HideImmediately(void);
void Task_Kill(void *);
void sub_0824923C(void *, u32);
extern u32 gUnk_030053F4, gUnk_030053F0;
struct SceneCallbackTask {
    u8 unknown[0xCE4];
    u32 callback;
};
s32 sub_0810E660(struct SceneCallbackTask *s)
{
    u32 mask = 0x800;
    if ((gUnk_030053F4 | gUnk_030053F0) & mask) {
        if (Registry1745_GetSessionState() == 0) {
            ScrollingDialogue_HideImmediately();
            Task_Kill(s);
            return -1;
        }
    }
    if (s->callback != 0)
        sub_0824923C(s, s->callback);
    return 1;
}
