#include "global.h"

struct S {
    u8 filler[0xb6];
    u16 h;
    u8 filler2[0xdc - 0xb8];
    u32 k;
};
extern u8 *gUnk_02000580;
void Player_AddAttachmentFlags(u8 *, u32, u32);
void Player_ClearAttachmentFlags(u8 *, u32);

void NamedObject_UpdatePlayerFollowFlags(struct S *s)
{
    if ((u16)(s->h - 3) > 1) {
        Player_AddAttachmentFlags(gUnk_02000580, 5, s->k);
    } else {
        Player_AddAttachmentFlags(gUnk_02000580, 1, s->k);
        Player_ClearAttachmentFlags(gUnk_02000580, 4);
    }
}
