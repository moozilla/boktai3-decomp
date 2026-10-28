#include "burst_emitter.h"
void BurstEmitter_PlayAmbientSound(struct BurstEmitter *emitter)
{
    if (!(gEngineFlags & 0x800) && !emitter->context) {
        u32 frame = emitter->frame;
        u32 random = (gRandomIndex + 1) & 0x3FF;
        u32 period;
        gRandomIndex = random;
        random <<= 1;
        period = *(u16 *)(random + (u32)gRandomTable);
        period &= 3;
        period += 9;
        if (!Mod(frame, period))
            Sound_PlaySong(0x339);
    }
}
