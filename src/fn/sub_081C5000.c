#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 sub_081C4EF0(void *);
void Task_Kill(void *);
void RegistryC281_Update(void *);
void RegistryC281_Destroy(void *);
void *RegistryC281_Create(void)
{
    void *p = Task_Create(0xb, 0x40);
    if (p != 0) {
        Task_SetCallbacks(p, RegistryC281_Update, RegistryC281_Destroy);
        if (sub_081C4EF0(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
