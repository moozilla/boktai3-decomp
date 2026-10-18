#include "global.h"
extern u32 gUnk_0300523C;
void sub_081681B8(u8 *);
void sub_08168D20(u8 *);
void PauseMenu_ApplySelectedEquipment(u8 *);
void PauseMenu_ApplySelectedVariant(u8 *);
void sub_081685B4(u8 *);
void Sound_RestartPlayer29And30Songs(void);
void Hud_SetUpdateCallback(u8 *, void *);
void sub_08167C84(void);
static inline void andm(u32 *a, u32 m)
{
    *a &= m;
}
void PauseMenu_FinishEquipmentSelection(u8 *p)
{
    sub_081681B8(p);
    sub_08168D20(p);
    PauseMenu_ApplySelectedEquipment(p);
    PauseMenu_ApplySelectedVariant(p);
    p[0xC73] = 0;
    sub_081685B4(p);
    Sound_RestartPlayer29And30Songs();
    andm(&gUnk_0300523C, ~4);
    Hud_SetUpdateCallback(p, sub_08167C84);
}
