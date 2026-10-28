#include "global.h"
#include "ram.h"
#include "player.h"

u8 *NamedObject_GetFollowTarget(void);
void Player_ClearAttachmentFlags(u8 *, s32);
static inline void clearBits(s16 *p, s32 mask)
{
    *p &= mask;
}
/* Script: undoes a named object's follow (-f in NamedObject_CreateFromScript):
   clears the manager's +0x1370, resets the player's +0x40, Player_ClearAttachmentFlags and
   clears the follow flags (tentative). */
void NamedObject_ReleaseFromScript(void)
{
    u8 *manager = gNamedActorManager;
    u8 *e = NamedObject_GetFollowTarget();
    *(u32 *)(manager + 0x1370) = 0;
    {
        u8 *player = ((u8 **)gPlayers)[0];
        *(u8 **)(player + 0x40) = player + 0x284;
        Player_ClearAttachmentFlags(player, 1);
    }
    clearBits((s16 *)(e + 0xB8), ~8);
    ((u8 **)gPlayers)[0][0x701] &= 0xFE;
}
