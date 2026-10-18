#include "global.h"
#define Hud_ResetGlobals sub_08177D54
#define gHudManager      gUnk_02000214
extern void *gUnk_02000214;
extern u16 gUnk_020005A4, gUnk_020005A0, gUnk_020005A8;
void sub_08177D54(void)
{
    gUnk_02000214 = 0;
    gUnk_020005A4 = 1;
    gUnk_020005A0 = 0;
    gUnk_020005A8 = 1;
}
