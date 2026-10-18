#include "global.h"
#define gSessionMenu gUnk_020004B4
#include "area_effect_link.h"
extern u32 gUnk_03005250, gUnk_030053F4, gUnk_0300546C;
void Serial_StopCommunication(void);
void Video_SetRenderCallbacks(s32, void (*)(void), void (*)(void), void (*)(void));
void sub_08217F4C(void);
void sub_08214B44(void);
void sub_08219624(void);
s32 TextWindow_Clear(s32);
s32 SessionMenu_Destroy(struct AreaEffectLinkState *p)
{
    Serial_StopCommunication();
    Video_SetRenderCallbacks(0, sub_08217F4C, sub_08214B44, sub_08219624);
    gUnk_03005250 = 0;
    gUnk_030053F4 &= ~0x800;
    if (p->menuTextWindow >= 0 && TextWindow_Clear(p->menuTextWindow) == 0)
        p->menuTextWindow = -1;
    gUnk_0300546C = 0;
    gUnk_020004B4 = 0;
    return 0;
}
