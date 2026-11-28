#include "effects/bursts/spiral_burst.h"

void SpiralBurst_InitParticles(struct SpiralBurst *);
void SpiralBurst_InitCollider(struct SpiralBurst *);
void SpiralBurst_UpdateParticles(struct SpiralBurst *);
void SpiralBurst_SetState(struct SpiralBurst *, void (*)(struct SpiralBurst *));
s32 Div(s32, s32);

s32 SpiralBurst_Init(struct SpiralBurst *burst, void *source, struct ContactPosition *origin)
{
    burst->origin = *origin;
    burst->source = source;
    SpiralBurst_InitParticles(burst);
    SpiralBurst_InitCollider(burst);
    burst->duration = 180;
    burst->unk_1FC = Div(burst->duration, 3);
    burst->soundBase = 900;
    Sound_PlaySong(burst->soundBase + 19);
    SpiralBurst_SetState(burst, SpiralBurst_UpdateParticles);
    return 0;
}
