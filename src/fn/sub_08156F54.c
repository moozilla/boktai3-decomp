// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "player.h"
#include "tether_actor.h"

#define gTetherActor gUnk_02000488
/* The tether header retains a distinct legacy player view. */
#define gUnk_02000580 gTetherPlayerView
#undef gUnk_02000580
extern struct TetherActor *gUnk_02000488;
s32 TetherActor_TestMatchingStatus(u32, u32, u32, u32);
s32 Player_TestTetherActionInput(struct Player *p)
{
    if (*p->inputKeys & 1) {
        struct TetherActor **g;
        struct TetherActor *a;
        {
            register struct TetherActor **g0 asm("r0") = &gUnk_02000488;
            a = *g0;
            g = g0;
            asm("" : "+r"(g));
        }
        if (a && (a->state == 1 || a->state == 0x11)) {
            u32 t = p->tetherStatusTag;
            u32 f = p->tetherStatusFlags;
            asm("" ::: "memory");
            if (TetherActor_TestMatchingStatus(t, f, (*g)->statusTag, (*g)->statusFlags))
                return 1;
        }
    }
    return 0;
}
