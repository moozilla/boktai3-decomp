#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 RegistryE7BC_InitManager(void *);
void Task_Kill(void *);
void sub_0811B7A0(void *);
void RegistryE7BC_OnManagerDestroy(void *);
void *RegistryE7BC_CreateManager(void)
{
    void *p = Task_Create(0x8, 0x378);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0811B7A0, RegistryE7BC_OnManagerDestroy);
        if (RegistryE7BC_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
