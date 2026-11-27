#include "global.h"

void Centipede_UpdateSegmentColliders(u8 *);
void Tentacle_FireSecondaryAttack(u8 *);
void ActorTrail_UpdateTimedScale(u8 *);
void SpritePool_FlagEntries(u8 *);
void Centipede_ShiftSegmentTrail(u8 *);
void Centipede_UpdateSegmentImpulses(u8 *);
void Centipede_AlignSegmentsToTerrain(u8 *);
void Centipede_ResolveBlockedMovement(u8 *);

s32 Centipede_UpdateComponents(u8 *p)
{
    s16 *timer;
    Centipede_UpdateSegmentColliders(p);
    Tentacle_FireSecondaryAttack(p);
    ActorTrail_UpdateTimedScale(p);
    SpritePool_FlagEntries(p);
    Centipede_ShiftSegmentTrail(p);
    Centipede_UpdateSegmentImpulses(p);
    Centipede_AlignSegmentsToTerrain(p);
    Centipede_ResolveBlockedMovement(p);
    timer = (s16 *)(p + 0x8d6);
    if (*timer > 0)
        (*timer)--;
    return 1;
}
