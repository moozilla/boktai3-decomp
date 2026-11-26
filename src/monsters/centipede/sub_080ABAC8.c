#include "global.h"

void TentacleActor_UpdateSegmentColliders(u8 *);
void Tentacle_FireSecondaryAttack(u8 *);
void ActorTrail_UpdateTimedScale(u8 *);
void SpritePool_FlagEntries(u8 *);
void TentacleActor_ShiftSegmentTrail(u8 *);
void sub_080AAA90(u8 *);
void TentacleActor_AlignSegmentsToTerrain(u8 *);
void TentacleActor_ResolveBlockedMovement(u8 *);

s32 TentacleActor_UpdateComponents(u8 *p)
{
    s16 *timer;
    TentacleActor_UpdateSegmentColliders(p);
    Tentacle_FireSecondaryAttack(p);
    ActorTrail_UpdateTimedScale(p);
    SpritePool_FlagEntries(p);
    TentacleActor_ShiftSegmentTrail(p);
    sub_080AAA90(p);
    TentacleActor_AlignSegmentsToTerrain(p);
    TentacleActor_ResolveBlockedMovement(p);
    timer = (s16 *)(p + 0x8d6);
    if (*timer > 0)
        (*timer)--;
    return 1;
}
