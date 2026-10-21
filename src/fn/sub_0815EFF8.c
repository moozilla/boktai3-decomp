// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "player.h"
#include "game_state.h"

#define gRandomTableIndex gUnk_03005308
#define gRandomTable      gUnk_0203B400
extern u32 gUnk_030053F4, gUnk_03005308;
extern const u16 gUnk_0203B400[];
s32 Mod(s32, s32);
void Player_CallEventScript(struct Player *, u32);
void *PlayerBurst_Create(void *);
static inline u16 nextRandom(void)
{
    register u32 table asm("r2");
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    table = (u32)gUnk_0203B400;
    asm("" : "+r"(table));
    offset += table;
    return *(const u16 *)offset;
}
void PlayerContact_UpdateThresholdEvent(struct Player *p)
{
    u32 zero = gUnk_030053F4 & 0x1000;
    if (zero)
        return;
    if (p->actionMode != 1)
        goto clearReady;
    {
        if ((s32)gUnk_02000710->unk600 < gUnk_02000710->contactThreshold) {
            goto clearPending;
        } else if (p->contactThresholdReady) {
            if (p->tetherMode <= 1) {
                gUnk_02000710->unk600 = zero;
                gUnk_02000710->contactThreshold = Mod(nextRandom(), 100) + 200;
                p->contactThresholdReady = zero;
                Player_CallEventScript(p, 2);
                PlayerBurst_Create(p);
            }
        } else if (p->contactThresholdPending) {
            p->contactThresholdReady = 1;
            p->contactThresholdPending = zero;
        }
    }
    return;
clearReady: {
    register u32 off asm("r2") = 0x53C;
    register u8 *q asm("r0");
    asm("" : "+r"(off));
    q = (u8 *)p + off;
    *q = zero;
}
clearPending: {
    register u32 off asm("r1") = 0x53B;
    register u8 *q asm("r0");
    asm("" : "+r"(off));
    q = (u8 *)p + off;
    *q = zero;
}
}
