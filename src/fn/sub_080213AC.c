#include "global.h"

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 SceneCommands_TaskInit(void *, void *);
void Task_Kill(void *);
void SceneCommands_TaskUpdate(void);
void SceneCommands_TaskDestroy(void);

void *SceneCommands_StartTask(void *a)
{
    void *p = Task_Create(3, 0x5C);
    if (p != 0) {
        Task_SetCallbacks(p, SceneCommands_TaskUpdate, SceneCommands_TaskDestroy);
        if (SceneCommands_TaskInit(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
