
#ifdef NONMATCHING
// fuzzy: 95.2%
// foundry draft (worker best 95.2%); what differs is mostly register allocation and instruction order
#define PlayerContact_UpdateThresholdEvent sub_0815EFF8
#define gRandomTableIndex                  gUnk_03005308
#define gRandomTable                       gUnk_0203B400
#include "global.h"
#include "player.h"
#include "game_state.h"
extern u32 gUnk_030053F4, gUnk_03005308;
extern const u16 gUnk_0203B400[];
s32 Mod(s32, s32);
void Player_CallEventScript(struct Player *, u32);
void *PlayerBurst_Create(void *);
static inline u16 nextRandom(void)
{
    const u16 *table;
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    table = gUnk_0203B400;
    offset += (u32)table;
    return *(const u16 *)offset;
}
void sub_0815EFF8(struct Player *p)
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
clearReady:
    *(u8 *)((u8 *)p + 0x53C) = zero;
clearPending:
    *(u8 *)((u8 *)p + 0x53B) = zero;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0815EFF8);
#endif
