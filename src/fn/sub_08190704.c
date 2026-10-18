#include "global.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, u32, u32);
void sub_0818EBF4(u8 *);
u32 ScenePanelEffect_UpdateTask(u8 *);
u32 ScenePanelEffect_DestroyTask(u8 *);

u8 *ScenePanelEffect_Create(u32 a, u32 b)
{
    u8 *r4 = Task_Create(8, 0x53CC);
    if (r4 != 0) {
        Task_SetCallbacks(r4, (u32)ScenePanelEffect_UpdateTask, (u32)ScenePanelEffect_DestroyTask);
        sub_0818EBF4(r4 + 0x18);
        *(u16 *)(r4 + 0x53C8) = a;
        *(u16 *)(r4 + 0x53CA) = b;
    }
    return r4;
}
