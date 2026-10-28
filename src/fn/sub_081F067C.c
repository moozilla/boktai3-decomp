#include "global.h"
#include "background_controller.h"

/* Tentative actor and motion-field interpretations, based on static accesses. */

struct SingletonSlot7Actor {
    u8 pad0[0xAC];
    u8 state, stateFrame, stateEntered, padAF;
    u8 active, padB1[2], motionMode;
    u8 padB4[0x10];
    u32 stateTimer;
    u8 padC8[0x268 - 0xC8];
    s32 worldX;
    u8 pad26C[0x278 - 0x26C];
    s16 localX;
    u8 pad27A[6];
    s32 velocity;
    u8 pad284[0x294 - 0x284];
    s32 targetX;
    u8 pad298[0x3A0 - 0x298];
    s32 player;
    u8 pad3A4[0x5D2 - 0x3A4];
    u16 flag5D2;
};
struct SingletonSlot7Origin {
    u8 pad0[0x44];
    s32 x;
};
/* The checker ELF predates this callee's promoted name. */
s32 Player_GetHorizontalVelocity(s32) __asm__("Player_GetHorizontalVelocity");

void SingletonSlot7_SetMotionMode(struct SingletonSlot7Actor *p, u32 mode)
{
    if (p->motionMode == 1)
        p->worldX = (p->localX + ((struct SingletonSlot7Origin *)gSceneCamera)->x) << 12;
    p->motionMode = mode;
    if (mode == 0) {
        p->targetX = p->localX;
        p->velocity = Player_GetHorizontalVelocity(p->player);
        {
            u32 entered = 1;
            p->state = 0;
            p->stateFrame = 0;
            p->stateEntered = entered;
            p->stateTimer = 0;
            p->active = entered;
        }
    } else {
        p->targetX = p->localX;
        p->flag5D2 = 1;
        {
            u32 entered = 3;
            p->state = 1;
            p->stateFrame = 0;
            p->stateEntered = entered;
            p->stateTimer = 0;
            p->active = 1;
        }
    }
}
