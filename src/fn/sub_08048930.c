#include "tether_actor.h"
#define TetherActor_UpdateShake sub_08048930
void TetherActor_UpdateShake(struct TetherActor *actor)
{
    u32 phase = actor->shakeFrame & 127;
    if (phase > 95) {
        s32 shake;
        if (phase == 96)
            Sound_PlaySong(0x2AA);
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        {
            u32 sample = ((const u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308];
            u32 mask = 1;
            shake = (sample & mask) * 11 - 5;
        }
        actor->shakeX = shake;
        actor->shakeZ = -shake;
    }
    ++actor->shakeFrame;
}
