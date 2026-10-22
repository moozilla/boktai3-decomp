#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
void Task_Kill(void *);
void MovingGlyph_UpdateTask(void *);
void MovingGlyph_DestroyTask(void *);
s32 sub_08237664(void *, u32, u32);

void *sub_08237764(u32 a, u32 b)
{
    void *task = Task_Create(9, 0x7c);
    if (task != NULL) {
        Task_SetCallbacks(task, MovingGlyph_UpdateTask, MovingGlyph_DestroyTask);
        if (sub_08237664(task, a, b) < 0) {
            Task_Kill(task);
            return NULL;
        }
    }
    return task;
}
