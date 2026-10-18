#include "global.h"

extern u8 *gUnk_020001C4;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 LinkPanel_InitFromScript(u8 *);
void Task_Kill(u8 *);
void RegistryF246_Update(void);
void RegistryF246_Destroy(void);

u8 *RegistryF246_Create(void)
{
    u8 *p;
    if (gUnk_020001C4 != 0)
        return gUnk_020001C4;
    p = Task_Create(0xb, 0xf78);
    if (p != 0) {
        Task_SetCallbacks(p, RegistryF246_Update, RegistryF246_Destroy);
        if (LinkPanel_InitFromScript(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
