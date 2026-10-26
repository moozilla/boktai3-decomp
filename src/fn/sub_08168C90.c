#include "global.h"
#include "render.h"
void Hud_LoadConditionalPaletteRows(u8 *);
void sub_08168214(u8 *);
void Sound_SuspendPlayer29And30Songs(void);
void Hud_SetUpdateCallback(u8 *, void *);
void PauseMenu_UpdateEquipmentSelection(void);
static inline void orm2(u32 m, u32 *a)
{
    *a |= m;
}
void PauseMenu_BeginEquipmentSelection(u8 *p)
{
    Hud_LoadConditionalPaletteRows(p);
    sub_08168214(p);
    Sound_SuspendPlayer29And30Songs();
    orm2(4, &gEnginePauseFlags);
    Hud_SetUpdateCallback(p, PauseMenu_UpdateEquipmentSelection);
}
