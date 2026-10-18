#include "global.h"

void CharacterModel_RemoveAnimations(u8 *);
void sub_0811B394(u8 *);
void ActorMarkerPool_RemoveMarker(s32);

void ScenePool_DestroyActorSlot(u32 a, u8 *p)
{
    CharacterModel_RemoveAnimations(p + 0x600);
    *(s32 *)(p + 0x854) = -1;
    sub_0811B394(p + 0x860);
    ActorMarkerPool_RemoveMarker(*(s8 *)(p + 0x850));
}
