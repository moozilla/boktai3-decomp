
#ifdef NONMATCHING
// fuzzy: 88.5%
// foundry draft (worker best 86.3%); what differs is mostly register allocation and instruction order
#define Player_TestTetherActionInput sub_08156F54
#define gTetherActor gUnk_02000488
#include "global.h"
#include "player.h"
/* The tether header retains a distinct legacy player view. */
#define gUnk_02000580 gTetherPlayerView
#include "tether_actor.h"
#undef gUnk_02000580
extern struct TetherActor *gUnk_02000488;
s32 TetherActor_TestMatchingStatus(u32, u32, u32, u32);
s32 sub_08156F54(struct Player *p)
{
    if (*p->inputKeys & 1) {
        if (gUnk_02000488 && (gUnk_02000488->state == 1 || gUnk_02000488->state == 0x11)) {
            if (TetherActor_TestMatchingStatus(p->tetherStatusTag, p->tetherStatusFlags,
                gUnk_02000488->statusTag, gUnk_02000488->statusFlags))
                return 1;
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08156F54);
#endif
