#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 Hud_InitAlternateLayout(u8 *, u32);
void Task_Kill(u8 *);
void Hud_UpdateWithCallback(void);
void SceneHud_DestroyOwner(void);

u8 *Hud_CreateAlternateLayout(u32 a)
{
    u8 *p;
    if (gFieldHud != 0)
        return gFieldHud;
    p = Task_Create(0xc, 0x4DA0);
    if (p != 0) {
        Task_SetCallbacks(p, Hud_UpdateWithCallback, SceneHud_DestroyOwner);
        *(u16 *)(p + 0xA5A) = a;
        if (Hud_InitAlternateLayout(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
