// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
#define TetherActor_UpdateGlow sub_08048D70
void TetherActor_UpdateGlow(struct TetherActor *actor)
{
    switch (actor->glowMode) {
    case 0:
        if (actor->glowFrame) {
            s32 frame = actor->glowFrame;
            actor->palette[1] = (frame * 31) >> 6;
            --actor->glowFrame;
        } else
            actor->palette[1] = 0;
        break;
    case 1:
        {
            s32 frame;
            if (actor->glowFrame <= 63)
                frame = actor->glowFrame;
            else {
                register s32 current asm("r0") = actor->glowFrame;
                frame = 128 - current;
            }
            actor->palette[1] = (frame * 31) >> 6;
            ++actor->glowFrame;
            if (actor->glowFrame > 127)
                actor->glowFrame = 0;
        }
        break;
    case 2:
        if (actor->glowFrame <= 63) {
            s32 frame = actor->glowFrame;
            actor->palette[1] = (frame * 31) >> 6;
            ++actor->glowFrame;
        } else
            actor->palette[1] = 31;
        break;
    }
}
