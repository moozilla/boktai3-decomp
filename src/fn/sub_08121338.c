#include "global.h"

#include "engine.h"
extern void *gUnk_0200020C;
extern u32 gUnk_030053F0;
u8 *sub_08120294(void *, s32);
void sub_0811B394(u8 *);
void sub_08020D08(u8 *);
/* Script: for the A8CB object named -n, removes its five animations
   (Anim_Remove), runs sub_0811B394 and, unless engine flag 0x800 is set,
   sub_08020D08 (tentative). */
void RegistryA8CB_ResetFromScript(void)
{
    void *manager = gUnk_0200020C;
    u8 *e;
    if (manager && (e = sub_08120294(manager, Script_GetKeywordValue('n', 0))) != 0) {
        u8 *p = e;
        u8 *s = e + 0x5F8;
        s32 i;
        for (i = 4; i >= 0; s += 0x60, i--)
            Anim_Remove(s);
        sub_0811B394(p + 0x56C);
        {
            u32 mask = 0x800;
            if (!((gUnk_030053F4 | gUnk_030053F0) & mask))
                sub_08020D08(e + 0x858);
        }
    }
}
