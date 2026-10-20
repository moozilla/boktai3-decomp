#include "global.h"
extern u32 gUnk_02000260;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 ControlPanel_InitializeFromScript(void *);
void Task_Kill(void *);
u32 ControlPanel_OnTaskUpdate(void *);
void SegmentDisplay_Destroy(void *);
void *ControlPanel_CreateFromScript(void)
{
    void *p;
    if (gUnk_02000260 != 0)
        return (void *)gUnk_02000260;
    p = Task_Create(8, 0x5bc);
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))ControlPanel_OnTaskUpdate, SegmentDisplay_Destroy);
        if (ControlPanel_InitializeFromScript(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
