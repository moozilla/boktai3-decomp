#include "global.h"

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 CollectionMenu_Init(void *, void *);
void Task_Kill(void *);
void CollectionMenu_Update(void);
void CollectionMenu_OnTaskDestroy(void);

void *CollectionMenu_Create(void *a)
{
    void *p = Task_Create(8, 0xba8);
    if (p != 0) {
        Task_SetCallbacks(p, CollectionMenu_Update, CollectionMenu_OnTaskDestroy);
        if (CollectionMenu_Init(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
