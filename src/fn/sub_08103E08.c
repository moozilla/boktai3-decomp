#include "global.h"

#define NamedObject_ReleaseFromScript sub_08103E08
extern u8 *gUnk_020001AC;
extern u8 *gUnk_02000580[];
u8 *sub_08101A74(void);
void sub_08159424(u8 *, s32);
static inline void clearBits(s16 *p, s32 mask)
{
    *p &= mask;
}
/* Script: undoes a named object's follow (-f in NamedObject_CreateFromScript):
   clears the manager's +0x1370, resets the player's +0x40, sub_08159424 and
   clears the follow flags (tentative). */
void NamedObject_ReleaseFromScript(void)
{
    u8 *manager = gUnk_020001AC;
    u8 *e = sub_08101A74();
    *(u32 *)(manager + 0x1370) = 0;
    {
        u8 *player = gUnk_02000580[0];
        *(u8 **)(player + 0x40) = player + 0x284;
        sub_08159424(player, 1);
    }
    clearBits((s16 *)(e + 0xB8), ~8);
    gUnk_02000580[0][0x701] &= 0xFE;
}
