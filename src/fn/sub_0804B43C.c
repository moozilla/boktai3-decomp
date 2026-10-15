#include "tether_actor.h"
// MATCHING_CONCESSION: register-pinning
void TetherActor_SetState(struct TetherActor *, u32);
s32 Mod(s32, s32);
void TetherActor_UpdateLockedShake(struct TetherActor *actor)
{
    s32 amplitude;
    register const u16 *randomTable asm("r6");
    if (gUnk_030053F4 & 512)
        return;
    if (actor->lockFrame <= 63)
        amplitude = 16;
    else {
        actor->glowMode = 2;
        amplitude = (actor->lockFrame >> 2) + 16;
    }
    randomTable = (const u16 *)RANDOM_TABLE_ADDR;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    actor->shakeX = Mod(randomTable[gUnk_03005308], amplitude) - (amplitude >> 1);
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    actor->shakeZ = Mod(randomTable[gUnk_03005308], amplitude) - (amplitude >> 1);
    if ((++actor->lockSoundFrame & 7) == 0) {
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        {
            u32 sample = randomTable[gUnk_03005308];
            sample &= 1;
            if (sample)
                Sound_PlaySong(0x2B9);
        }
    }
    if (++actor->lockFrame > 127) {
        actor->interactionLock = 0;
        if (actor->state == 1)
            actor->transitionReason = 2;
        else
            actor->transitionReason = 1;
        TetherActor_SetState(actor, 2);
        actor->recoveryTimer = actor->recoveryDuration;
    }
}
