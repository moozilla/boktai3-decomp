#define Input_UpdatePadStates sub_08219B24
#include "runtime_input.h"
#include "render.h"
#include "ram.h"
#include "runtime.h"
#include "engine_globals.h"

void SessionMenu_InvokeCallback(void);

void Wireless_UpdateController(void);

void Input_UpdatePadStates(void);

void MessageQueue_SwapBuffers(void);

s32 TaskSystem_UpdateRoot(void)
{
    SessionMenu_InvokeCallback();
    Wireless_UpdateController();
    Input_UpdatePadStates();
    MessageQueue_SwapBuffers();
    return 0;
}

void Heap_Initialize(void);

void TaskSystem_InitLists(void);

void ResourceRegistry_Initialize(void);

void MessageQueue_InitBuffers(void);

s32 TaskSystem_UpdateRoot(void);

void Task_SetCallbacks(struct RuntimeTask *, s32 (*)(void), void (*)(void));

void Task_LinkToPriorityList(struct RuntimeTask *);

void TaskSystem_Initialize(void)
{
    struct RuntimeTask *task;
    Heap_Initialize();
    TaskSystem_InitLists();
    ResourceRegistry_Initialize();
    MessageQueue_InitBuffers();
    gLcgState = 123456;
    task = &gRootTask;
    Task_SetCallbacks(task, TaskSystem_UpdateRoot, 0);
    task->priority = 0;
    task->unk16 = 1;
    task->id = 0;
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

#define PAD_UPDATE(i, k)                                                                           \
    {                                                                                              \
        old = gPadState[i].held;                                                                   \
        gPadState[i].held = k;                                                                     \
        gPadState[i].pressed = k & ~old;                                                           \
        gPadState[i].released = old & ~k;                                                          \
    }

void Input_UpdatePadStates(void)
{
    s32 i;
    u16 keys, old;

    gHardwareKeys = REG_KEYINPUT;
    if (gLinkMenuActive == 0) {
        keys = 0x3FF ^ gHardwareKeys;
        PAD_UPDATE(0, keys);
        for (i = 1; i < 2; i++) {
            keys = 0;
            PAD_UPDATE(i, keys);
        }
    } else if (!(gEnginePauseFlags & 1)) {
        for (i = 0; i < 2; i++) {
            if (gSceneSelectionDisplaySlots[i] == 0xFFFF)
                keys = 0;
            else
                keys = 0x3FF ^ gSceneSelectionDisplaySlots[i];
            PAD_UPDATE(i, keys);
        }
    }
}

u16 String_Hash(u8 *s)
{
    u16 h = 0;
    while (*s != 0) {
        u32 a = h << 5;
        u32 b = h >> 11;
        h = a | b;
        h = h + *s;
        s++;
    }
    return h;
}

u32 Random_Next15(void)
{
    gLcgState = gLcgState * 0x5D588B65 + 1;
    return gLcgState & 0x7FFF;
}
