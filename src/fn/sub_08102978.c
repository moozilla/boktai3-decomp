#include "global.h"
#include "player.h"

struct S {
    u8 filler[0xb6];
    u16 h;
    u8 filler2[0xdc - 0xb8];
    u32 k;
};
void Player_AddAttachmentFlags(u8 *, u32, u32);
void Player_ClearAttachmentFlags(u8 *, u32);

void NamedObject_UpdatePlayerFollowFlags(struct S *s)
{
    if ((u16)(s->h - 3) > 1) {
        Player_AddAttachmentFlags((*(u8 **)gPlayers), 5, s->k);
    } else {
        Player_AddAttachmentFlags((*(u8 **)gPlayers), 1, s->k);
        Player_ClearAttachmentFlags((*(u8 **)gPlayers), 4);
    }
}
