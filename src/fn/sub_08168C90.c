#include "global.h"
extern u32 gUnk_0300523C;
void sub_081680F4(u8 *);
void sub_08168214(u8 *);
void Sound_SuspendPlayer29And30Songs(void);
void Hud_SetUpdateCallback(u8 *, void *);
void sub_08169228(void);
static inline void orm2(u32 m, u32 *a)
{
    *a |= m;
}
void PauseMenu_BeginEquipmentSelection(u8 *p)
{
    sub_081680F4(p);
    sub_08168214(p);
    Sound_SuspendPlayer29And30Songs();
    orm2(4, &gUnk_0300523C);
    Hud_SetUpdateCallback(p, sub_08169228);
}
