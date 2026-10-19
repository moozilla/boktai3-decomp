#include "global.h"
#ifdef NONMATCHING
// fuzzy: 89.7%
// foundry draft (worker best 88.9%); what differs is mostly register allocation and instruction order
#define ActorAnim_WaitForIdleTransition sub_081E42C4
#define ActorAnim_UpdateIdleTransition Actor_UpdateIdlePhaseRestoration

struct ActorAnimController {
    u8 pad0[0xdfa];
    u16 angle;
    u8 pad1[0xe2c - 0xdfc];
    u16 frame;
    u8 pad2[4];
    u16 animTimer;
    u8 pad3[0xe78 - 0xe34];
    u16 count;
    u8 pad4[0xf58 - 0xe7a];
    u8 done, state;
    u16 mode : 4;
    u16 phase : 12;
    u16 oldMode;
    u8 pad5[0xf64 - 0xf5e];
    u8 flags;
    u8 pad6[0xf7a - 0xf65];
    u16 savedAngle;
    u8 pad7;
    u8 interrupted;
    u16 savedPhase;
    u8 pad8[4];
    u16 delay;
    u8 savedFrame, savedTimer;
    u8 pad9[8];
    void (*callback)(void *);
    u8 pad10[8];
    s32 idleTimer;
};
extern s32 gUnk_03005260;
extern u8 gUnk_03005430[];
void Actor_UpdateIdlePhaseRestoration(void *);

void sub_081E42C4(struct ActorAnimController *p)
{
    if (gUnk_03005260) {
        p->idleTimer = 0;
        if (gUnk_03005430[14] == 5 || gUnk_03005430[14] == 0 || gUnk_03005430[14] == 4)
            p->delay = 1800;
        else
            p->delay = 10800;
    }
    if (++p->idleTimer > p->delay && p->done) {
        p->interrupted = 0;
        *(u16 *)((u8 *)p + 0xf7e) = p->phase;
        p->phase = 5;
        if (p->oldMode == 3 && p->mode == 3) {
            p->savedFrame = p->frame;
            p->savedTimer = p->animTimer;
        }
        { u32 mask = 1; p->flags = mask | p->flags; }
        p->callback = Actor_UpdateIdlePhaseRestoration;
        p->count = 0;
        p->savedAngle = p->angle;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081E42C4);
#endif
