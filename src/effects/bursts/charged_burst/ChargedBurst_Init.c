#include "effects/bursts/charged_burst.h"

void ChargedBurst_InitCoreEffect(struct ChargedBurst *);
void ChargedBurst_InitParticles(struct ChargedBurst *);
void ChargedBurst_InitCollider(struct ChargedBurst *, u32, s32, s32);
void Sound_PlaySong(u32);
void TimedDisplay_AdvanceIntro(struct ChargedBurst *);

s32 ChargedBurst_Init(struct ChargedBurst *burst, struct ContactPosition *position,
                      u32 contactFlags, u32 power, u32 b, u32 d)
{
    burst->position = *position;
    ChargedBurst_InitCoreEffect(burst);
    ChargedBurst_InitParticles(burst);
    ChargedBurst_InitCollider(burst, contactFlags, power, b);
    burst->unk_206 = 0;
    burst->unk_208 = d;
    Sound_PlaySong(920);
    ChargedBurst_SetState(burst, TimedDisplay_AdvanceIntro);
    return 0;
}
