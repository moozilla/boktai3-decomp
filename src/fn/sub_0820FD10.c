#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void sub_0820F378(void);
void OrangeSpine_OnTaskDestroy(void);
s32 OrangeSpine_InitializeFromScript(void *, void *);
void *OrangeSpine_Create(void *p)
{
    void *r = Task_Create(8, 0x198);
    if (r) {
        Task_SetCallbacks(r, sub_0820F378, OrangeSpine_OnTaskDestroy);
        if (OrangeSpine_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
