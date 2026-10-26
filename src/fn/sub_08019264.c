#include "global.h"
#include "ram.h"
u32 Script_GetKeywordValue(u32, u32);
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 CircleWindow_Initialize(void *, u32);
void Task_Kill(void *);
void CircleWindow_Update(void);
void CircleWindow_Destroy(void);
void *CircleWindow_Create(u32 a)
{
    void *p;
    u32 r = Script_GetKeywordValue(0x61, 0);
    if (gCircleWindow != 0)
        return (void *)gCircleWindow;
    if (r != 0) {
        p = Task_Create(0xb, 0x2b0);
    } else {
        p = Task_Create(9, 0x2b0);
    }
    if (p != 0) {
        Task_SetCallbacks(p, CircleWindow_Update, CircleWindow_Destroy);
        if (CircleWindow_Initialize(p, (u16)a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
