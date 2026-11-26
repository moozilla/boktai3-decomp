#include "global.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 Glove_InitFromScript(u8 *);
void Task_Kill(u8 *);
void Glove_UpdateTask(void);
void MotionActor_Destroy(void);

u8 *Glove_CreateFromScript(void)
{
    u8 *p;
    p = Task_Create(6, 0x968);
    if (p != 0) {
        Task_SetCallbacks(p, Glove_UpdateTask, MotionActor_Destroy);
        if (Glove_InitFromScript(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
