#include "global.h"
#include "render.h"
#include "ram.h"
struct Task {
    u8 pad[16];
    u16 f10;
    u8 pad12[2];
    u8 f14, f15, f16, f17;
};
extern struct Task gUnk_03001668;
extern u32 gUnk_03005278;
extern u16 gNextTaskId;
void Heap_Initialize(void);
void TaskSystem_InitLists(void);
void ResourceRegistry_Initialize(void);
void MessageQueue_InitBuffers(void);
s32 TaskSystem_UpdateRoot(void);
void Task_SetCallbacks(struct Task *, s32 (*)(void), void (*)(void));
void Task_LinkToPriorityList(struct Task *);
void TaskSystem_Initialize(void)
{
    struct Task *task;
    Heap_Initialize();
    TaskSystem_InitLists();
    ResourceRegistry_Initialize();
    MessageQueue_InitBuffers();
    gUnk_03005278 = 123456;
    task = &gUnk_03001668;
    Task_SetCallbacks(task, TaskSystem_UpdateRoot, 0);
    task->f14 = 0;
    task->f16 = 1;
    task->f10 = 0;
    Task_LinkToPriorityList(task);
    gEnginePauseFlags = 0;
    gNextTaskId = 1;
    gLinkMenuActive = 0;
    {
        u16 *start = gSceneSelectionDisplaySlots;
        u32 value = 0x3FF;
        u16 *p = start;
        p += 3;
        do {
            *p = value;
            p--;
        } while ((s32)p >= (s32)start);
    }
}
