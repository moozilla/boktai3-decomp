#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 TargetRegistry_Init(void *, void *, void *);
void Task_Kill(void *);
void TargetRegistry_Update(void);
void TargetRegistry_Destroy(void);
extern void *gUnk_02000598;

void *TargetRegistry_Create(void *a, void *b)
{
    void *r = gUnk_02000598;
    void *q;
    if (r == 0) {
        q = Task_Create(2, 0x20);
        if (q != 0) {
            Task_SetCallbacks(q, TargetRegistry_Update, TargetRegistry_Destroy);
            if (TargetRegistry_Init(q, a, b) < 0) {
                Task_Kill(q);
                return 0;
            }
        }
        r = q;
    }
    return r;
}
