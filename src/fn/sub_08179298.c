#include "global.h"
#include "engine.h"
#include "game_state.h"
void *sub_0817923C(u32, u32);
void *sub_081791E0(u32, u32);
void *sub_08178B3C(u32, u32);
static inline u8 testFlag(u32 mask, u32 *flags)
{
    if (*flags & mask)
        return TRUE;
    return FALSE;
}
/* Registry routine CC28: creates the field HUD/menu manager (0x02000214),
   choosing one of three creators by a game-state flag (+0x868 bit 7) and an
   engine flag (gUnk_030053F4 bit 11). */
void *Hud_Create(u32 a, u32 b)
{
    void *p;
    if (testFlag(0x80, (u32 *)((u8 *)gUnk_02000710 + 0x868)))
        p = sub_0817923C(a, b);
    else if (!(gUnk_030053F4 & 0x800))
        p = sub_081791E0(a, b);
    else
        p = sub_08178B3C(a, b);
    return p;
}
